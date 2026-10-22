/*!
 * @file Texture2D.hpp
 * @brief 2D Texture asset abstraction encapsulating Metal texture objects and samplers.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace MTL {
  class Texture;
  class SamplerState;
  class RenderCommandEncoder;
}

namespace KanVerse {
  
  /**
   * @brief Configuration specification for creating a 2D Texture.
   */
  struct TextureSpecification {
    uint32_t Width{1};
    uint32_t Height{1};
    bool     GenerateMipmaps{false};
    
    /**
     * @brief Determines the color space of the texture.
     * True (sRGB) for color textures (Albedo, Emissive).
     * False (Linear) for data textures (Normal, Roughness, Metallic, AO, Height).
     */
    bool     sRGB{true};
  };
  
  /**
   * @brief Cross-platform interface for an Engine 2D Texture.
   */
  class Texture2D {
  public:
    virtual ~Texture2D() = default;
    
    /**
     * @brief Creates a 2D texture based on a manual specification.
     * @param spec The specification dictating dimensions and format.
     * @return A shared pointer to the created texture.
     */
    [[nodiscard]] static std::shared_ptr<Texture2D> Create(const TextureSpecification& spec);
    
    /**
     * @brief Loads and creates a 2D texture from an image file.
     * @param filepath The absolute or relative path to the image file.
     * @param sRGB Whether to load the image in sRGB color space.
     * @return A shared pointer to the created texture, or nullptr if loading failed.
     */
    [[nodiscard]] static std::shared_ptr<Texture2D> Create(const std::string& filepath, bool sRGB = true);
    
    /**
     * @brief Generates a default checkerboard texture (useful as a fallback for missing textures).
     * @param resolution The width and height of the square texture.
     * @return A shared pointer to the checkerboard texture.
     */
    [[nodiscard]] static std::shared_ptr<Texture2D> CreateCheckerboard(uint32_t resolution = 256);
    
    /**
     * @brief Uploads raw pixel data to the GPU texture.
     * @param data Pointer to the raw pixel buffer.
     * @param size The size of the buffer in bytes.
     */
    virtual void SetData(const void* data, size_t size) = 0;
    
    /**
     * @brief Binds the texture and its sampler to the current render pipeline.
     * @param encoder The Metal render command encoder.
     * @param textureSlot The slot index to bind the texture to.
     * @param samplerSlot The slot index to bind the sampler state to.
     */
    virtual void Bind(MTL::RenderCommandEncoder* encoder,
                      uint32_t textureSlot = 0,
                      uint32_t samplerSlot = 0) const = 0;
    
    [[nodiscard]] virtual uint32_t GetWidth() const noexcept = 0;
    [[nodiscard]] virtual uint32_t GetHeight() const noexcept = 0;
    [[nodiscard]] virtual MTL::Texture* GetNativeTexture() const noexcept = 0;
  };
}
