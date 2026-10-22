/*!
 * @file PerspectiveCamera.hpp
 * @brief 3D perspective camera computing view and view-projection matrices.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Renderer/Camera/Camera.hpp"

namespace KanVerse {
  
  /*!
   * @class PerspectiveCamera
   * @brief 3D perspective camera supporting positional movement, pitch, yaw, and target tracking.
   */
  class PerspectiveCamera : public Camera {
  public:
    /*!
     * @brief Constructs a perspective camera.
     * @param fovRadians Vertical field of view in radians.
     * @param aspectRatio Viewport width over height.
     * @param nearClip Near clipping distance.
     * @param farClip Far clipping distance.
     */
    PerspectiveCamera(float fovRadians, float aspectRatio, float nearClip = 0.1f, float farClip = 1000.0f);
    
    /*!
     * @brief Updates projection matrix when viewport dimensions mutate.
     * @param width Viewport width in logical points.
     * @param height Viewport height in logical points.
     */
    void SetViewportSize(uint32_t width, uint32_t height);
    
    /*!
     * @brief Sets world position and recalculates view and view-projection transforms.
     * @param position New camera coordinates in world space.
     */
    void SetPosition(const Math::Vec3& position);
    
    /*!
     * @brief Sets focal look-at target and recalculates view matrices.
     * @param target Center point the camera faces.
     */
    void SetTarget(const Math::Vec3& target);
    
    [[nodiscard]] const Math::Vec3& GetPosition() const noexcept { return m_Position; }
    [[nodiscard]] const Math::Vec3& GetTarget() const noexcept { return m_Target; }
    [[nodiscard]] const Math::Mat4& GetViewMatrix() const noexcept { return m_ViewMatrix; }
    [[nodiscard]] const Math::Mat4& GetViewProjectionMatrix() const noexcept { return m_ViewProjectionMatrix; }
    
  private:
    void RecalculateViewMatrix();
    void RecalculateProjectionMatrix();
    
    float      m_FovRadians;
    float      m_AspectRatio;
    float      m_NearClip;
    float      m_FarClip;
    
    Math::Vec3 m_Position{0.0f, 0.0f, 3.0f};
    Math::Vec3 m_Target{0.0f, 0.0f, 0.0f};
    Math::Vec3 m_Up{0.0f, 1.0f, 0.0f};
    
    Math::Mat4 m_ViewMatrix{Math::Identity()};
    Math::Mat4 m_ViewProjectionMatrix{Math::Identity()};
  };
  
} // namespace KanVerse
