/*!
 * @file TextureCube.mm
 * @brief Implementation of Metal cubemap texture allocation, image decoding, and procedural generation.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/Textures/TextureCube.hpp"
#include "Core/Diagnostics/Log.hpp"
#include "Core/Diagnostics/Assert.hpp"
#include "Core/Application/Application.hpp"
#include "Core/Memory/MemoryTracking.hpp"

#include <stb_image.h>
#include <Metal/Metal.hpp>
#import <CoreGraphics/CoreGraphics.h>
#import <ImageIO/ImageIO.h>
#import <Foundation/Foundation.h>
#include <cmath>
#include <algorithm>

namespace KanVerse {
  
  class MetalTextureCube : public TextureCube {
  public:
    explicit MetalTextureCube(const std::vector<std::string>& faces) {
      KV_CORE_ASSERT(faces.size() == 6, "[KanVerse TextureCube] Cubemap requires exactly 6 face image paths!");
      
      MTL::Device* device = Application::Get().GetRenderContext()->GetDevice();
      KV_CORE_ASSERT(device != nullptr, "[KanVerse TextureCube] Active Metal device is null!");
      
      uint32_t resolution = 0;
      std::vector<std::vector<uint8_t>> facePixels(6);
      
      @autoreleasepool {
        for (size_t i = 0; i < 6; ++i) {
          NSString* pathString = [NSString stringWithUTF8String:faces[i].c_str()];
          NSURL* fileURL = [NSURL fileURLWithPath:pathString];
          
          CGImageSourceRef imageSource = CGImageSourceCreateWithURL((__bridge CFURLRef)fileURL, NULL);
          if (!imageSource) {
            KV_CORE_ERROR("TextureCube: Failed to open face image at '{}'", faces[i]);
            return;
          }
          
          CGImageRef image = CGImageSourceCreateImageAtIndex(imageSource, 0, NULL);
          CFRelease(imageSource);
          if (!image) {
            KV_CORE_ERROR("TextureCube: Failed to decode face image at '{}'", faces[i]);
            return;
          }
          
          const uint32_t w = static_cast<uint32_t>(CGImageGetWidth(image));
          const uint32_t h = static_cast<uint32_t>(CGImageGetHeight(image));
          if (i == 0) {
            resolution = w;
          } else {
            KV_CORE_ASSERT(w == resolution && h == resolution, "[KanVerse TextureCube] All cubemap faces must be square and matching dimensions!");
          }
          
          facePixels[i].resize(resolution * resolution * 4);
          CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
          CGContextRef context = CGBitmapContextCreate(
                                                       facePixels[i].data(),
                                                       resolution,
                                                       resolution,
                                                       8,
                                                       resolution * 4,
                                                       colorSpace,
                                                       static_cast<uint32_t>(kCGImageAlphaPremultipliedLast) | static_cast<uint32_t>(kCGBitmapByteOrder32Big)
                                                       );
          CGColorSpaceRelease(colorSpace);
          
          if (context) {
            CGRect rect = CGRectMake(0, 0, resolution, resolution);
            CGContextDrawImage(context, rect, image);
            CGContextRelease(context);
          }
          CGImageRelease(image);
        }
      }
      
      m_Resolution = resolution;
      
      MTL::TextureDescriptor* desc = MTL::TextureDescriptor::textureCubeDescriptor(
                                                                                   MTL::PixelFormatRGBA8Unorm_sRGB,
                                                                                   m_Resolution,
                                                                                   false
                                                                                   );
      desc->setUsage(MTL::TextureUsageShaderRead);
      desc->setStorageMode(MTL::StorageModeShared);
      
      m_Texture = device->newTexture(desc);
      desc->release();
      KV_CORE_ASSERT(m_Texture != nullptr, "[KanVerse TextureCube] Failed to allocate MTL::Texture for cubemap!");
      
      for (size_t i = 0; i < 6; ++i) {
        const MTL::Region region = MTL::Region(0, 0, m_Resolution, m_Resolution);
        const NSUInteger bytesPerRow = m_Resolution * 4;
        m_Texture->replaceRegion(region, 0, static_cast<NSUInteger>(i), facePixels[i].data(), bytesPerRow, bytesPerRow * m_Resolution);
      }
      
      const size_t vramBytes = m_Resolution * m_Resolution * 4 * 6;
      m_MemoryGuard.Reset(vramBytes);
      
      MTL::SamplerDescriptor* samplerDesc = MTL::SamplerDescriptor::alloc()->init();
      samplerDesc->setMinFilter(MTL::SamplerMinMagFilterLinear);
      samplerDesc->setMagFilter(MTL::SamplerMinMagFilterLinear);
      samplerDesc->setMipFilter(MTL::SamplerMipFilterNotMipmapped);
      samplerDesc->setSAddressMode(MTL::SamplerAddressModeClampToEdge);
      samplerDesc->setTAddressMode(MTL::SamplerAddressModeClampToEdge);
      m_Sampler = device->newSamplerState(samplerDesc);
      samplerDesc->release();
      
      KV_CORE_INFO("TextureCube: Successfully loaded cubemap (Resolution: {}x{}, {:.2f} MB)", m_Resolution, m_Resolution, static_cast<double>(vramBytes) / (1024.0 * 1024.0));
    }
    
    explicit MetalTextureCube(const ProceduralSkyParams& params, uint32_t resolution) {
      m_Resolution = resolution;
      
      MTL::Device* device = Application::Get().GetRenderContext()->GetDevice();
      KV_CORE_ASSERT(device != nullptr, "[KanVerse TextureCube] Active Metal device is null!");
      
      // Use RGBA16Float to store high-dynamic-range sun radiance without 8-bit clipping
      MTL::TextureDescriptor* desc = MTL::TextureDescriptor::textureCubeDescriptor(
                                                                                   MTL::PixelFormatRGBA16Float,
                                                                                   m_Resolution,
                                                                                   false
                                                                                   );
      desc->setUsage(MTL::TextureUsageShaderRead);
      desc->setStorageMode(MTL::StorageModeShared);
      
      m_Texture = device->newTexture(desc);
      desc->release();
      KV_CORE_ASSERT(m_Texture != nullptr, "[KanVerse TextureCube] Failed to allocate procedural MTL::Texture for cubemap!");
      
      // Normal direction vectors corresponding to the 6 cube faces (+Y and -Y aligned)
      const simd::float3 faceDirections[6] = {
        simd_make_float3( 1.0f,  0.0f,  0.0f), // 0: +X
        simd_make_float3(-1.0f,  0.0f,  0.0f), // 1: -X
        simd_make_float3( 0.0f, -1.0f,  0.0f), // 2: -Y (Zenith in Metal view space)
        simd_make_float3( 0.0f,  1.0f,  0.0f), // 3: +Y (Ground in Metal view space)
        simd_make_float3( 0.0f,  0.0f,  1.0f), // 4: +Z
        simd_make_float3( 0.0f,  0.0f, -1.0f)  // 5: -Z
      };
      
      const simd::float3 faceU[6] = {
        simd_make_float3( 0.0f,  0.0f, -1.0f),
        simd_make_float3( 0.0f,  0.0f,  1.0f),
        simd_make_float3( 1.0f,  0.0f,  0.0f),
        simd_make_float3( 1.0f,  0.0f,  0.0f),
        simd_make_float3( 1.0f,  0.0f,  0.0f),
        simd_make_float3(-1.0f,  0.0f,  0.0f)
      };
      
      const simd::float3 faceV[6] = {
        simd_make_float3( 0.0f,  1.0f,  0.0f),
        simd_make_float3( 0.0f,  1.0f,  0.0f),
        simd_make_float3( 0.0f,  0.0f,  1.0f),
        simd_make_float3( 0.0f,  0.0f, -1.0f),
        simd_make_float3( 0.0f,  1.0f,  0.0f),
        simd_make_float3( 0.0f,  1.0f,  0.0f)
      };
      
      // Vector pointing toward the sun position in the sky
      const simd::float3 sunPosition = simd_normalize(-params.SunDirection);
      
      std::vector<uint16_t> facePixelsHalf(m_Resolution * m_Resolution * 4);
      
      for (size_t face = 0; face < 6; ++face) {
        for (uint32_t y = 0; y < m_Resolution; ++y) {
          float py = (static_cast<float>(y) + 0.5f) / static_cast<float>(m_Resolution);
          py = py * 2.0f - 1.0f;
          
          for (uint32_t x = 0; x < m_Resolution; ++x) {
            float px = (static_cast<float>(x) + 0.5f) / static_cast<float>(m_Resolution);
            px = px * 2.0f - 1.0f;
            
            simd::float3 dir = simd_normalize(faceDirections[face] + (faceU[face] * px) + (faceV[face] * py));
            
            // Atmospheric gradient: above horizon uses zenith blend; below uses ground blend
            float t = dir.y;
            simd::float3 skyColor;
            if (t >= 0.0f) {
              float factor = std::pow(t, 0.45f);
              skyColor = params.HorizonColor * (1.0f - factor) + params.ZenithColor * factor;
            } else {
              float factor = std::pow(std::clamp(-t, 0.0f, 1.0f), 0.5f);
              skyColor = params.HorizonColor * (1.0f - factor) + params.GroundColor * factor;
            }
            
            // Calculate directional angular alignment with the sun
            float sunDot = simd_dot(dir, sunPosition);
            if (sunDot > 0.0f && sunPosition.y > -0.05f) {
              // Soft corona halo glow around the sun
              float glowFactor = std::clamp((sunDot - (1.0f - params.SunGlowSize)) / params.SunGlowSize, 0.0f, 1.0f);
              glowFactor = std::pow(glowFactor, 3.5f);
              skyColor += params.SunColor * (glowFactor * params.SunIntensity * 0.25f);
              
              // Sharp, high-intensity core sun disc
              float discFactor = std::clamp((sunDot - (1.0f - params.SunDiscSize)) / (params.SunDiscSize * 0.15f), 0.0f, 1.0f);
              skyColor += params.SunColor * (discFactor * params.SunIntensity);
            }
            
            size_t idx = (y * m_Resolution + x) * 4;
            // Store as 16-bit half floats (__fp16) for HDR buffers
            __fp16 r = static_cast<__fp16>(skyColor.x);
            __fp16 g = static_cast<__fp16>(skyColor.y);
            __fp16 b = static_cast<__fp16>(skyColor.z);
            __fp16 a = static_cast<__fp16>(1.0f);
            
            std::memcpy(&facePixelsHalf[idx + 0], &r, sizeof(__fp16));
            std::memcpy(&facePixelsHalf[idx + 1], &g, sizeof(__fp16));
            std::memcpy(&facePixelsHalf[idx + 2], &b, sizeof(__fp16));
            std::memcpy(&facePixelsHalf[idx + 3], &a, sizeof(__fp16));
          }
        }
        
        const MTL::Region region = MTL::Region(0, 0, m_Resolution, m_Resolution);
        const NSUInteger bytesPerRow = m_Resolution * 4 * sizeof(uint16_t);
        m_Texture->replaceRegion(region, 0, static_cast<NSUInteger>(face), facePixelsHalf.data(), bytesPerRow, bytesPerRow * m_Resolution);
      }
      
      const size_t vramBytes = m_Resolution * m_Resolution * 8 * 6;
      m_MemoryGuard.Reset(vramBytes);
      
      MTL::SamplerDescriptor* samplerDesc = MTL::SamplerDescriptor::alloc()->init();
      samplerDesc->setMinFilter(MTL::SamplerMinMagFilterLinear);
      samplerDesc->setMagFilter(MTL::SamplerMinMagFilterLinear);
      samplerDesc->setMipFilter(MTL::SamplerMipFilterNotMipmapped);
      samplerDesc->setSAddressMode(MTL::SamplerAddressModeClampToEdge);
      samplerDesc->setTAddressMode(MTL::SamplerAddressModeClampToEdge);
      m_Sampler = device->newSamplerState(samplerDesc);
      samplerDesc->release();
      
      KV_CORE_INFO("TextureCube: Successfully generated HDR procedural sky with sun disc (Resolution: {}x{}, {:.2f} MB)",
                   m_Resolution, m_Resolution, static_cast<double>(vramBytes) / (1024.0 * 1024.0));
    }
    
    explicit MetalTextureCube(const std::string& filepath, uint32_t resolution) {
      m_Resolution = resolution;
      
      MTL::Device* device = Application::Get().GetRenderContext()->GetDevice();
      KV_CORE_ASSERT(device != nullptr, "[KanVerse TextureCube] Active Metal device is null!");
      
      int width, height, channels;
      float* data = stbi_loadf(filepath.c_str(), &width, &height, &channels, 4);
      if (!data) {
        KV_CORE_ERROR("TextureCube: Failed to load HDR image '{0}'. Reason: {1}", filepath, stbi_failure_reason());
        KV_CORE_ASSERT(false, "Failed to load Equirectangular HDR map!");
        return;
      }
      
      MTL::TextureDescriptor* hdrDesc = MTL::TextureDescriptor::texture2DDescriptor(
                                                                                    MTL::PixelFormatRGBA32Float, width, height, false);
      hdrDesc->setUsage(MTL::TextureUsageShaderRead);
      hdrDesc->setStorageMode(MTL::StorageModeShared);
      
      MTL::Texture* hdrTexture = device->newTexture(hdrDesc);
      hdrDesc->release();
      
      const MTL::Region hdrRegion = MTL::Region(0, 0, width, height);
      hdrTexture->replaceRegion(hdrRegion, 0, data, width * 4 * sizeof(float));
      stbi_image_free(data);
      
      MTL::TextureDescriptor* cubeDesc = MTL::TextureDescriptor::textureCubeDescriptor(
                                                                                       MTL::PixelFormatRGBA16Float, m_Resolution, true);
      cubeDesc->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite);
      cubeDesc->setStorageMode(MTL::StorageModePrivate);
      
      m_Texture = device->newTexture(cubeDesc);
      cubeDesc->release();
      
      MTL::Library* defaultLib = device->newDefaultLibrary();
      KV_CORE_ASSERT(defaultLib, "[KanVerse TextureCube] Failed to load default Metal library!");
      
      auto funcName = NS::String::string("equirectangular_to_cubemap", NS::UTF8StringEncoding);
      MTL::Function* computeFunc = defaultLib->newFunction(funcName);
      funcName->release();
      defaultLib->release();
      KV_CORE_ASSERT(computeFunc, "[KanVerse TextureCube] Compute kernel 'equirectangular_to_cubemap' not found!");
      
      NS::Error* error = nullptr;
      MTL::ComputePipelineState* computePipeline = device->newComputePipelineState(computeFunc, &error);
      computeFunc->release();
      KV_CORE_ASSERT(computePipeline, "[KanVerse TextureCube] Failed to create compute pipeline!");
      
      MTL::CommandQueue* queue = Application::Get().GetRenderContext()->GetCommandQueue();
      MTL::CommandBuffer* cmdBuffer = queue->commandBuffer();
      MTL::ComputeCommandEncoder* encoder = cmdBuffer->computeCommandEncoder();
      
      encoder->setComputePipelineState(computePipeline);
      
      MTL::Texture* cubemapArrayView = m_Texture->newTextureView(
                                                                 MTL::PixelFormatRGBA16Float,
                                                                 MTL::TextureType2DArray,
                                                                 NS::Range::Make(0, 1),
                                                                 NS::Range::Make(0, 6)
                                                                 );
      
      encoder->setTexture(hdrTexture, 0);
      encoder->setTexture(cubemapArrayView, 1);
      
      const MTL::Size threadgroupSize(16, 16, 1);
      const MTL::Size threadgroups(
                                   (m_Resolution + threadgroupSize.width - 1) / threadgroupSize.width,
                                   (m_Resolution + threadgroupSize.height - 1) / threadgroupSize.height,
                                   6
                                   );
      
      encoder->dispatchThreadgroups(threadgroups, threadgroupSize);
      encoder->endEncoding();
      
      MTL::BlitCommandEncoder* blitEncoder = cmdBuffer->blitCommandEncoder();
      blitEncoder->generateMipmaps(m_Texture);
      blitEncoder->endEncoding();
      
      cmdBuffer->commit();
      cmdBuffer->waitUntilCompleted();
      
      cubemapArrayView->release();
      hdrTexture->release();
      computePipeline->release();
      
      MTL::SamplerDescriptor* samplerDesc = MTL::SamplerDescriptor::alloc()->init();
      samplerDesc->setMinFilter(MTL::SamplerMinMagFilterLinear);
      samplerDesc->setMagFilter(MTL::SamplerMinMagFilterLinear);
      samplerDesc->setMipFilter(MTL::SamplerMipFilterLinear);
      samplerDesc->setSAddressMode(MTL::SamplerAddressModeClampToEdge);
      samplerDesc->setTAddressMode(MTL::SamplerAddressModeClampToEdge);
      m_Sampler = device->newSamplerState(samplerDesc);
      samplerDesc->release();
      
      const size_t vramBytes = m_Resolution * m_Resolution * 8 * 6;
      m_MemoryGuard.Reset(vramBytes);
      
      KV_CORE_INFO("TextureCube: Successfully generated HDR Cubemap from '{0}' (Resolution: {1}x{1}, {2:.2f} MB)",
                   filepath, m_Resolution, static_cast<double>(vramBytes) / (1024.0 * 1024.0));
    }
    
    explicit MetalTextureCube(const std::shared_ptr<TextureCube>& sourceEnvMap, uint32_t resolution) {
      m_Resolution = resolution;
      
      MTL::Device* device = Application::Get().GetRenderContext()->GetDevice();
      
      MTL::TextureDescriptor* cubeDesc = MTL::TextureDescriptor::textureCubeDescriptor(
                                                                                       MTL::PixelFormatRGBA16Float, m_Resolution, false);
      cubeDesc->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite);
      cubeDesc->setStorageMode(MTL::StorageModePrivate);
      
      m_Texture = device->newTexture(cubeDesc);
      cubeDesc->release();
      
      MTL::Library* defaultLib = device->newDefaultLibrary();
      auto funcName = NS::String::string("irradiance_convolution", NS::UTF8StringEncoding);
      MTL::Function* computeFunc = defaultLib->newFunction(funcName);
      funcName->release();
      defaultLib->release();
      
      NS::Error* error = nullptr;
      MTL::ComputePipelineState* computePipeline = device->newComputePipelineState(computeFunc, &error);
      computeFunc->release();
      
      MTL::CommandQueue* queue = Application::Get().GetRenderContext()->GetCommandQueue();
      MTL::CommandBuffer* cmdBuffer = queue->commandBuffer();
      MTL::ComputeCommandEncoder* encoder = cmdBuffer->computeCommandEncoder();
      
      MTL::Texture* cubemapArrayView = m_Texture->newTextureView(
                                                                 MTL::PixelFormatRGBA16Float,
                                                                 MTL::TextureType2DArray,
                                                                 NS::Range::Make(0, 1),
                                                                 NS::Range::Make(0, 6)
                                                                 );
      
      encoder->setComputePipelineState(computePipeline);
      encoder->setTexture(sourceEnvMap->GetNativeTexture(), 0);
      encoder->setTexture(cubemapArrayView, 1);
      
      const MTL::Size threadgroupSize(16, 16, 1);
      const MTL::Size threadgroups(
                                   (m_Resolution + threadgroupSize.width - 1) / threadgroupSize.width,
                                   (m_Resolution + threadgroupSize.height - 1) / threadgroupSize.height,
                                   6
                                   );
      
      encoder->dispatchThreadgroups(threadgroups, threadgroupSize);
      encoder->endEncoding();
      
      cmdBuffer->commit();
      cmdBuffer->waitUntilCompleted();
      
      cubemapArrayView->release();
      computePipeline->release();
      
      MTL::SamplerDescriptor* samplerDesc = MTL::SamplerDescriptor::alloc()->init();
      samplerDesc->setMinFilter(MTL::SamplerMinMagFilterLinear);
      samplerDesc->setMagFilter(MTL::SamplerMinMagFilterLinear);
      samplerDesc->setMipFilter(MTL::SamplerMipFilterNotMipmapped);
      samplerDesc->setSAddressMode(MTL::SamplerAddressModeClampToEdge);
      samplerDesc->setTAddressMode(MTL::SamplerAddressModeClampToEdge);
      m_Sampler = device->newSamplerState(samplerDesc);
      samplerDesc->release();
      
      m_MemoryGuard.Reset(m_Resolution * m_Resolution * 8 * 6);
      KV_CORE_INFO("TextureCube: Generated Irradiance Map (Resolution: {0}x{0})", m_Resolution);
    }
    
    ~MetalTextureCube() override {
      if (m_Sampler) { m_Sampler->release(); m_Sampler = nullptr; }
      if (m_Texture) { m_Texture->release(); m_Texture = nullptr; }
      m_MemoryGuard.Reset(0);
    }
    
    void Bind(MTL::RenderCommandEncoder* encoder, uint32_t textureSlot, uint32_t samplerSlot) const override {
      KV_CORE_ASSERT(encoder != nullptr, "[KanVerse TextureCube] Command encoder cannot be null during bind!");
      encoder->setFragmentTexture(m_Texture, textureSlot);
      encoder->setFragmentSamplerState(m_Sampler, samplerSlot);
    }
    
    [[nodiscard]] uint32_t GetResolution() const noexcept override { return m_Resolution; }
    [[nodiscard]] MTL::Texture* GetNativeTexture() const noexcept override { return m_Texture; }
    
  private:
    uint32_t               m_Resolution{0};
    MTL::Texture*          m_Texture{nullptr};
    MTL::SamplerState*     m_Sampler{nullptr};
    ScopedMemoryAllocation m_MemoryGuard{};
  };
  
  std::shared_ptr<TextureCube> TextureCube::Create(const std::vector<std::string>& faces) {
    return std::make_shared<MetalTextureCube>(faces);
  }
  
  std::shared_ptr<TextureCube> TextureCube::CreateProcedural(uint32_t resolution) {
    return std::make_shared<MetalTextureCube>(ProceduralSkyParams{}, resolution);
  }
  
  std::shared_ptr<TextureCube> TextureCube::CreateProcedural(const ProceduralSkyParams& params, uint32_t resolution) {
    return std::make_shared<MetalTextureCube>(params, resolution);
  }
  
  std::shared_ptr<TextureCube> TextureCube::CreateFromEquirectangular(const std::string& filepath, uint32_t resolution) {
    return std::make_shared<MetalTextureCube>(filepath, resolution);
  }
  
  std::shared_ptr<TextureCube> TextureCube::CreateIrradianceMap(const std::shared_ptr<TextureCube>& sourceEnvMap, uint32_t resolution) {
    return std::make_shared<MetalTextureCube>(sourceEnvMap, resolution);
  }
  
} // namespace KanVerse
