/*!
 * @file Bloom.metal
 * @brief Metal compute kernels for Dual-Kawase HDR bloom downsampling and upsampling.
 * @details Implements a multi-pass compute blur chain. The downsample stage extracts
 *          high-luminance pixels using a soft-knee threshold and downsamples with a
 *          13-tap anti-flicker kernel. The upsample stage expands lower-resolution mips
 *          using a 9-tap 3x3 tent filter with progressive additive accumulation.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include <metal_stdlib>
using namespace metal;

/*!
 * @struct BloomDownsampleUniforms
 * @brief Parameters for the compute downsampling pass (strictly 32 bytes).
 */
struct BloomDownsampleUniforms {
  float2 texelSize;   ///< Inverse dimensions of the source texture (8 bytes, offset 0).
  float  threshold;   ///< Minimum luminance cut-off value (4 bytes, offset 8).
  float  softKnee;    ///< Soft transition factor [0, 1] (4 bytes, offset 12).
  int    mipLevel;    ///< Current downsample hierarchy level (4 bytes, offset 16).
  float  padding[3];  ///< Array of 3 floats for 32-byte total alignment (12 bytes, offset 20).
};

/*!
 * @struct BloomUpsampleUniforms
 * @brief Parameters for the compute upsampling and additive blending pass (strictly 16 bytes).
 */
struct BloomUpsampleUniforms {
  float2 texelSize;     ///< Inverse dimensions of the lower-resolution mip texture (8 bytes).
  float  filterRadius;  ///< Blur sample radius expansion scalar (4 bytes).
  float  padding;       ///< Alignment padding (4 bytes).
};

/*!
 * @brief Computes relative perceived luminance using standard Rec. 709 coefficients.
 * @param color Linear RGB color vector.
 * @return Scalar luminance value.
 */
float Luminance(float3 color) {
  return dot(color, float3(0.2126f, 0.7152f, 0.0722f));
}

/*!
 * @brief Applies a quadratic soft-knee threshold curve to extract bright HDR highlights.
 * @param color Unfiltered source pixel color.
 * @param threshold Luminance threshold cut-off.
 * @param softKnee Knee sharpness factor.
 * @return Highlight color energy exceeding threshold.
 */
float3 Prefilter(float3 color, float threshold, float softKnee) {
  const float brightness = max(color.r, max(color.g, color.b));
  const float knee = threshold * softKnee;
  float soft = brightness - threshold + knee;
  soft = clamp(soft, 0.0f, 2.0f * knee);
  soft = (soft * soft) / (4.0f * max(knee, 1e-4f));
  
  float contribution = max(soft, brightness - threshold);
  contribution /= max(brightness, 1e-4f);
  return color * max(contribution, 0.0f);
}

// =============================================================================
// Compute Kernel: Downsample with 13-Tap Anti-Flicker Filter Pattern
// =============================================================================

/*!
 * @brief Downsamples the source texture by 2x using a 13-tap weighted box-and-cross kernel.
 * @param srcTexture Source input texture (sampled via bilinear clamp).
 * @param dstTexture Destination downscaled texture (half resolution).
 * @param uniforms Downsample parameters and thresholding settings.
 * @param linearSampler Bilinear clamp sampler.
 * @param gid 2D thread coordinate corresponding to target destination texel.
 */
kernel void bloom_downsample(texture2d<float, access::sample> srcTexture     [[texture(0)]],
                             texture2d<float, access::write>  dstTexture     [[texture(1)]],
                             constant BloomDownsampleUniforms& uniforms      [[buffer(0)]],
                             sampler                          linearSampler  [[sampler(0)]],
                             uint2                            gid            [[thread_position_in_grid]]) {
  // Boundary check against destination texture dimensions
  if (gid.x >= dstTexture.get_width() || gid.y >= dstTexture.get_height()) {
    return;
  }
  
  // Center UV coordinate in destination grid
  const float2 uv = (float2(gid) + 0.5f) * uniforms.texelSize;
  const float2 d  = uniforms.texelSize;
  
  // Sample 13 taps surrounding the central point to suppress sub-pixel flickering
  const float3 a = srcTexture.sample(linearSampler, uv + float2(-2.0f * d.x,  2.0f * d.y)).rgb;
  const float3 b = srcTexture.sample(linearSampler, uv + float2( 0.0f,        2.0f * d.y)).rgb;
  const float3 c = srcTexture.sample(linearSampler, uv + float2( 2.0f * d.x,  2.0f * d.y)).rgb;
  
  const float3 d0 = srcTexture.sample(linearSampler, uv + float2(-2.0f * d.x,  0.0f)).rgb;
  const float3 e  = srcTexture.sample(linearSampler, uv).rgb;
  const float3 f  = srcTexture.sample(linearSampler, uv + float2( 2.0f * d.x,  0.0f)).rgb;
  
  const float3 g = srcTexture.sample(linearSampler, uv + float2(-2.0f * d.x, -2.0f * d.y)).rgb;
  const float3 h = srcTexture.sample(linearSampler, uv + float2( 0.0f,       -2.0f * d.y)).rgb;
  const float3 i = srcTexture.sample(linearSampler, uv + float2( 2.0f * d.x, -2.0f * d.y)).rgb;
  
  const float3 j = srcTexture.sample(linearSampler, uv + float2(-d.x,  d.y)).rgb;
  const float3 k = srcTexture.sample(linearSampler, uv + float2( d.x,  d.y)).rgb;
  const float3 l = srcTexture.sample(linearSampler, uv + float2(-d.x, -d.y)).rgb;
  const float3 m = srcTexture.sample(linearSampler, uv + float2( d.x, -d.y)).rgb;
  
  // Weighted integration: center (12.5%), corners (12.5%), edges (25%), inner square (50%)
  float3 sampleSum = (e * 0.125f) +
  ((a + c + g + i) * 0.03125f) +
  ((b + d0 + f + h) * 0.0625f) +
  ((j + k + l + m) * 0.125f);
  
  // Apply bright pass prefilter strictly on the base mip (level 0)
  if (uniforms.mipLevel == 0) {
    sampleSum = Prefilter(sampleSum, uniforms.threshold, uniforms.softKnee);
  }
  
  dstTexture.write(float4(sampleSum, 1.0f), gid);
}

// =============================================================================
// Compute Kernel: Upsample with 9-Tap 3x3 Tent Filter & Additive Accumulation
// =============================================================================

/*!
 * @brief Upsamples a lower-resolution mip, blurs via a 3x3 tent filter, and adds to the higher mip.
 * @param srcLowerMip Coarser mip level to upsample.
 * @param srcHigherMip Finer mip level to blend into.
 * @param dstTexture Output destination holding the accumulated blur.
 * @param uniforms Filter radius and texel size configuration.
 * @param linearSampler Bilinear clamp sampler.
 * @param gid 2D thread coordinate in the target higher-resolution grid.
 */
kernel void bloom_upsample(texture2d<float, access::sample> srcLowerMip   [[texture(0)]],
                           texture2d<float, access::sample> srcHigherMip  [[texture(1)]],
                           texture2d<float, access::write>  dstTexture    [[texture(2)]],
                           constant BloomUpsampleUniforms&  uniforms      [[buffer(0)]],
                           sampler                          linearSampler [[sampler(0)]],
                           uint2                            gid           [[thread_position_in_grid]]) {
  // Boundary check against destination texture dimensions
  if (gid.x >= dstTexture.get_width() || gid.y >= dstTexture.get_height()) {
    return;
  }
  
  const float2 uv = (float2(gid) + 0.5f) * uniforms.texelSize;
  const float2 d  = uniforms.texelSize * uniforms.filterRadius;
  
  // 9-tap 3x3 tent filter sampling pattern
  const float3 a = srcLowerMip.sample(linearSampler, uv + float2(-d.x,  d.y)).rgb;
  const float3 b = srcLowerMip.sample(linearSampler, uv + float2( 0.0f, d.y)).rgb * 2.0f;
  const float3 c = srcLowerMip.sample(linearSampler, uv + float2( d.x,  d.y)).rgb;
  
  const float3 d0 = srcLowerMip.sample(linearSampler, uv + float2(-d.x,  0.0f)).rgb * 2.0f;
  const float3 e  = srcLowerMip.sample(linearSampler, uv).rgb * 4.0f;
  const float3 f  = srcLowerMip.sample(linearSampler, uv + float2( d.x,  0.0f)).rgb * 2.0f;
  
  const float3 g = srcLowerMip.sample(linearSampler, uv + float2(-d.x, -d.y)).rgb;
  const float3 h = srcLowerMip.sample(linearSampler, uv + float2( 0.0f, -d.y)).rgb * 2.0f;
  const float3 i = srcLowerMip.sample(linearSampler, uv + float2( d.x, -d.y)).rgb;
  
  // Normalization factor: 1 + 2 + 1 + 2 + 4 + 2 + 1 + 2 + 1 = 16
  const float3 upsampled = (a + b + c + d0 + e + f + g + h + i) * (1.0f / 16.0f);
  
  // Additive accumulation: blend upsampled blur with current mip's details
  const float3 current = srcHigherMip.sample(linearSampler, uv).rgb;
  const float3 accumulated = current + upsampled;
  
  dstTexture.write(float4(accumulated, 1.0f), gid);
}
