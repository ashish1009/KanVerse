/*!
 * @file Camera.hpp
 * @brief Base camera contract holding primary projection properties.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Core/Math/Math.hpp"

namespace KanVerse {
  
  /*!
   * @class Camera
   * @brief Abstract foundation representing an optical projection matrix.
   */
  class Camera {
  public:
    virtual ~Camera() = default;
    
    Camera() = default;
    explicit Camera(const Math::Mat4& projection) : m_Projection(projection) {}
    
    /*!
     * @brief Queries the computed projection matrix.
     * @return Const reference to internal Mat4.
     */
    [[nodiscard]] const Math::Mat4& GetProjection() const noexcept { return m_Projection; }
    
  protected:
    Math::Mat4 m_Projection{Math::Identity()};
  };
  
} // namespace KanVerse
