/*!
 * @file ChamferBox.cpp
 * @brief Implementation of chamfered box procedural geometry synthesis with uniform white vertex colors.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/Procedural/ChamferBox.hpp"
#include <cmath>

namespace KanVerse {
  
  MeshData ChamferBoxGenerator::Generate(float size, float chamfer, uint32_t segments) {
    MeshData data;
    
    struct FaceDef {
      simd::float3 normal;
      simd::float3 u;
      simd::float3 v;
      simd::float4 color;
    };
    
    // Set all face colors to neutral white to prevent unwanted color tinting on edges
    const FaceDef faces[6] = {
      // Normal                // u                   // v                   // Color
      { { 0.0f,  0.0f,  1.0f}, { 1.0f,  0.0f,  0.0f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} }, // +Z Front
      { { 0.0f,  0.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} }, // -Z Back
      { { 1.0f,  0.0f,  0.0f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} }, // +X Right
      { {-1.0f,  0.0f,  0.0f}, { 0.0f,  0.0f,  1.0f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} }, // -X Left
      { { 0.0f,  1.0f,  0.0f}, { 1.0f,  0.0f,  0.0f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} }, // +Y Top
      { { 0.0f, -1.0f,  0.0f}, { 1.0f,  0.0f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} }  // -Y Bottom
    };
    
    const float inner = size - chamfer;
    
    // 1. Generate 6 Main Faces
    for (uint32_t f = 0; f < 6; ++f) {
      const uint16_t startIndex = static_cast<uint16_t>(data.vertices.size());
      const auto& face = faces[f];
      
      const simd::float3 center = face.normal * size;
      const simd::float3 p0 = center - face.u * inner - face.v * inner;
      const simd::float3 p1 = center + face.u * inner - face.v * inner;
      const simd::float3 p2 = center + face.u * inner + face.v * inner;
      const simd::float3 p3 = center - face.u * inner + face.v * inner;
      
      data.vertices.push_back({ p0, face.normal, face.color, {0.0f, 0.0f}, {0.0f, 0.0f} });
      data.vertices.push_back({ p1, face.normal, face.color, {1.0f, 0.0f}, {0.0f, 0.0f} });
      data.vertices.push_back({ p2, face.normal, face.color, {1.0f, 1.0f}, {0.0f, 0.0f} });
      data.vertices.push_back({ p3, face.normal, face.color, {0.0f, 1.0f}, {0.0f, 0.0f} });
      
      data.indices.push_back(startIndex + 0);
      data.indices.push_back(startIndex + 1);
      data.indices.push_back(startIndex + 2);
      data.indices.push_back(startIndex + 2);
      data.indices.push_back(startIndex + 3);
      data.indices.push_back(startIndex + 0);
    }
    
    // 2. Generate 12 Curved Bevel Edges
    struct EdgeDef {
      simd::float3 dir;
      simd::float3 nA;
      simd::float3 nB;
      simd::float4 color;
    };
    
    // Set all edge bevel colors to neutral white as well
    const EdgeDef edges[12] = {
      // Edges parallel to Z
      { {0, 0, 1}, { 1, 0, 0}, { 0, 1, 0}, {1.0f, 1.0f, 1.0f, 1.0f} },
      { {0, 0, 1}, { 0, 1, 0}, {-1, 0, 0}, {1.0f, 1.0f, 1.0f, 1.0f} },
      { {0, 0, 1}, {-1, 0, 0}, { 0,-1, 0}, {1.0f, 1.0f, 1.0f, 1.0f} },
      { {0, 0, 1}, { 0,-1, 0}, { 1, 0, 0}, {1.0f, 1.0f, 1.0f, 1.0f} },
      // Edges parallel to X
      { {1, 0, 0}, { 0, 1, 0}, { 0, 0, 1}, {1.0f, 1.0f, 1.0f, 1.0f} },
      { {1, 0, 0}, { 0, 0, 1}, { 0,-1, 0}, {1.0f, 1.0f, 1.0f, 1.0f} },
      { {1, 0, 0}, { 0,-1, 0}, { 0, 0,-1}, {1.0f, 1.0f, 1.0f, 1.0f} },
      { {1, 0, 0}, { 0, 0,-1}, { 0, 1, 0}, {1.0f, 1.0f, 1.0f, 1.0f} },
      // Edges parallel to Y
      { {0, 1, 0}, { 0, 0, 1}, { 1, 0, 0}, {1.0f, 1.0f, 1.0f, 1.0f} },
      { {0, 1, 0}, { 1, 0, 0}, { 0, 0,-1}, {1.0f, 1.0f, 1.0f, 1.0f} },
      { {0, 1, 0}, { 0, 0,-1}, {-1, 0, 0}, {1.0f, 1.0f, 1.0f, 1.0f} },
      { {0, 1, 0}, {-1, 0, 0}, { 0, 0, 1}, {1.0f, 1.0f, 1.0f, 1.0f} }
    };
    
    for (uint32_t e = 0; e < 12; ++e) {
      const auto& edge = edges[e];
      const simd::float3 center = (edge.nA + edge.nB) * inner;
      
      for (uint32_t s = 0; s < segments; ++s) {
        const float t0 = static_cast<float>(s) / static_cast<float>(segments);
        const float t1 = static_cast<float>(s + 1) / static_cast<float>(segments);
        const float angle0 = t0 * (M_PI * 0.5f);
        const float angle1 = t1 * (M_PI * 0.5f);
        
        const simd::float3 norm0 = simd_normalize(edge.nA * std::cos(angle0) + edge.nB * std::sin(angle0));
        const simd::float3 norm1 = simd_normalize(edge.nA * std::cos(angle1) + edge.nB * std::sin(angle1));
        
        const simd::float3 p0 = center + norm0 * chamfer - edge.dir * inner;
        const simd::float3 p1 = center + norm1 * chamfer - edge.dir * inner;
        const simd::float3 p2 = center + norm1 * chamfer + edge.dir * inner;
        const simd::float3 p3 = center + norm0 * chamfer + edge.dir * inner;
        
        const uint16_t startIndex = static_cast<uint16_t>(data.vertices.size());
        data.vertices.push_back({ p0, norm0, edge.color, {t0, 0.0f}, {0.0f, 0.0f} });
        data.vertices.push_back({ p1, norm1, edge.color, {t1, 0.0f}, {0.0f, 0.0f} });
        data.vertices.push_back({ p2, norm1, edge.color, {t1, 1.0f}, {0.0f, 0.0f} });
        data.vertices.push_back({ p3, norm0, edge.color, {t0, 1.0f}, {0.0f, 0.0f} });
        
        data.indices.push_back(startIndex + 0);
        data.indices.push_back(startIndex + 1);
        data.indices.push_back(startIndex + 2);
        data.indices.push_back(startIndex + 2);
        data.indices.push_back(startIndex + 3);
        data.indices.push_back(startIndex + 0);
      }
    }
    
    return data;
  }
  
} // namespace KanVerse
