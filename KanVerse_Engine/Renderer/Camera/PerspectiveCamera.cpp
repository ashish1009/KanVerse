/*!
 * @file PerspectiveCamera.cpp
 * @brief Implementation of the 3D perspective camera matrix calculations.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/Camera/PerspectiveCamera.hpp"

namespace KanVerse {
  
  PerspectiveCamera::PerspectiveCamera(float fovRadians, float aspectRatio, float nearClip, float farClip)
  : m_FovRadians(fovRadians),
  m_AspectRatio(aspectRatio),
  m_NearClip(nearClip),
  m_FarClip(farClip) {
    RecalculateProjectionMatrix();
    RecalculateViewMatrix();
  }
  
  void PerspectiveCamera::SetViewportSize(uint32_t width, uint32_t height) {
    if (width == 0 || height == 0) return;
    m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
    RecalculateProjectionMatrix();
  }
  
  void PerspectiveCamera::SetPosition(const Math::Vec3& position) {
    m_Position = position;
    RecalculateViewMatrix();
  }
  
  void PerspectiveCamera::SetTarget(const Math::Vec3& target) {
    m_Target = target;
    RecalculateViewMatrix();
  }
  
  void PerspectiveCamera::RecalculateProjectionMatrix() {
    m_Projection = Math::Perspective(m_FovRadians, m_AspectRatio, m_NearClip, m_FarClip);
    m_ViewProjectionMatrix = Math::Multiply(m_Projection, m_ViewMatrix);
  }
  
  void PerspectiveCamera::RecalculateViewMatrix() {
    m_ViewMatrix = Math::LookAt(m_Position, m_Target, m_Up);
    m_ViewProjectionMatrix = Math::Multiply(m_Projection, m_ViewMatrix);
  }
  
} // namespace KanVerse

