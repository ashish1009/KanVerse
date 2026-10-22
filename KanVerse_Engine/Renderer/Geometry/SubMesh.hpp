/*!
 * @file Submesh.hpp
 * @brief Discrete submesh draw-call descriptors and local bounds definition.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Core/Math/Math.hpp"
#include <cstdint>
#include <string>

namespace KanVerse {
  
  /*!
   * @struct AABB
   * @brief Axis-aligned bounding box defined by minimum and maximum extents in 3D space.
   */
  struct AABB {
    Math::Vec3 Min{ 1e30f,  1e30f,  1e30f};
    Math::Vec3 Max{-1e30f, -1e30f, -1e30f};
    
    /*!
     * @brief Expands bounding extents to encapsulate an arbitrary point.
     * @param point 3D coordinates.
     */
    void Encapsulate(const Math::Vec3& point) {
      Min = Math::MakeVec3(
                           std::min(Min.x, point.x),
                           std::min(Min.y, point.y),
                           std::min(Min.z, point.z)
                           );
      Max = Math::MakeVec3(
                           std::max(Max.x, point.x),
                           std::max(Max.y, point.y),
                           std::max(Max.z, point.z)
                           );
    }
  };
  
  /*!
   * @struct Submesh
   * @brief Partitioned primitive geometry metadata mapping to a dedicated draw-call.
   */
  struct Submesh {
    std::string NodeName{"Submesh"};      ///< Semantic diagnostic identifier.
    uint32_t    BaseVertex{0};             ///< Starting vertex offset in primary vertex stream.
    uint32_t    BaseIndex{0};              ///< Starting byte offset in primary index stream.
    uint32_t    IndexCount{0};             ///< Number of primitive indices.
    uint32_t    MaterialIndex{0};          ///< Target surface material index.
    Math::Mat4  LocalTransform{Math::Identity()}; ///< Hierarchical transformation.
    AABB        BoundingBox;               ///< Local object space extents.
  };
  
} // namespace KanVerse
