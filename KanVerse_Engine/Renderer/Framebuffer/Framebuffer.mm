/*!
 * @file Framebuffer.mm
 * @brief Apple Metal implementation of offscreen Framebuffer supporting HDR textures.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/Framebuffer/Framebuffer.hpp"
#include "Core/Diagnostics/Assert.hpp"
#include "Core/Application/Application.hpp"
#include "Core/Memory/MemoryTracking.hpp"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

namespace KanVerse {
  
  class MetalFramebuffer : public Framebuffer {
  public:
    explicit MetalFramebuffer(const FramebufferSpecification& spec)
    : m_Spec(spec) {
      Invalidate();
    }
    
    ~MetalFramebuffer() override {
      ReleaseResources();
    }
    
    void Resize(uint32_t width, uint32_t height) override {
      if (width == 0 || height == 0 || (m_Spec.Width == width && m_Spec.Height == height)) {
        return;
      }
      
      m_Spec.Width = width;
      m_Spec.Height = height;
      Invalidate();
    }
    
    [[nodiscard]] const FramebufferSpecification& GetSpecification() const noexcept override {
      return m_Spec;
    }
    
    [[nodiscard]] MTL::Texture* GetColorAttachment(uint32_t index) const noexcept override {
      if (m_Spec.DepthOnly) {
        return nullptr;
      }
      KV_CORE_ASSERT(index == 0, "[KanVerse] Multiple color attachments currently unsupported!");
      return m_ColorTexture;
    }
    
    [[nodiscard]] MTL::Texture* GetDepthAttachment() const noexcept override {
      return m_DepthTexture;
    }
    
    [[nodiscard]] MTL::RenderPassDescriptor* GetRenderPassDescriptor() const noexcept override {
      return m_PassDescriptor;
    }
    
  private:
    void ReleaseResources() {
      if (m_PassDescriptor) {
        m_PassDescriptor->release();
        m_PassDescriptor = nullptr;
      }
      if (m_DepthTexture) {
        m_DepthTexture->release();
        m_DepthTexture = nullptr;
      }
      if (m_ColorTexture) {
        m_ColorTexture->release();
        m_ColorTexture = nullptr;
      }
    }
    
    void Invalidate() {
      ReleaseResources();
      
      MTL::Device* device = Application::Get().GetRenderContext()->GetDevice();
      KV_CORE_ASSERT(device != nullptr, "[KanVerse] Active Metal device is null!");
      KV_CORE_ASSERT(m_Spec.Width > 0 && m_Spec.Height > 0, "[KanVerse] Viewport dimensions must be non-zero!");
      
      size_t totalTextureBytes = 0;
      
      // 1. Allocate Color Attachment (RGBA16Float for HDR or BGRA8 for SDR)
      if (!m_Spec.DepthOnly) {
        const MTL::PixelFormat colorFormat = (m_Spec.Format == FramebufferPixelFormat::RGBA16Float)
        ? MTL::PixelFormatRGBA16Float
        : MTL::PixelFormatBGRA8Unorm_sRGB;
        
        const size_t bytesPerPixel = (m_Spec.Format == FramebufferPixelFormat::RGBA16Float) ? 8 : 4;
        
        auto colorDesc = MTL::TextureDescriptor::texture2DDescriptor(
                                                                     colorFormat,
                                                                     m_Spec.Width,
                                                                     m_Spec.Height,
                                                                     false
                                                                     );
        colorDesc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
        colorDesc->setStorageMode(MTL::StorageModePrivate);
        m_ColorTexture = device->newTexture(colorDesc);
        colorDesc->release();
        KV_CORE_ASSERT(m_ColorTexture != nullptr, "[KanVerse] Failed to allocate Framebuffer Color Texture!");
        
        totalTextureBytes += static_cast<size_t>(m_Spec.Width) * m_Spec.Height * bytesPerPixel;
      }
      
      // 2. Allocate Depth Texture (Depth32Float -> 4 bytes per pixel)
      auto depthDesc = MTL::TextureDescriptor::texture2DDescriptor(
                                                                   MTL::PixelFormatDepth32Float,
                                                                   m_Spec.Width,
                                                                   m_Spec.Height,
                                                                   false
                                                                   );
      depthDesc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
      depthDesc->setStorageMode(MTL::StorageModePrivate);
      m_DepthTexture = device->newTexture(depthDesc);
      depthDesc->release();
      KV_CORE_ASSERT(m_DepthTexture != nullptr, "[KanVerse] Failed to allocate Framebuffer Depth Texture!");
      
      totalTextureBytes += static_cast<size_t>(m_Spec.Width) * m_Spec.Height * 4;
      m_MemoryGuard.Reset(totalTextureBytes);
      
      // 3. Build Reusable MTL::RenderPassDescriptor
      m_PassDescriptor = MTL::RenderPassDescriptor::renderPassDescriptor();
      KV_CORE_ASSERT(m_PassDescriptor != nullptr, "[KanVerse] Failed to create MTL::RenderPassDescriptor!");
      
      if (!m_Spec.DepthOnly) {
        auto colorAttachment = m_PassDescriptor->colorAttachments()->object(0);
        colorAttachment->setTexture(m_ColorTexture);
        colorAttachment->setLoadAction(MTL::LoadActionClear);
        colorAttachment->setStoreAction(MTL::StoreActionStore);
        colorAttachment->setClearColor(MTL::ClearColor::Make(
                                                             m_Spec.ClearColor[0],
                                                             m_Spec.ClearColor[1],
                                                             m_Spec.ClearColor[2],
                                                             m_Spec.ClearColor[3]
                                                             ));
      }
      
      auto depthAttachment = m_PassDescriptor->depthAttachment();
      depthAttachment->setTexture(m_DepthTexture);
      depthAttachment->setLoadAction(MTL::LoadActionClear);
      depthAttachment->setStoreAction(MTL::StoreActionStore);
      depthAttachment->setClearDepth(1.0);
    }
    
    FramebufferSpecification   m_Spec;
    MTL::Texture*              m_ColorTexture{nullptr};
    MTL::Texture*              m_DepthTexture{nullptr};
    MTL::RenderPassDescriptor* m_PassDescriptor{nullptr};
    ScopedMemoryAllocation     m_MemoryGuard{};
  };
  
  std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec) {
    return std::make_shared<MetalFramebuffer>(spec);
  }
  
} // namespace KanVerse
