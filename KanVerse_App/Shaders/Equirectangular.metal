/*!
 * @file Equirectangular.metal
 * @brief High-throughput depth-only vertex shader for directional shadow map rasterization.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include <metal_stdlib>
using namespace metal;

// Helper function to map a 3D unit direction vector to a 2D Spherical (Equirectangular) UV coordinate
float2 SampleSphericalMap(float3 v) {
  // Add clamp to prevent NaN at absolute poles due to float imprecision
  float2 uv = float2(atan2(v.z, v.x), asin(clamp(v.y, -1.0f, 1.0f)));
  
  // Scale by inverse of 2*PI and PI to normalize to [0.0, 1.0]
  uv *= float2(0.15915494309f, 0.31830988618f);
  uv += 0.5f;
  return uv;
}

kernel void equirectangular_to_cubemap(
                                       texture2d<float, access::sample>      equirectangularTexture [[texture(0)]],
                                       texture2d_array<float, access::write> cubemapTexture         [[texture(1)]],
                                       uint3                                 tid                    [[thread_position_in_grid]])
{
  uint2 size = uint2(cubemapTexture.get_width(), cubemapTexture.get_height());
  
  if (tid.x >= size.x || tid.y >= size.y || tid.z >= 6) {
    return;
  }
  
  float2 uv = float2(tid.xy) / float2(size.x - 1, size.y - 1);
  uv = uv * 2.0f - 1.0f;
  uv.y = -uv.y;
  
  float3 dir;
  
  switch(tid.z) {
    case 0: dir = float3( 1.0f,  uv.y, -uv.x); break; // +X (Right)
    case 1: dir = float3(-1.0f,  uv.y,  uv.x); break; // -X (Left)
    case 2: dir = float3( uv.x,  1.0f, -uv.y); break; // +Y (Top)
    case 3: dir = float3( uv.x, -1.0f,  uv.y); break; // -Y (Bottom)
    case 4: dir = float3( uv.x,  uv.y,  1.0f); break; // +Z (Front)
    case 5: dir = float3(-uv.x,  uv.y, -1.0f); break; // -Z (Back)
  }
  
  dir = normalize(dir);
  
  // --- REPLACE YOUR OLD SAMPLER WITH THIS ONE HERE ---
  constexpr sampler linearSampler(
                                  coord::normalized,
                                  min_filter::linear,
                                  mag_filter::linear,
                                  mip_filter::none,
                                  s_address::repeat,
                                  t_address::clamp_to_edge
                                  );
  
  float4 color = equirectangularTexture.sample(linearSampler, SampleSphericalMap(dir));
  cubemapTexture.write(color, tid.xy, tid.z);
}
