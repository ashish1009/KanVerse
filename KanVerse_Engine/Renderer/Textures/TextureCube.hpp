/*!
 * @file TextureCube.hpp
 * @brief Abstract contract representing hardware-accelerated GPU cubemap textures.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include <simd/simd.h>

namespace MTL {
  class Texture;
  class SamplerState;
  class RenderCommandEncoder;
}

namespace KanVerse {
  
  /*!
   * @struct ProceduralSkyParams
   * @brief Configuration controlling procedural atmospheric gradients and the physical sun disc.
   */
  struct ProceduralSkyParams {
    simd::float3 ZenithColor{0.25f, 0.50f, 0.95f};      ///< Deep sky color overhead
    simd::float3 HorizonColor{0.70f, 0.82f, 0.98f};     ///< Atmospheric haze along the horizon
    simd::float3 GroundColor{0.18f, 0.20f, 0.24f};      ///< Ground reflectance below horizon
    
    simd::float3 SunDirection{-0.4f, -1.0f, -0.3f};     ///< Incident sunlight vector pointing down
    simd::float3 SunColor{1.0f, 0.95f, 0.88f};          ///< Radiance tint of the sun disc
    float        SunIntensity{12.0f};                   ///< High dynamic range intensity for bloom flare
    float        SunDiscSize{0.035f};                   ///< Angular radius of the sharp core disc
    float        SunGlowSize{0.32f};                    ///< Soft halo/corona radius around the disc
  };
  
  class TextureCube {
  public:
    virtual ~TextureCube() = default;
    
    /*!
     * @brief Factory creating a Cubemap texture from 6 individual image files (+X, -X, +Y, -Y, +Z, -Z).
     * @param faces Vector of 6 file paths.
     * @return Shared pointer to initialized TextureCube, or nullptr on failure.
     */
    [[nodiscard]] static std::shared_ptr<TextureCube> Create(const std::vector<std::string>& faces);
    
    /*!
     * @brief Factory procedurally synthesizing a default sky-to-ground environment cubemap.
     * @param resolution Resolution of each square face (e.g., 256 pixels).
     * @return Shared pointer to initialized procedural TextureCube.
     */
    [[nodiscard]] static std::shared_ptr<TextureCube> CreateProcedural(uint32_t resolution = 256);
    
    /*!
     * @brief Factory procedurally synthesizing a sky environment with customizable sun and colors.
     * @param params Sky colors, sun direction, and intensity.
     * @param resolution Resolution of each square face (e.g., 256 pixels).
     * @return Shared pointer to initialized procedural TextureCube.
     */
    [[nodiscard]] static std::shared_ptr<TextureCube> CreateProcedural(const ProceduralSkyParams& params, uint32_t resolution = 256);
    
    /*!
     * @brief Factory creating a Cubemap texture from a single equirectangular .hdr image via GPU Compute.
     * @param filepath Path to the .hdr image file.
     * @param resolution Target resolution of each cubemap face (default: 512).
     * @return Shared pointer to initialized HDR TextureCube.
     */
    [[nodiscard]] static std::shared_ptr<TextureCube> CreateFromEquirectangular(const std::string& filepath, uint32_t resolution = 512);
    
    /*!
     * @brief Convolutes an existing environment map to generate a diffuse Irradiance map.
     * @param sourceEnvMap The crisp source cubemap to sample from.
     * @param resolution Resolution of the output irradiance map (defaults to 32 for extreme blur).
     * @return Shared pointer to the new blurred TextureCube.
     */
    [[nodiscard]] static std::shared_ptr<TextureCube> CreateIrradianceMap(const std::shared_ptr<TextureCube>& sourceEnvMap, uint32_t resolution = 32);
    
    virtual void Bind(MTL::RenderCommandEncoder* encoder,
                      uint32_t textureSlot = 0,
                      uint32_t samplerSlot = 0) const = 0;
    
    [[nodiscard]] virtual uint32_t GetResolution() const noexcept = 0;
    [[nodiscard]] virtual MTL::Texture* GetNativeTexture() const noexcept = 0;
  };
  
} // namespace KanVerse
