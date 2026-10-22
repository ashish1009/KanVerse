/*!
 * @file Irradiance.metal
 * @brief High-throughput depth-only vertex shader for directional shadow map rasterization.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include <metal_stdlib>
using namespace metal;

constant float PI = 3.14159265359f;

kernel void irradiance_convolution(
                                   texturecube<float, access::sample>    envMap        [[texture(0)]],
                                   texture2d_array<float, access::write> irradianceMap [[texture(1)]],
                                   uint3                                 tid           [[thread_position_in_grid]])
{
  uint2 size = uint2(irradianceMap.get_width(), irradianceMap.get_height());
  if (tid.x >= size.x || tid.y >= size.y || tid.z >= 6) return;
  
  // Map thread ID to [-1.0, 1.0] for the current face
  float2 uv = float2(tid.xy) / float2(size.x - 1, size.y - 1);
  uv = uv * 2.0f - 1.0f;
  uv.y = -uv.y;
  
  float3 N;
  switch(tid.z) {
    case 0: N = float3( 1.0f,  uv.y, -uv.x); break; // +X
    case 1: N = float3(-1.0f,  uv.y,  uv.x); break; // -X
    case 2: N = float3( uv.x,  1.0f, -uv.y); break; // +Y
    case 3: N = float3( uv.x, -1.0f,  uv.y); break; // -Y
    case 4: N = float3( uv.x,  uv.y,  1.0f); break; // +Z
    case 5: N = float3(-uv.x,  uv.y, -1.0f); break; // -Z
  }
  N = normalize(N);
  
  // Tangent space calculation
  float3 up    = float3(0.0f, 1.0f, 0.0f);
  float3 right = normalize(cross(up, N));
  up           = normalize(cross(N, right));
  
  float3 irradiance = float3(0.0f);
  float sampleDelta = 0.025f;
  float nrSamples = 0.0f;
  
  constexpr sampler linearSampler(min_filter::linear, mag_filter::linear, mip_filter::linear);
  
  // Integrate over the hemisphere
  for(float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta) {
    for(float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta) {
      // Spherical to cartesian (in tangent space)
      float3 tangentSample = float3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
      // Tangent space to world
      float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
      
      irradiance += envMap.sample(linearSampler, sampleVec).rgb * cos(theta) * sin(theta);
      nrSamples++;
    }
  }
  
  irradiance = PI * irradiance * (1.0f / nrSamples);
  
  // FIX: Tone down and clamp extreme HDR hot-spots so they don't blow out into white splotches
  irradiance = irradiance / (irradiance + 1.0f); // Simple Reinhard tone operator for ambient term
  
  // Write out the smooth, balanced irradiance
  irradianceMap.write(float4(irradiance, 1.0f), tid.xy, tid.z);
}
