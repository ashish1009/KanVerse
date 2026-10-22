/*!
 * @file Texture2D.mm
 * @brief Implementation of Metal 2D texture allocation, image loading, and command binding.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/Textures/Texture2D.hpp"
#include "Core/Diagnostics/Log.hpp"
#include "Core/Diagnostics/Assert.hpp"
#include "Core/Application/Application.hpp"
#include "Core/Memory/MemoryTracking.hpp"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#import <Metal/Metal.h>
#import <CoreGraphics/CoreGraphics.h>
#import <ImageIO/ImageIO.h>
#import <Foundation/Foundation.h>
#include <vector>

namespace KanVerse {
  
  /**
   * @brief Metal API specific implementation of Texture2D.
   */
  class MetalTexture2D : public Texture2D {
  public:
    explicit MetalTexture2D(const TextureSpecification& spec)
    : m_Specification(spec), m_Width(spec.Width), m_Height(spec.Height) {
      KV_CORE_ASSERT(m_Width > 0 && m_Height > 0, "[KanVerse Texture] Texture dimensions must be positive non-zero!");
      
      MTL::Device* device = Application::Get().GetRenderContext()->GetDevice();
      
      // Determine correct pixel format based on sRGB flag
      MTL::PixelFormat pixelFormat = m_Specification.sRGB ?
      MTL::PixelFormatRGBA8Unorm_sRGB :
      MTL::PixelFormatRGBA8Unorm;
      
      MTL::TextureDescriptor* desc = MTL::TextureDescriptor::texture2DDescriptor(
                                                                                 pixelFormat,
                                                                                 m_Width,
                                                                                 m_Height,
                                                                                 m_Specification.GenerateMipmaps
                                                                                 );
      desc->setUsage(MTL::TextureUsageShaderRead);
      desc->setStorageMode(MTL::StorageModeShared);
      
      m_Texture = device->newTexture(desc);
      KV_CORE_ASSERT(m_Texture != nullptr, "[KanVerse Texture] Failed to allocate MTL::Texture!");
      
      const size_t vramBytes = m_Width * m_Height * 4;
      m_MemoryGuard.Reset(vramBytes);
      
      MTL::SamplerDescriptor* samplerDesc = MTL::SamplerDescriptor::alloc()->init();
      samplerDesc->setMinFilter(MTL::SamplerMinMagFilterLinear);
      samplerDesc->setMagFilter(MTL::SamplerMinMagFilterLinear);
      samplerDesc->setMipFilter(MTL::SamplerMipFilterLinear);
      samplerDesc->setSAddressMode(MTL::SamplerAddressModeRepeat);
      samplerDesc->setTAddressMode(MTL::SamplerAddressModeRepeat);
      
      m_Sampler = device->newSamplerState(samplerDesc);
      samplerDesc->release();
    }
    
    ~MetalTexture2D() override {
      if (m_Sampler) {
        m_Sampler->release();
        m_Sampler = nullptr;
      }
      if (m_Texture) {
        m_Texture->release();
        m_Texture = nullptr;
      }
      m_MemoryGuard.Reset(0);
    }
    
    void SetData(const void* data, size_t size) override {
      const size_t expectedBytes = m_Width * m_Height * 4;
      KV_CORE_ASSERT(size == expectedBytes, "[KanVerse Texture] Upload buffer byte size mismatch!");
      
      const MTL::Region region = MTL::Region(0, 0, 0, m_Width, m_Height, 1);
      const NSUInteger bytesPerRow = m_Width * 4;
      m_Texture->replaceRegion(region, 0, data, bytesPerRow);
    }
    
    void Bind(MTL::RenderCommandEncoder* encoder, uint32_t textureSlot, uint32_t samplerSlot) const override {
      KV_CORE_ASSERT(encoder != nullptr, "[KanVerse Texture] Command encoder cannot be null during bind!");
      encoder->setFragmentTexture(m_Texture, textureSlot);
      encoder->setFragmentSamplerState(m_Sampler, samplerSlot);
    }
    
    [[nodiscard]] uint32_t GetWidth() const noexcept override { return m_Width; }
    [[nodiscard]] uint32_t GetHeight() const noexcept override { return m_Height; }
    [[nodiscard]] MTL::Texture* GetNativeTexture() const noexcept override { return m_Texture; }
    
  private:
    TextureSpecification   m_Specification;
    uint32_t               m_Width{0};
    uint32_t               m_Height{0};
    MTL::Texture*          m_Texture{nullptr};
    MTL::SamplerState*     m_Sampler{nullptr};
    ScopedMemoryAllocation m_MemoryGuard{};
  };
  
  std::shared_ptr<Texture2D> Texture2D::Create(const TextureSpecification& spec) {
    return std::make_shared<MetalTexture2D>(spec);
  }
  
  std::shared_ptr<Texture2D> Texture2D::Create(const std::string& filepath, bool sRGB) {
    std::vector<uint8_t> rawPixels;
    uint32_t width = 0;
    uint32_t height = 0;
    
    @autoreleasepool {
      NSString* pathString = [NSString stringWithUTF8String:filepath.c_str()];
      NSURL* fileURL = [NSURL fileURLWithPath:pathString];
      
      CGImageSourceRef imageSource = CGImageSourceCreateWithURL((__bridge CFURLRef)fileURL, NULL);
      if (!imageSource) {
        KV_CORE_ERROR("Texture2D: Failed to open image file at path: '{}'", filepath);
        return nullptr;
      }
      
      CGImageRef image = CGImageSourceCreateImageAtIndex(imageSource, 0, NULL);
      CFRelease(imageSource);
      if (!image) {
        KV_CORE_ERROR("Texture2D: Failed to decode image data from file: '{}'", filepath);
        return nullptr;
      }
      
      width  = static_cast<uint32_t>(CGImageGetWidth(image));
      height = static_cast<uint32_t>(CGImageGetHeight(image));
      rawPixels.resize(width * height * 4);
      
      CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
      CGContextRef context = CGBitmapContextCreate(
                                                   rawPixels.data(),
                                                   width,
                                                   height,
                                                   8,
                                                   width * 4,
                                                   colorSpace,
                                                   static_cast<uint32_t>(kCGImageAlphaPremultipliedLast) | static_cast<uint32_t>(kCGBitmapByteOrder32Big)
                                                   );
      CGColorSpaceRelease(colorSpace);
      
      if (!context) {
        CGImageRelease(image);
        KV_CORE_ERROR("Texture2D: Failed to create CGBitmapContext for '{}'", filepath);
        return nullptr;
      }
      
      CGRect rect = CGRectMake(0, 0, width, height);
      CGContextDrawImage(context, rect, image);
      CGContextRelease(context);
      CGImageRelease(image);
    }
    
    if (rawPixels.empty() || width == 0 || height == 0) {
      return nullptr;
    }
    
    // Pass the format flag down to the specification
    TextureSpecification spec{
      .Width = width,
      .Height = height,
      .GenerateMipmaps = false,
      .sRGB = sRGB
    };
    
    auto texture = std::make_shared<MetalTexture2D>(spec);
    texture->SetData(rawPixels.data(), rawPixels.size());
    
    KV_CORE_INFO("Texture2D: Successfully loaded '{}' ({}x{} RGBA, sRGB: {}, {:.2f} MB)",
                 filepath, width, height, sRGB,
                 static_cast<double>(rawPixels.size()) / (1024.0 * 1024.0));
    
    return texture;
  }
  
  std::shared_ptr<Texture2D> Texture2D::CreateCheckerboard(uint32_t resolution) {
    TextureSpecification spec{
      .Width = resolution,
      .Height = resolution,
      .GenerateMipmaps = false,
      .sRGB = true // Checkerboard acts as a color texture
    };
    
    auto texture = std::make_shared<MetalTexture2D>(spec);
    std::vector<uint32_t> pixels(resolution * resolution);
    constexpr uint32_t TileSize = 16;
    
    for (uint32_t y = 0; y < resolution; ++y) {
      for (uint32_t x = 0; x < resolution; ++x) {
        const bool isWhite = ((x / TileSize) % 2) == ((y / TileSize) % 2);
        pixels[y * resolution + x] = isWhite ? 0xFFFFFFFF : 0xFF222222;
      }
    }
    
    texture->SetData(pixels.data(), pixels.size() * sizeof(uint32_t));
    return texture;
  }
}
