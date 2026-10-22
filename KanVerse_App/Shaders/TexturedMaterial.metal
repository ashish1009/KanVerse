/*!
 * @file TexturedMaterial.metal
 * @brief Cook-Torrance PBR shader with seamless reflection sampling, smooth shadow frustum fade, and point lights.
 * @copyright KanVerse Engine
 */

#include <metal_stdlib>
using namespace metal;

// Mathematical constant Pi for energy-conserving BRDF normalization
constant float PI = 3.14159265358979323846f;

// Maximum number of dynamic point lights evaluated per fragment (matches Renderer::kMaxPointLights in C++)
constant uint kMaxPointLights = 64;

/*
 * ============================================================================
 * 1. VERTEX STAGE INPUT & OUTPUT STRUCTURES
 * ============================================================================
 * VertexInput maps directly to the MTL::VertexDescriptor bound at [[buffer(0)]].
 * Note that in Metal, [[stage_in]] unpacks attributes from the vertex buffer
 * using the offsets defined in Renderer::GetStandardMeshLayout().
 */
struct VertexInput {
  float3 position  [[attribute(0)]]; // Offset 0  (16 bytes in C++ simd::float3)
  float3 normal    [[attribute(1)]]; // Offset 16 (16 bytes in C++ simd::float3)
  float4 color     [[attribute(2)]]; // Offset 32 (16 bytes in C++ simd::float4)
  float2 texCoord  [[attribute(3)]]; // Offset 48 (8 bytes in C++ simd::float2)
};

/*
 * Interpolated payload passed from the Vertex Shader to the Rasterizer and Fragment Shader.
 */
struct VertexOutput {
  float4 position [[position]]; // Homogeneous clip-space position (x, y, z, w) for rasterization
  float3 worldPosition;         // Fragment position in 3D world space (used for lighting & derivatives)
  float3 worldNormal;           // Geometric surface normal in 3D world space
  float4 color;                 // Per-vertex color multiplier
  float2 texCoord;              // Base 2D UV coordinates [0, 1]
  float4 shadowPosition;        // Fragment position projected into the Sun's orthographic light space
};

/*
 * ============================================================================
 * 2. CONSTANT BUFFER STRUCTURES (MUST MATCH C++ MEMORY LAYOUT BIT-FOR-BIT)
 * ============================================================================
 */

/*
 * Matches KanVerse::GPUPointLight (48 bytes total, 16-byte aligned).
 * Packs scalars into float4s to prevent hidden SIMD padding discrepancies.
 */
struct GPUPointLight {
  float4 positionAndRadius;  // .xyz = World Position, .w = Cutoff Radius in meters
  float4 colorAndIntensity;  // .rgb = Linear RGB Color, .a = Luminous Intensity multiplier
  float4 attenuation;        // .x = Constant, .y = Linear, .z = Quadratic falloff, .w = Unused
};

/*
 * Bound at [[buffer(1)]] in both Vertex and Fragment stages.
 * Matches KanVerse::Renderer::GPUUniforms in Renderer.hpp (256-byte aligned).
 * Note: In Metal Shading Language, `float3` occupies 16 bytes (same as C++ `simd::float3`).
 */
struct GPUUniforms {
  float4x4      viewProjectionMatrix;             // Camera Projection * View matrix (64 bytes, offset 0)
  float4x4      lightSpaceMatrix;                 // Sun Orthographic Proj * View matrix (64 bytes, offset 64)
  float3        cameraPosition;                   // World-space eye coordinates (16 bytes, offset 128)
  float3        lightDirection;                   // Normalized direction the sun rays travel (16 bytes, offset 144)
  float4        directionalColor;                 // Sun RGB color * intensity (16 bytes, offset 160)
  float4        ambientColor;                     // Sky ambient RGB * intensity (16 bytes, offset 176)
  uint          pointLightCount;                  // Active point lights in the scene [0..64] (4 bytes, offset 192)
  uint          padding[3];                       // 12-byte padding to align pointLights array to 16-byte boundary (offset 196)
  GPUPointLight pointLights[kMaxPointLights];     // Array of 64 point lights (64 * 48 = 3072 bytes, offset 208)
};

/*
 * Bound at [[buffer(3)]] in the Vertex stage.
 * Matches KanVerse::InstanceData in Renderer.hpp (128 bytes per instance).
 */
struct InstanceData {
  float4x4 modelMatrix;  // Local-to-World transformation matrix (64 bytes)
  float4x4 normalMatrix; // Transpose(Inverse(modelMatrix)) to transform normals under non-uniform scale (64 bytes)
};

/*
 * Bound at [[buffer(2)]] in the Fragment stage.
 * Matches KanVerse::MaterialUniforms in Material.hpp (256-byte aligned).
 */
struct MaterialUniforms {
  float4   baseColor;        // RGBA Albedo tint (16 bytes, offset 0)
  float4   specularColor;    // Specular reflectance tint (16 bytes, offset 16)
  float4   emissiveColor;    // HDR self-illumination radiance for bloom (16 bytes, offset 32)
  float3   tilingFactor;     // World-axis UV tiling scale (16 bytes in MSL float3, offset 48)
  float    roughness;        // Microfacet roughness scalar [0.05..1.0] (4 bytes, offset 64)
  float    metallic;         // Metalness scalar [0.0..1.0] (4 bytes, offset 68)
  int32_t  hasAlbedoMap;     // 1 if [[texture(0)]] has a valid texture, 0 if using fallback (4 bytes, offset 72)
  int32_t  hasNormalMap;     // 1 if [[texture(1)]] has a valid normal map (4 bytes, offset 76)
  int32_t  hasRoughnessMap;  // 1 if [[texture(2)]] has a valid roughness map (4 bytes, offset 80)
  int32_t  hasMetallicMap;   // 1 if [[texture(3)]] has a valid metallic map (4 bytes, offset 84)
  int32_t  hasAOMap;         // 1 if [[texture(4)]] has a valid ambient occlusion map (4 bytes, offset 88)
  int32_t  hasHeightMap;     // 1 if [[texture(5)]] has a valid height map (4 bytes, offset 92)
  float    padding;          // Trailing alignment padding (4 bytes, offset 96)
};

/*
 * ============================================================================
 * 3. COOK-TORRANCE MICROFACET BRDF MATHEMATICAL FUNCTIONS
 * ============================================================================
 * The Cook-Torrance specular BRDF is defined as:
 *
 *               D(N, H, a) * G(N, V, L, k) * F(H, V, F0)
 *   f_spec = ----------------------------------------------
 *                    4 * dot(N, V) * dot(N, L)
 */

/*
 * Trowbridge-Reitz GGX Normal Distribution Function (D):
 * Computes the statistical fraction of microscopic surface facets that are aligned
 * with the halfway vector H. Lower roughness creates a tiny, blindingly bright highlight;
 * higher roughness spreads microfacets wide for a chalky appearance.
 */
inline float DistributionGGX(float3 N, float3 H, float roughness) {
  float a      = roughness * roughness; // Disney/Epic perceptual roughness remapping (alpha = roughness^2)
  float a2     = a * a;
  float NdotH  = max(dot(N, H), 0.0f);
  float NdotH2 = NdotH * NdotH;
  
  float num   = a2;
  float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
  denom       = PI * denom * denom;
  
  return num / max(denom, 1e-4f); // Guard against division by zero when roughness approaches 0
}

/*
 * Schlick-GGX Geometry Shadowing Helper:
 * Approximates how microfacets self-occlude or overshadow one another along a single vector.
 */
inline float GeometrySchlickGGX(float NdotV, float roughness) {
  // Direct lighting remapping: k = (roughness + 1)^2 / 8
  float r = (roughness + 1.0f);
  float k = (r * r) / 8.0f;
  
  float num   = NdotV;
  float denom = NdotV * (1.0f - k) + k;
  return num / max(denom, 1e-4f);
}

/*
 * Smith's Method for Geometry Attenuation (G):
 * Combines both view-direction microfacet masking (NdotV) and light-direction
 * microfacet shadowing (NdotL).
 */
inline float GeometrySmith(float3 N, float3 V, float3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0f);
  float NdotL = max(dot(N, L), 0.0f);
  float ggx2  = GeometrySchlickGGX(NdotV, roughness);
  float ggx1  = GeometrySchlickGGX(NdotL, roughness);
  return ggx1 * ggx2;
}

/*
 * Schlick's Approximation for the Fresnel Equation (F):
 * Computes the ratio of light that reflects at a given viewing angle (cosTheta).
 * At grazing angles (cosTheta -> 0), all surfaces approach 100% specular reflection.
 * F0 is the base reflectivity at normal incidence (0.04 for dielectrics, albedo for metals).
 */
inline float3 FresnelSchlick(float cosTheta, float3 F0) {
  return F0 + (1.0f - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

/*
 * ============================================================================
 * 4. VERTEX SHADER STAGE
 * ============================================================================
 * Executes once per vertex per instance. Uses hardware instancing ([[instance_id]])
 * to draw thousands of walls, floor tiles, and debris shards in a single draw call.
 */
vertex VertexOutput vertex_main(VertexInput             in         [[stage_in]],
                                constant GPUUniforms&   uniforms   [[buffer(1)]],
                                constant InstanceData*  instances  [[buffer(3)]],
                                uint                    instanceID [[instance_id]]) {
  VertexOutput out;
  
  // 1. Fetch this specific instance's transformation matrices from [[buffer(3)]]
  constant InstanceData& instance = instances[instanceID];
  
  // 2. Transform local vertex position into 3D World Space, then into Clip Space
  float4 worldPosition = instance.modelMatrix * float4(in.position, 1.0f);
  out.worldPosition    = worldPosition.xyz;
  out.position         = uniforms.viewProjectionMatrix * worldPosition;
  
  // 3. Transform local surface normal using the Inverse-Transpose normalMatrix
  //    (w = 0.0f ignores translation; normalMatrix prevents skewing on non-uniformly scaled walls)
  out.worldNormal = (instance.normalMatrix * float4(in.normal, 0.0f)).xyz;
  out.color       = in.color;
  out.texCoord    = in.texCoord;
  
  // 4. Normal-Offset Shadow Bias:
  //    Push the shadow lookup position 3.5 cm outward along the true geometric normal.
  //    This eliminates "shadow acne" staircase artifacts on flat ground and steep walls
  //    without causing "peter-panning" (detached shadows).
  float3 worldGeomNormal = normalize(out.worldNormal);
  float4 shadowWorldPos  = float4(worldPosition.xyz + worldGeomNormal * 0.035f, 1.0f);
  out.shadowPosition     = uniforms.lightSpaceMatrix * shadowWorldPos;
  
  return out;
}

/*
 * ============================================================================
 * 5. FRAGMENT SHADER STAGE
 * ============================================================================
 * Evaluates full Cook-Torrance PBR + PCF Shadows + 64 Point Lights + IBL per pixel.
 */
fragment float4 fragment_main(VertexOutput              in            [[stage_in]],
                              constant GPUUniforms&     uniforms      [[buffer(1)]],
                              constant MaterialUniforms& material     [[buffer(2)]],
                              texture2d<float>          albedoTex     [[texture(0)]],
                              texture2d<float>          normalTex     [[texture(1)]],
                              texture2d<float>          roughnessTex  [[texture(2)]],
                              texture2d<float>          metallicTex   [[texture(3)]],
                              texture2d<float>          aoTex         [[texture(4)]],
                              texture2d<float>          heightTex     [[texture(5)]],
                              depth2d<float>            shadowMap     [[texture(6)]],
                              texturecube<float>        envMap        [[texture(7)]],
                              texturecube<float>        irradianceMap [[texture(8)]],
                              sampler                   texSampler    [[sampler(0)]],
                              sampler                   shadowSampler [[sampler(1)]]) {
  // Re-normalize interpolated geometric normal across the triangle face
  float3 N_geom = normalize(in.worldNormal);
  float3 N      = N_geom;
  
  /*
   * ------------------------------------------------------------------------
   * Step A: Box-Aligned Triplanar UV Tiling
   * ------------------------------------------------------------------------
   * Scales the [0, 1] UVs based on which world axis the face normal is most aligned with:
   * - Front/Back walls (|N.z| > 0.5): tile by (Width X, Height Y)
   * - Left/Right walls (|N.x| > 0.5): tile by (Depth Z, Height Y)
   * - Floors/Ceilings  (|N.y| > 0.5): tile by (Width X, Depth Z)
   */
  float2 tiledUV = in.texCoord;
  if (abs(N_geom.z) > 0.5f) {
    tiledUV *= material.tilingFactor.xy;
  } else if (abs(N_geom.x) > 0.5f) {
    tiledUV *= material.tilingFactor.zy;
  } else {
    tiledUV *= material.tilingFactor.xz;
  }
  
  /*
   * ------------------------------------------------------------------------
   * Step B: Sample Albedo, Normal, Roughness, Metallic, and AO Maps
   * ------------------------------------------------------------------------
   */
  float3 texColor = float3(1.0f);
  if (material.hasAlbedoMap != 0) {
    // albedoTex is bound as RGBA8Unorm_sRGB, so hardware automatically converts sRGB -> Linear
    texColor = albedoTex.sample(texSampler, tiledUV).rgb;
  }
  float3 albedo = in.color.rgb * material.baseColor.rgb * texColor;
  
  /*
   * Screen-Space Derivative TBN Normal Mapping (Cotangent Frame):
   * Instead of storing extra float3 Tangent and Bitangent vectors in every Vertex
   * (which would bloat Vertex size from 64B to 96B), we compute the tangent basis (T, B, N)
   * on the fly using hardware 2x2 pixel quad partial derivatives (dfdx, dfdy).
   */
  if (material.hasNormalMap != 0) {
    // Unpack tangent-space normal from [0, 1] texture range to [-1, 1] vector range
    float3 normalPixel = normalTex.sample(texSampler, tiledUV).rgb;
    float3 localNormal = normalize(normalPixel * 2.0f - 1.0f);
    
    // Scale XY perturbation slightly (0.65) so extreme grazing angles on flat ground never invert NdotV
    localNormal.xy *= 0.65f;
    localNormal = normalize(localNormal);
    
    // Compute rate of change of 3D world position and 2D UVs across the 2x2 pixel quad
    float3 Q1     = dfdx(in.worldPosition);
    float3 Q2     = dfdy(in.worldPosition);
    float2 st1    = dfdx(tiledUV);
    float2 st2    = dfdy(tiledUV);
    
    float  det    = st1.x * st2.y - st2.x * st1.y;
    float  invDet = (det < 0.0f) ? -1.0f : 1.0f;
    float3 T_raw  = (Q1 * st2.y - Q2 * st1.y) * invDet;
    
    // Gram-Schmidt orthogonalization: project T perpendicular to N_geom
    float3 T_ortho = T_raw - N_geom * dot(N_geom, T_raw);
    float  tLenSq  = dot(T_ortho, T_ortho);
    if (tLenSq > 1e-8f) {
      float3 T     = T_ortho * rsqrt(tLenSq);
      float3 B     = cross(N_geom, T);
      float3x3 TBN = float3x3(T, B, N_geom);
      N            = normalize(TBN * localNormal);
    }
  }
  
  // Sample Roughness (clamped to >= 0.05 so GGX denominator never explodes to infinity)
  float roughness = material.roughness;
  if (material.hasRoughnessMap != 0) {
    roughness *= roughnessTex.sample(texSampler, tiledUV).r;
  }
  roughness = clamp(roughness, 0.05f, 1.0f);
  
  // Sample Metallic [0 = Dielectric (plastic/stone/wood), 1 = Pure Conductor (gold/iron/steel)]
  float metallic = material.metallic;
  if (material.hasMetallicMap != 0) {
    metallic *= metallicTex.sample(texSampler, tiledUV).r;
  }
  metallic = clamp(metallic, 0.0f, 1.0f);
  
  // Sample Ambient Occlusion (darkens crevices and corners)
  float ao = 1.0f;
  if (material.hasAOMap != 0) {
    ao = aoTex.sample(texSampler, tiledUV).r;
  }
  
  // View vector V pointing from surface fragment toward the camera eye
  float3 V  = normalize(uniforms.cameraPosition - in.worldPosition);
  
  // Base reflectivity F0: non-metals reflect 4% (0.04) monochrome; metals tint reflections with their albedo
  float3 F0 = mix(float3(0.04f), albedo, metallic);
  
  /*
   * ------------------------------------------------------------------------
   * 1. Directional Sun/Moon Light + 3x3 PCF Shadow Mapping
   * ------------------------------------------------------------------------
   */
  float3 L = normalize(-uniforms.lightDirection); // Vector pointing toward the sun
  float3 H = normalize(V + L);                    // Halfway vector between View and Light
  
  // Evaluate Cook-Torrance BRDF terms for the Sun
  float  NDF = DistributionGGX(N, H, roughness);
  float  G   = GeometrySmith(N, V, L, roughness);
  float3 F   = FresnelSchlick(max(dot(H, V), 0.0f), F0);
  
  float3 numerator   = NDF * G * F;
  float  denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 1e-4f;
  float3 specular    = min(numerator / denominator, float3(1.5f)); // Clamp extreme specular fireflies
  
  // Energy Conservation: Diffuse light (kD) is whatever energy was not reflected as Specular (kS = F).
  // Pure metals (metallic = 1.0) have zero diffuse scattering (kD = 0).
  float3 kS    = F;
  float3 kD    = (float3(1.0f) - kS) * (1.0f - metallic);
  float  NdotL = max(dot(N, L), 0.0f);
  
  // Perspective divide to convert light-space position to Normalized Device Coordinates (NDC)
  float  shadowFactor = 1.0f;
  float3 projCoords   = in.shadowPosition.xyz / in.shadowPosition.w;
  
  // Remap X and Y from NDC [-1, 1] to Metal texture UV coordinates [0, 1].
  // Note: Y is negated (-0.5f) because Metal texture origin (0,0) is Top-Left, whereas NDC +Y is Up.
  projCoords.xy = projCoords.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
  
  // Check if fragment lies inside the shadow map frustum (with a 1% inset margin for 3x3 PCF taps)
  if (projCoords.x >= 0.01f && projCoords.x <= 0.99f &&
      projCoords.y >= 0.01f && projCoords.y <= 0.99f &&
      projCoords.z >= 0.0f  && projCoords.z <= 1.0f) {
    
    float currentDepth = projCoords.z;
    // Slope-scaled depth bias: steeper surfaces relative to the sun get slightly more bias
    float bias      = max(0.0020f * (1.0f - max(dot(N_geom, L), 0.0f)), 0.0008f);
    float shadowSum = 0.0f;
    float2 texelSize = 1.0f / float2(shadowMap.get_width(), shadowMap.get_height());
    
    // 3x3 Percentage-Closer Filtering (PCF): average 9 depth comparisons for soft shadow penumbras
    for (int x = -1; x <= 1; ++x) {
      for (int y = -1; y <= 1; ++y) {
        float pcfDepth = shadowMap.sample(shadowSampler, projCoords.xy + float2(x, y) * texelSize);
        shadowSum += (currentDepth - bias > pcfDepth) ? 0.0f : 1.0f;
      }
    }
    float rawShadow = shadowSum / 9.0f;
    
    // Smoothly fade shadow out over the outer 8% border of the ortho box so no hard box edge is visible
    float2 edgeDist = abs(projCoords.xy * 2.0f - 1.0f);
    float  edgeFade = smoothstep(0.92f, 1.0f, max(edgeDist.x, edgeDist.y));
    shadowFactor    = mix(rawShadow, 1.0f, edgeFade);
  } else {
    // Outside shadow frustum: smoothly blend to 1.0 (fully lit)
    float2 edgeDist = abs(projCoords.xy * 2.0f - 1.0f);
    float  outFade  = smoothstep(1.0f, 1.05f, max(edgeDist.x, edgeDist.y));
    shadowFactor    = mix(0.0f, 1.0f, outFade);
  }
  
  float3 radiance        = uniforms.directionalColor.rgb;
  float  effectiveShadow = mix(0.12f, 1.0f, shadowFactor); // Keep a subtle 12% bounce light floor in deep shadows
  float3 directLighting  = (kD * albedo / PI + specular) * radiance * NdotL * effectiveShadow;
  
  /*
   * ------------------------------------------------------------------------
   * 2. Dynamic Point Lights (Wall Sconces, Ceiling Lights & Street Lamps)
   * ------------------------------------------------------------------------
   */
  float3 pointLighting = float3(0.0f);
  uint   lightCount    = min(uniforms.pointLightCount, kMaxPointLights);
  
  for (uint i = 0; i < lightCount; ++i) {
    float3 lightPos = uniforms.pointLights[i].positionAndRadius.xyz;
    float  radius   = max(uniforms.pointLights[i].positionAndRadius.w, 1.0f);
    float3 lColor   = uniforms.pointLights[i].colorAndIntensity.rgb;
    float  lInten   = clamp(uniforms.pointLights[i].colorAndIntensity.w, 0.0f, 12.0f);
    float3 atten    = max(uniforms.pointLights[i].attenuation.xyz, float3(1.0f, 0.22f, 0.045f));
    
    float3 toLight  = lightPos - in.worldPosition;
    float  distSq   = dot(toLight, toLight);
    float  radiusSq = radius * radius;
    
    // Early-out branch: skip BRDF evaluation if fragment is outside the point light's cutoff sphere
    if (distSq < radiusSq) {
      float  dist = sqrt(max(distSq, 1e-4f));
      float3 Lp   = toLight / dist;
      float3 Hp   = normalize(V + Lp);
      
      // Blend perturbed normal with geometric normal so normal maps don't create harsh black self-shadow seams
      float NdotL_geom = dot(N_geom, Lp);
      float NdotL_pert = dot(N, Lp);
      float NdotL_p    = max(mix(NdotL_pert, NdotL_geom, 0.35f), 0.0f);
      
      if (NdotL_p > 0.0f) {
        // Unreal/Frostbite smooth windowing function: forces attenuation to hit 0.0 smoothly at dist == radius
        float distRatio2 = distSq / radiusSq;
        float window     = saturate(1.0f - distRatio2 * distRatio2);
        window          *= window;
        
        // Physical Constant-Linear-Quadratic distance falloff
        float denomAtten  = atten.x + atten.y * dist + atten.z * distSq;
        float attenuation = (lInten * window) / denomAtten;
        
        // Point light Cook-Torrance BRDF
        float  NDF_p  = DistributionGGX(N, Hp, roughness);
        float  G_p    = GeometrySmith(N, V, Lp, roughness);
        float3 F_p    = FresnelSchlick(max(dot(Hp, V), 0.0f), F0);
        
        float3 spec_p = min((NDF_p * G_p * F_p) /
                            (4.0f * max(dot(N, V), 0.0f) * max(NdotL_p, 0.1f) + 1e-4f),
                            float3(0.8f));
        float3 kD_p   = (float3(1.0f) - F_p) * (1.0f - metallic);
        
        pointLighting += (kD_p * albedo / PI + spec_p) * lColor * attenuation * NdotL_p;
      }
    }
  }
  pointLighting = min(pointLighting, float3(1.05f)); // Prevent overlapping sconces from blowing out white
  
  /*
   * ------------------------------------------------------------------------
   * 3. Horizon-Stabilized Image-Based Lighting (Diffuse & Specular IBL)
   * ------------------------------------------------------------------------
   */
  constexpr sampler cubeSampler(coord::normalized, filter::linear, address::clamp_to_edge);
  
  // Diffuse IBL: sample pre-convolved irradiance cubemap using inverted Y to match Metal cubemap coordinates
  float3 sampleN       = normalize(mix(N, N_geom, 0.70f));
  sampleN.y            = -sampleN.y;
  float3 rawIrradiance = irradianceMap.sample(cubeSampler, sampleN).rgb;
  rawIrradiance        = rawIrradiance / (rawIrradiance + 1.0f); // Reinhard compression
  
  float  skyOcclusion = mix(0.25f, 1.0f, shadowFactor);
  float3 ambientTerm  = mix(uniforms.ambientColor.rgb, rawIrradiance * uniforms.ambientColor.rgb * 2.2f, 0.5f) * skyOcclusion;
  float3 diffuseIBL   = kD * albedo * ambientTerm * ao;
  
  // Specular IBL: reflect view ray about surface normal and sample mipmapped environment cubemap.
  // Smooth surfaces (roughness -> 0) sample sharp Mip 0; rough surfaces sample blurry higher mips.
  float3 N_refl  = normalize(mix(N, N_geom, 0.75f));
  float3 R       = reflect(-V, N_refl);
  float3 sampleR = R;
  sampleR.y      = -abs(R.y) - 0.05f; // Stabilize horizon lookup to prevent a moving seam line on flat ground
  sampleR        = normalize(sampleR);
  
  float  maxMipLevel = float(max(envMap.get_num_mip_levels(), 1u) - 1u);
  float  sampleMip   = roughness * maxMipLevel;
  float3 envColor    = envMap.sample(cubeSampler, sampleR, level(sampleMip)).rgb;
  envColor           = (envColor / (envColor + 1.0f)) * clamp(length(uniforms.ambientColor.rgb), 0.05f, 0.55f) * skyOcclusion;
  
  // Roughness-aware Fresnel for IBL: prevents rough stone/grass from having mirror-like grazing edges
  float  NdotV       = max(dot(N_geom, V), 0.0f);
  float3 F_ibl       = F0 + (max(float3(1.0f - roughness), F0) - F0) * pow(clamp(1.0f - NdotV, 0.0f, 1.0f), 5.0f);
  float3 specularIBL = envColor * F_ibl * (1.0f - roughness) * ao;
  
  /*
   * ------------------------------------------------------------------------
   * 4. Final Radiance Accumulation & Alpha Output
   * ------------------------------------------------------------------------
   */
  float3 color = directLighting + pointLighting + diffuseIBL + specularIBL + material.emissiveColor.rgb;
  
  float finalAlpha = material.baseColor.a;
  if (material.hasAlbedoMap != 0) {
    finalAlpha = albedoTex.sample(texSampler, tiledUV).a * material.baseColor.a;
  }
  
  // Output linear HDR RGBA16Float color (Tonemapping + Compute Bloom will resolve this in EndScene)
  return float4(color, finalAlpha);
}
