/*!
 * @file Capsule.hpp
 * @brief Procedural UV capsule mesh generator for character bodies.
 * @details Synthesizes a capped cylindrical hull along the vertical Y-axis
 *          with configurable radius, central cylinder height, and radial/vertical rings.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Renderer/Geometry/Mesh.hpp"
#include "Renderer/Buffers/BufferLayout.hpp"

#include <memory>

namespace KanVerse {
  
  /*!
   * @class Capsule
   * @brief Procedural generator for capsule geometry with smooth normals and UV coordinates.
   */
  class Capsule {
  public:
    /*!
     * @brief Generates a procedural capsule mesh aligned along the local Y-axis.
     * @param radius Hemisphere and cylinder radius in world units.
     * @param cylinderHeight Height of the cylindrical center section excluding hemisphere caps.
     * @param radialSegments Number of subdivisions around the azimuth perimeter.
     * @param capRings Number of latitude rings per hemisphere cap.
     * @param layout Vertex buffer layout specifying interleaved attributes.
     * @return Shared pointer to the constructed GPU Mesh instance.
     */
    [[nodiscard]] static std::shared_ptr<Mesh> Create(
                                                      float radius,
                                                      float cylinderHeight,
                                                      uint32_t radialSegments,
                                                      uint32_t capRings,
                                                      const BufferLayout& layout
                                                      );
  };
  
} // namespace KanVerse
