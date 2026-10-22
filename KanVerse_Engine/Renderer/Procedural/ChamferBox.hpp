/*!
 * @file ChamferBox.hpp
 * @brief Procedural geometry synthesizer for chamfered box meshes with UV coordinates.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include <simd/simd.h>
#include <vector>
#include <cstdint>

namespace KanVerse {
  
  /*!
   * @struct Vertex
   * @brief Interleaved 64-byte vertex format strictly aligned for Metal SIMD pipelines.
   */
  struct alignas(16) Vertex {
    simd::float3 position;  ///< Local 3D vertex position (offset 0, 16 bytes)
    simd::float3 normal;    ///< Surface normal vector (offset 16, 16 bytes)
    simd::float4 color;     ///< Per-vertex diffuse color (offset 32, 16 bytes)
    simd::float2 texCoord;  ///< Texture coordinates (offset 48, 8 bytes)
    simd::float2 padding;   ///< Alignment padding to enforce 64-byte stride (offset 56, 8 bytes)
  };
  
  /*!
   * @struct MeshData
   * @brief CPU geometry buffer payload holding vertices and element indices.
   */
  struct MeshData {
    std::vector<Vertex>   vertices;
    std::vector<uint16_t> indices;
  };
  
  /*!
   * @class ChamferBoxGenerator
   * @brief Synthesizes chamfered cube geometries with smooth corner normals and UV coordinates.
   */
  class ChamferBoxGenerator {
  public:
    /*!
     * @brief Synthesizes a chamfered box mesh with full bevels and face UVs.
     * @param size Half-extent dimension.
     * @param chamfer Radius of the beveled corner curves.
     * @param segments Tessellation resolution along curved edges.
     * @return Populated MeshData container.
     */
    static MeshData Generate(float size = 0.5f, float chamfer = 0.08f, uint32_t segments = 3);
  };
  
} // namespace KanVerse
