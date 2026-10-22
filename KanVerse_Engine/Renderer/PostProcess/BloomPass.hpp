/*!
 * @file BloomPass.hpp
 * @brief Multi-mip Dual-Kawase compute bloom blur subsystem.
 * @details Manages downsampling and upsampling mip-pyramid textures, compiles
 *          compute pipeline states, and executes compute dispatches to generate
 *          cinematic optical glow around high-radiance fragments.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Core/Math/Math.hpp"
#include <cstdint>
#include <vector>

namespace MTL {
  class Texture;
  class ComputePipelineState;
  class CommandBuffer;
  class SamplerState;
}

namespace KanVerse {
  
  /*!
   * @class BloomPass
   * @brief Manages a pyramid of downsampled and upsampled textures and executes compute blur dispatches.
   */
  class BloomPass {
  public:
    static constexpr uint32_t kMipLevels = 4; ///< Number of progressive 2x downsample steps.
    
    BloomPass();
    ~BloomPass();
    
    /*!
     * @brief Allocates compute pipelines and default linear clamp samplers.
     */
    void Init();
    
    /*!
     * @brief Reallocates downsample and upsample mip-chain textures when viewport size mutates.
     * @param baseWidth Primary viewport width in pixels.
     * @param baseHeight Primary viewport height in pixels.
     */
    void Resize(uint32_t baseWidth, uint32_t baseHeight);
    
    /*!
     * @brief Encodes the complete Dual-Kawase downsample and upsample blur pyramid on the GPU.
     * @param cmdBuffer Active command buffer to append compute dispatches to.
     * @param hdrSource The offscreen RGBA16Float scene texture to extract highlights from.
     */
    void Execute(MTL::CommandBuffer* cmdBuffer, MTL::Texture* hdrSource);
    
    /*!
     * @brief Accesses the final blurred and blended bloom texture (at mip 0 resolution).
     * @return Native MTL::Texture pointer.
     */
    [[nodiscard]] MTL::Texture* GetBloomTexture() const noexcept;
    
    /*!
     * @brief Sets the minimum luminance threshold for bloom extraction.
     * @param threshold Cutoff scalar (default: 1.1f).
     */
    void SetThreshold(float threshold) noexcept { m_Threshold = threshold; }
    
    /*!
     * @brief Sets the soft-knee transition factor avoiding harsh cutoffs.
     * @param knee Normalized scalar [0, 1].
     */
    void SetSoftKnee(float knee) noexcept { m_SoftKnee = knee; }
    
    /*!
     * @brief Sets the additive composite intensity multiplier.
     * @param intensity Energy scalar.
     */
    void SetIntensity(float intensity) noexcept { m_Intensity = intensity; }
    
    [[nodiscard]] float GetIntensity() const noexcept { return m_Intensity; }
    
  private:
    // Memory layout matching BloomDownsampleUniforms in Bloom.metal (strictly 32 bytes)
    struct alignas(16) DownsampleUniforms {
      Math::Vec2 TexelSize;         ///< 8 bytes (offset 0)
      float      Threshold{1.1f};   ///< 4 bytes (offset 8)
      float      SoftKnee{0.5f};    ///< 4 bytes (offset 12)
      int32_t    MipLevel{0};       ///< 4 bytes (offset 16)
      float      Padding[3]{0.0f, 0.0f, 0.0f}; ///< 12 bytes (offset 20) -> Total: 32 bytes
    };
    
    // Memory layout matching BloomUpsampleUniforms in Bloom.metal (strictly 16 bytes)
    struct alignas(16) UpsampleUniforms {
      Math::Vec2 TexelSize;         ///< 8 bytes (offset 0)
      float      FilterRadius{1.0f};///< 4 bytes (offset 8)
      float      Padding{0.0f};     ///< 4 bytes (offset 12) -> Total: 16 bytes
    };
    
    void ReleaseTextures();
    
    uint32_t m_BaseWidth{0};
    uint32_t m_BaseHeight{0};
    float    m_Threshold{1.1f};
    float    m_SoftKnee{0.5f};
    float    m_Intensity{0.8f};
    
    MTL::ComputePipelineState* m_DownsamplePipeline{nullptr};
    MTL::ComputePipelineState* m_UpsamplePipeline{nullptr};
    MTL::SamplerState*         m_LinearSampler{nullptr};
    
    std::vector<MTL::Texture*> m_DownsampleMips; ///< Downsampled textures: [1/2, 1/4, 1/8, 1/16].
    std::vector<MTL::Texture*> m_UpsampleMips;   ///< Upsampled accumulation textures.
  };
  
} // namespace KanVerse
