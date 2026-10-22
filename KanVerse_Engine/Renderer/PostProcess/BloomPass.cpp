/*!
 * @file BloomPass.mm
 * @brief Implementation of the multi-mip Dual-Kawase compute bloom pass.
 * @details Allocates RGBA16Float mip targets and coordinates compute encoders
 *          for highlight extraction, progressive downsampling, and tent upsampling.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/PostProcess/BloomPass.hpp"
#include "Core/Application/Application.hpp"
#include "Core/Diagnostics/Log.hpp"
#include "Core/Diagnostics/Assert.hpp"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <algorithm>

namespace KanVerse {
  
  BloomPass::BloomPass() = default;
  
  BloomPass::~BloomPass() {
    ReleaseTextures();
    if (m_LinearSampler) {
      m_LinearSampler->release();
      m_LinearSampler = nullptr;
    }
    if (m_DownsamplePipeline) {
      m_DownsamplePipeline->release();
      m_DownsamplePipeline = nullptr;
    }
    if (m_UpsamplePipeline) {
      m_UpsamplePipeline->release();
      m_UpsamplePipeline = nullptr;
    }
  }
  
  void BloomPass::ReleaseTextures() {
    for (auto* tex : m_DownsampleMips) {
      if (tex) tex->release();
    }
    m_DownsampleMips.clear();
    
    for (auto* tex : m_UpsampleMips) {
      if (tex) tex->release();
    }
    m_UpsampleMips.clear();
  }
  
  void BloomPass::Init() {
    MTL::Device* device = Application::Get().GetRenderContext()->GetDevice();
    KV_CORE_ASSERT(device != nullptr, "[KanVerse Bloom] Metal device is null!");
    
    // 1. Bilinear clamp sampler for sub-pixel texture filtering
    MTL::SamplerDescriptor* samplerDesc = MTL::SamplerDescriptor::alloc()->init();
    samplerDesc->setMinFilter(MTL::SamplerMinMagFilterLinear);
    samplerDesc->setMagFilter(MTL::SamplerMinMagFilterLinear);
    samplerDesc->setMipFilter(MTL::SamplerMipFilterNotMipmapped);
    samplerDesc->setSAddressMode(MTL::SamplerAddressModeClampToEdge);
    samplerDesc->setTAddressMode(MTL::SamplerAddressModeClampToEdge);
    m_LinearSampler = device->newSamplerState(samplerDesc);
    samplerDesc->release();
    
    // 2. Compile compute kernels from application library bundle
    MTL::Library* defaultLib = device->newDefaultLibrary();
    KV_CORE_ASSERT(defaultLib != nullptr, "[KanVerse Bloom] Failed to load default library!");
    
    auto downName = NS::String::string("bloom_downsample", NS::UTF8StringEncoding);
    auto upName   = NS::String::string("bloom_upsample",   NS::UTF8StringEncoding);
    
    MTL::Function* downFunc = defaultLib->newFunction(downName);
    MTL::Function* upFunc   = defaultLib->newFunction(upName);
    downName->release();
    upName->release();
    defaultLib->release();
    
    KV_CORE_ASSERT(downFunc != nullptr, "[KanVerse Bloom] 'bloom_downsample' kernel not found!");
    KV_CORE_ASSERT(upFunc != nullptr,   "[KanVerse Bloom] 'bloom_upsample' kernel not found!");
    
    NS::Error* error = nullptr;
    m_DownsamplePipeline = device->newComputePipelineState(downFunc, &error);
    downFunc->release();
    KV_CORE_ASSERT(m_DownsamplePipeline != nullptr, "[KanVerse Bloom] Failed to create downsample pipeline!");
    
    m_UpsamplePipeline = device->newComputePipelineState(upFunc, &error);
    upFunc->release();
    KV_CORE_ASSERT(m_UpsamplePipeline != nullptr, "[KanVerse Bloom] Failed to create upsample pipeline!");
    
    KV_CORE_INFO("BloomPass compute pipelines compiled successfully.");
  }
  
  void BloomPass::Resize(uint32_t baseWidth, uint32_t baseHeight) {
    if (m_BaseWidth == baseWidth && m_BaseHeight == baseHeight && !m_DownsampleMips.empty()) {
      return;
    }
    
    m_BaseWidth = baseWidth;
    m_BaseHeight = baseHeight;
    ReleaseTextures();
    
    MTL::Device* device = Application::Get().GetRenderContext()->GetDevice();
    
    uint32_t mipW = std::max(1u, baseWidth / 2);
    uint32_t mipH = std::max(1u, baseHeight / 2);
    
    m_DownsampleMips.resize(kMipLevels, nullptr);
    m_UpsampleMips.resize(kMipLevels, nullptr);
    
    // Allocate RGBA16Float texture pyramid with compute read/write access
    for (uint32_t i = 0; i < kMipLevels; ++i) {
      auto desc = MTL::TextureDescriptor::texture2DDescriptor(
                                                              MTL::PixelFormatRGBA16Float,
                                                              mipW,
                                                              mipH,
                                                              false
                                                              );
      desc->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite);
      desc->setStorageMode(MTL::StorageModePrivate);
      
      m_DownsampleMips[i] = device->newTexture(desc);
      m_UpsampleMips[i]   = device->newTexture(desc);
      desc->release();
      
      mipW = std::max(1u, mipW / 2);
      mipH = std::max(1u, mipH / 2);
    }
  }
  
  void BloomPass::Execute(MTL::CommandBuffer* cmdBuffer, MTL::Texture* hdrSource) {
    if (!hdrSource || m_DownsampleMips.empty() || !m_DownsamplePipeline || !m_UpsamplePipeline) {
      return;
    }
    
    MTL::ComputeCommandEncoder* encoder = cmdBuffer->computeCommandEncoder();
    if (!encoder) return;
    
    // =========================================================================
    // Phase 1: Downsampling Pyramid with Threshold Prefiltering
    // =========================================================================
    encoder->setComputePipelineState(m_DownsamplePipeline);
    encoder->setSamplerState(m_LinearSampler, 0);
    
    MTL::Texture* currentSrc = hdrSource;
    for (uint32_t i = 0; i < kMipLevels; ++i) {
      MTL::Texture* dst = m_DownsampleMips[i];
      
      // Normalized UV texel size must be 1.0 / dst->size so (gid + 0.5) * texelSize spans [0.0, 1.0] exactly
      DownsampleUniforms uniforms;
      uniforms.TexelSize = Math::MakeVec2(1.0f / static_cast<float>(dst->width()),
                                          1.0f / static_cast<float>(dst->height()));
      uniforms.Threshold = m_Threshold;
      uniforms.SoftKnee  = m_SoftKnee;
      uniforms.MipLevel  = static_cast<int32_t>(i);
      
      encoder->setTexture(currentSrc, 0);
      encoder->setTexture(dst, 1);
      encoder->setBytes(&uniforms, sizeof(DownsampleUniforms), 0);
      
      const MTL::Size threadgroupSize(16, 16, 1);
      const MTL::Size threadgroups(
                                   (dst->width()  + threadgroupSize.width  - 1) / threadgroupSize.width,
                                   (dst->height() + threadgroupSize.height - 1) / threadgroupSize.height,
                                   1
                                   );
      
      encoder->dispatchThreadgroups(threadgroups, threadgroupSize);
      currentSrc = dst; // Next mip downsamples from current destination
    }
    
    // =========================================================================
    // Phase 2: Upsampling & Progressive Additive Accumulation
    // =========================================================================
    encoder->setComputePipelineState(m_UpsamplePipeline);
    encoder->setSamplerState(m_LinearSampler, 0);
    
    // Progressive upsampling starting from the second-lowest mip (kMipLevels - 2) up to mip 0
    for (int32_t i = static_cast<int32_t>(kMipLevels) - 2; i >= 0; --i) {
      MTL::Texture* lowerMip  = (i == static_cast<int32_t>(kMipLevels) - 2) ? m_DownsampleMips[i + 1] : m_UpsampleMips[i + 1];
      MTL::Texture* higherMip = m_DownsampleMips[i];
      MTL::Texture* dst       = m_UpsampleMips[i];
      
      // Normalized UV texel size must be 1.0 / dst->size so (gid + 0.5) * texelSize spans [0.0, 1.0] across dst
      UpsampleUniforms uniforms;
      uniforms.TexelSize    = Math::MakeVec2(1.0f / static_cast<float>(dst->width()),
                                             1.0f / static_cast<float>(dst->height()));
      uniforms.FilterRadius = 1.0f;
      
      encoder->setTexture(lowerMip, 0);
      encoder->setTexture(higherMip, 1);
      encoder->setTexture(dst, 2);
      encoder->setBytes(&uniforms, sizeof(UpsampleUniforms), 0);
      
      const MTL::Size threadgroupSize(16, 16, 1);
      const MTL::Size threadgroups(
                                   (dst->width()  + threadgroupSize.width  - 1) / threadgroupSize.width,
                                   (dst->height() + threadgroupSize.height - 1) / threadgroupSize.height,
                                   1
                                   );
      
      encoder->dispatchThreadgroups(threadgroups, threadgroupSize);
    }
    
    encoder->endEncoding();
  }
  
  MTL::Texture* BloomPass::GetBloomTexture() const noexcept {
    return m_UpsampleMips.empty() ? nullptr : m_UpsampleMips[0];
  }
  
} // namespace KanVerse
