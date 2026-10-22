/*!
 * @file Skybox.metal
 * @brief High-throughput depth-only vertex shader for directional shadow map rasterization.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include <metal_stdlib>
using namespace metal;

struct SkyboxVertexOut {
  float4 position [[position]];
  float3 viewDir;
};

// We pass the inverse matrices to un-project the screen back into the 3D world
struct SkyboxUniforms {
  float4x4 inverseView;
  float4x4 inverseProjection;
};

vertex SkyboxVertexOut skybox_vertex(uint vertexID [[vertex_id]],
                                     constant SkyboxUniforms& uniforms [[buffer(0)]]) {
  // Generate a fullscreen triangle without needing a vertex buffer
  float2 uv = float2((vertexID << 1) & 2, vertexID & 2);
  float4 clipPos = float4(uv * 2.0f - 1.0f, 1.0f, 1.0f); // Z = 1.0 pushes it to the absolute background
  
  SkyboxVertexOut out;
  out.position = clipPos;
  
  // Un-project from 2D screen space to 3D view space
  float4 viewPos = uniforms.inverseProjection * clipPos;
  viewPos /= viewPos.w;
  
  // Convert to world space direction (w = 0.0 ignores camera translation so the skybox feels infinitely far away)
  out.viewDir = (uniforms.inverseView * float4(viewPos.xyz, 0.0f)).xyz;
  
  // Invert the Y axis to match Metal's texture coordinate system
  out.viewDir.y = -out.viewDir.y;
  
  return out;
}

fragment float4 skybox_fragment(SkyboxVertexOut in [[stage_in]],
                                texturecube<float> envMap [[texture(0)]],
                                sampler envSampler [[sampler(0)]]) {
  // Sample the cubemap using the calculated 3D ray direction
  float3 dir = normalize(in.viewDir);
  return envMap.sample(envSampler, dir);
}
