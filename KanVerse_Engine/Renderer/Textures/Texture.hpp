/*!
 * @file Texture.hpp
 * @brief Base abstract contract representing hardware-accelerated GPU textures.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace MTL {
  class Texture;
  class SamplerState;
  class RenderCommandEncoder;
}

namespace KanVerse {
  
  /*!
   * @class Texture
   * @brief Abstract base class for GPU texture resources.
   */
  class Texture {
  public:
    virtual ~Texture() = default;
    
    /*!
     * @brief Queries texture width in texels.
     */
    [[nodiscard]] virtual uint32_t GetWidth() const noexcept = 0;
    
    /*!
     * @brief Queries texture height in texels.
     */
    [[nodiscard]] virtual uint32_t GetHeight() const noexcept = 0;
    
    /*!
     * @brief Binds texture and sampler states to a render encoder at specified binding slots.
     * @param encoder Active Metal command encoder.
     * @param textureSlot Fragment shader texture slot index.
     * @param samplerSlot Fragment shader sampler slot index.
     */
    virtual void Bind(MTL::RenderCommandEncoder* encoder, uint32_t textureSlot = 0, uint32_t samplerSlot = 0) const = 0;
    
    /*!
     * @brief Accesses underlying native Metal texture pointer.
     */
    [[nodiscard]] virtual MTL::Texture* GetNativeTexture() const noexcept = 0;
    
    /*!
     * @brief Accesses underlying native Metal sampler state pointer.
     */
    [[nodiscard]] virtual MTL::SamplerState* GetNativeSampler() const noexcept = 0;
  };
  
} // namespace KanVerse
