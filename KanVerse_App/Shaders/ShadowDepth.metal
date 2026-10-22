/*!
 * @file ShadowDepth.metal
 * @brief High-throughput depth-only vertex shader for directional shadow map rasterization.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include <metal_stdlib>
using namespace metal;

struct VertexInput {
  float3 position [[attribute(0)]];
};

struct ShadowUniforms {
  float4x4 lightViewProjectionMatrix;
};

struct InstanceData {
  float4x4 modelMatrix;
  float4x4 normalMatrix;
};

struct ShadowVertexOutput {
  float4 clipPosition [[position]];
};

vertex ShadowVertexOutput shadow_vertex_main(VertexInput             in            [[stage_in]],
                                             constant ShadowUniforms& uniforms      [[buffer(1)]],
                                             constant InstanceData*  instances     [[buffer(3)]],
                                             uint                    instanceID    [[instance_id]]) {
  ShadowVertexOutput out;
  constant InstanceData& instance = instances[instanceID];
  
  float4 worldPosition = instance.modelMatrix * float4(in.position, 1.0);
  out.clipPosition     = uniforms.lightViewProjectionMatrix * worldPosition;
  
  return out;
}
