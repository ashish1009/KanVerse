/*!
 * @file Tonemap.metal
 * @brief Fullscreen ACES tonemapping and HDR resolve pass with Bloom composite.
 * @details Generates an oversized viewport-covering triangle in clip space without
 *          vertex buffers, composites the blurred compute bloom texture with the HDR
 *          scene buffer, and maps the result to SDR via the ACES filmic curve.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include <metal_stdlib>
using namespace metal;

/*!
 * @struct TonemapVertexOutput
 * @brief Interpolated vertex stage payload passed into the tonemapping fragment shader.
 */
struct TonemapVertexOutput {
  float4 position [[position]]; ///< Clip-space coordinate covering NDC [-1, 1].
  float2 texCoord;             ///< UV texture coordinate covering [0, 1].
};

/*!
 * @struct TonemapUniforms
 * @brief Per-frame exposure and bloom composite parameters.
 */
struct TonemapUniforms {
  float bloomIntensity; ///< Multiplier scaling the additive bloom contribution.
  float exposure;       ///< Linear exposure multiplier applied to the scene color.
  float2 padding;       ///< 16-byte alignment padding.
};

/*!
 * @brief Synthesizes an oversized viewport-covering triangle using vertex ID indices.
 * @param vertexID Hardware generated vertex identifier (0, 1, 2).
 * @return TonemapVertexOutput with normalized coordinates.
 */
vertex TonemapVertexOutput tonemap_vertex(uint vertexID [[vertex_id]]) {
  TonemapVertexOutput out;
  // Three vertices: (-1, -1), (3, -1), (-1, 3) covering the full viewport
  out.texCoord = float2((vertexID << 1) & 2, vertexID & 2);
  out.position = float4(out.texCoord * 2.0 - 1.0, 0.0, 1.0);
  out.texCoord.y = 1.0 - out.texCoord.y; // Correct Y orientation for Metal textures
  return out;
}

/*!
 * @brief Evaluates the Narkowicz 2015 ACES Filmic curve fit for natural highlight compression.
 * @param x Linear radiant color vector.
 * @return Tonemapped linear color clamped within [0, 1].
 */
float3 ACESFilm(float3 x) {
  constexpr float a = 2.51f;
  constexpr float b = 0.03f;
  constexpr float c = 2.43f;
  constexpr float d = 0.59f;
  constexpr float e = 0.14f;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);
}

/*!
 * @brief Resolves HDR scene radiance and bloom into tonemapped SDR output for the swapchain.
 * @param in Rasterized stage inputs containing UVs.
 * @param hdrTexture Primary offscreen RGBA16Float HDR color texture.
 * @param bloomTexture Accumulated compute bloom texture.
 * @param linearSampler Bilinear clamp sampler.
 * @param uniforms Composite parameters containing exposure and bloom intensity.
 * @return 32-bit presentation color.
 */
fragment float4 tonemap_fragment(TonemapVertexOutput in             [[stage_in]],
                                 texture2d<float>    hdrTexture     [[texture(0)]],
                                 texture2d<float>    bloomTexture   [[texture(1)]],
                                 sampler             linearSampler  [[sampler(0)]],
                                 constant TonemapUniforms& uniforms [[buffer(0)]]) {
  // Sample scene radiance and blurred bloom
  const float3 hdrColor   = hdrTexture.sample(linearSampler, in.texCoord).rgb;
  const float3 bloomColor = bloomTexture.sample(linearSampler, in.texCoord).rgb;
  
  // Composite scene color with scaled bloom optical glow
  const float3 combined = (hdrColor * uniforms.exposure) + (bloomColor * uniforms.bloomIntensity);
  
  // Compress dynamic range using ACES Filmic curve
  const float3 finalMapped = ACESFilm(combined);
  
  return float4(finalMapped, 1.0f);
}
