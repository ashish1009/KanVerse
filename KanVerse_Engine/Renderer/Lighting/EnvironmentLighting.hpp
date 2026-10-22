/*!
 * @file EnvironmentLighting.hpp
 * @brief Ambient environmental illumination parameters.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Core/Math/Math.hpp"

namespace KanVerse {
  
  /*!
   * @struct EnvironmentLighting
   * @brief Environmental ambient radiant parameters.
   */
  struct EnvironmentLighting {
    Math::Vec3 AmbientColor{0.18f, 0.20f, 0.24f}; ///< Background ambient tint.
    float      AmbientIntensity{1.0f};             ///< Global ambient scalar.
    
    /*!
     * @brief Computes effective scaled ambient color vector.
     * @return 4D vector formatted for GPU uniform blocks.
     */
    [[nodiscard]] Math::Vec4 GetEffectiveAmbient() const noexcept {
      return Math::MakeVec4(
                            AmbientColor.x * AmbientIntensity,
                            AmbientColor.y * AmbientIntensity,
                            AmbientColor.z * AmbientIntensity,
                            1.0f
                            );
    }
  };
  
} // namespace KanVerse
