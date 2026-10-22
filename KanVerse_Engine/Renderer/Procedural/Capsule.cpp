/*!
 * @file Capsule.cpp
 * @brief Implementation of procedural capsule mesh generator with CCW winding.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/Procedural/Capsule.hpp"
#include "Core/Diagnostics/Assert.hpp"
#include <cmath>
#include <vector>

namespace KanVerse {
  std::shared_ptr<Mesh> Capsule::Create(
                                        float radius,
                                        float cylinderHeight,
                                        uint32_t radialSegments,
                                        uint32_t capRings,
                                        const BufferLayout& layout
                                        ) {
    KV_CORE_ASSERT(radius > 0.0f, "Capsule radius must be greater than zero.");
    KV_CORE_ASSERT(cylinderHeight >= 0.0f, "Capsule cylinder height cannot be negative.");
    KV_CORE_ASSERT(radialSegments >= 3, "Radial segments must be at least 3.");
    KV_CORE_ASSERT(capRings >= 1, "Cap rings must be at least 1.");
    
    const float halfHeight = cylinderHeight * 0.5f;
    const bool hasCylinder = cylinderHeight > 0.001f;
    
    // If no cylinder, it's just two hemispheres (a sphere).
    // If cylinder exists, we add 1 extra ring to duplicate the equator at the bottom.
    const uint32_t totalRings = (capRings * 2) + (hasCylinder ? 1 : 0);
    
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    
    const size_t estimatedVertices = (totalRings + 1) * (radialSegments + 1);
    vertices.reserve(estimatedVertices);
    
    for (uint32_t ring = 0; ring <= totalRings; ++ring) {
      float phi = 0.0f;
      float yOffset = 0.0f;
      
      if (ring < capRings) {
        // Top hemisphere
        const float t = static_cast<float>(ring) / static_cast<float>(capRings);
        phi = t * (static_cast<float>(M_PI) * 0.5f);
        yOffset = halfHeight;
      } else if (ring == capRings) {
        // Equator (Top)
        phi = static_cast<float>(M_PI) * 0.5f;
        yOffset = halfHeight;
      } else if (hasCylinder && ring == capRings + 1) {
        // Equator (Bottom) - Only generated if we actually have a cylinder wall
        phi = static_cast<float>(M_PI) * 0.5f;
        yOffset = -halfHeight;
      } else {
        // Bottom hemisphere
        // Adjust the index so `t` sweeps smoothly from 0 to 1 regardless of the cylinder
        const uint32_t bottomRing = hasCylinder ? (ring - (capRings + 1)) : (ring - capRings);
        const float t = static_cast<float>(bottomRing) / static_cast<float>(capRings);
        phi = (static_cast<float>(M_PI) * 0.5f) + (t * static_cast<float>(M_PI) * 0.5f);
        yOffset = -halfHeight;
      }
      
      const float cosPhi = std::cos(phi);
      const float sinPhi = std::sin(phi);
      
      for (uint32_t seg = 0; seg <= radialSegments; ++seg) {
        const float theta = static_cast<float>(seg) * (2.0f * static_cast<float>(M_PI) / static_cast<float>(radialSegments));
        const float cosTheta = std::cos(theta);
        const float sinTheta = std::sin(theta);
        
        const float nx = sinPhi * cosTheta;
        const float ny = cosPhi;
        const float nz = sinPhi * sinTheta;
        
        const float px = radius * nx;
        const float py = radius * ny + yOffset;
        const float pz = radius * nz;
        
        const float u = static_cast<float>(seg) / static_cast<float>(radialSegments);
        const float v = static_cast<float>(ring) / static_cast<float>(totalRings);
        
        Vertex vertex{};
        vertex.position = simd_make_float3(px, py, pz);
        vertex.normal   = simd_make_float3(nx, ny, nz);
        vertex.color    = simd_make_float4(1.0f, 1.0f, 1.0f, 1.0f); // Pure white prevents PBR tinting
        vertex.texCoord = simd_make_float2(u, v);
        vertex.padding  = simd_make_float2(0.0f, 0.0f);
        vertices.push_back(vertex);
      }
    }
    
    for (uint32_t ring = 0; ring < totalRings; ++ring) {
      for (uint32_t seg = 0; seg < radialSegments; ++seg) {
        const uint16_t current = static_cast<uint16_t>(ring * (radialSegments + 1) + seg);
        const uint16_t next    = static_cast<uint16_t>(current + radialSegments + 1);
        
        indices.push_back(current);
        indices.push_back(current + 1);
        indices.push_back(next);
        
        indices.push_back(current + 1);
        indices.push_back(next + 1);
        indices.push_back(next);
      }
    }
    
    return std::make_shared<Mesh>(
                                  vertices.data(),
                                  vertices.size(),
                                  indices.data(),
                                  static_cast<uint32_t>(indices.size()),
                                  layout
                                  );
  }
}
