/*!
 * @file Framebuffer.hpp
 * @brief Offscreen render target abstraction managing color and depth textures.
 * @details Encapsulates GPU color attachments, depth-stencil textures, and preconfigured
 *          Metal render pass descriptors conforming to HDR floating-point targets.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Core/Memory/MemoryTracking.hpp"
#include <cstdint>
#include <memory>

namespace MTL {
  class Texture;
  class RenderPassDescriptor;
}

namespace KanVerse {
  
  /*!
   * @enum FramebufferPixelFormat
   * @brief Supported color attachment pixel formats.
   */
  enum class FramebufferPixelFormat : uint8_t {
    RGBA8 = 0,    ///< Standard 8-bit SDR format.
    RGBA16Float   ///< Half-precision 16-bit float format for HDR radiance.
  };
  
  /*!
   * @struct FramebufferSpecification
   * @brief Configuration properties used to allocate offscreen GPU framebuffer targets.
   */
  struct FramebufferSpecification {
    uint32_t               Width{1280};                                    ///< Viewport pixel width.
    uint32_t               Height{720};                                   ///< Viewport pixel height.
    uint32_t               Samples{1};                                    ///< Multi-sample anti-aliasing count.
    FramebufferPixelFormat Format{FramebufferPixelFormat::RGBA16Float};   ///< Defaults to 16-bit half float HDR target.
    bool                   SwapChainTarget{false};                        ///< Direct swapchain rendering flag.
    bool                   DepthOnly{false};                              ///< True if allocating a shadow depth map.
    float                  ClearColor[4]{0.06f, 0.07f, 0.09f, 1.0f};      ///< Default background clear color (RGBA).
  };
  
  /*!
   * @class Framebuffer
   * @brief High-level hardware render target container managing color and depth-stencil textures.
   */
  class Framebuffer {
  public:
    virtual ~Framebuffer() = default;
    
    /*!
     * @brief Resizes backing GPU textures and reallocates attachment allocations.
     * @param width New pixel width.
     * @param height New pixel height.
     */
    virtual void Resize(uint32_t width, uint32_t height) = 0;
    
    /*!
     * @brief Queries configuration specification.
     * @return Const reference to specification.
     */
    [[nodiscard]] virtual const FramebufferSpecification& GetSpecification() const noexcept = 0;
    
    /*!
     * @brief Accesses the color attachment texture.
     * @param index Attachment slot index (defaults to primary target 0).
     * @return Raw pointer to MTL::Texture (nullptr if DepthOnly is true).
     */
    [[nodiscard]] virtual MTL::Texture* GetColorAttachment(uint32_t index = 0) const noexcept = 0;
    
    /*!
     * @brief Accesses the depth-stencil attachment texture.
     * @return Raw pointer to MTL::Texture.
     */
    [[nodiscard]] virtual MTL::Texture* GetDepthAttachment() const noexcept = 0;
    
    /*!
     * @brief Accesses the preconfigured Metal render pass descriptor.
     * @return Raw pointer to MTL::RenderPassDescriptor.
     */
    [[nodiscard]] virtual MTL::RenderPassDescriptor* GetRenderPassDescriptor() const noexcept = 0;
    
    /*!
     * @brief Factory creating an offscreen Framebuffer instance.
     * @param spec Specification configuration.
     * @return Shared pointer owning the Framebuffer.
     */
    [[nodiscard]] static std::shared_ptr<Framebuffer> Create(const FramebufferSpecification& spec);
  };
  
} // namespace KanVerse
