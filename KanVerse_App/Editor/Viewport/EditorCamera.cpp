/*!
 * @file EditorCamera.cpp
 * @brief Implementation of the KanVerse Studio 3D EditorCamera.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Editor/Viewport/EditorCamera.hpp"
#include "Platform/Input/GameControllerBridge.hpp"
#include "Platform/Input/KeyCodes.hpp"
#include <algorithm>
#include <cmath>

namespace KanVerse {
  
  /*
   * EditorCamera Implementation Notes:
   * ----------------------------------
   * Supports two complementary navigation paradigms:
   * 1. Orbit/Pan/Zoom: Left-click or Right-click drag orbits around `m_FocalPoint`,
   *    Shift + Drag pans `m_FocalPoint`, and Scroll Wheel zooms radially.
   * 2. WASD Fly-through: Holding Right-Click + W/A/S/D/Q/E translates `m_FocalPoint`
   *    along the camera's basis vectors for rapid level traversal.
   */
  
  EditorCamera::EditorCamera(float fovDegrees, float aspectRatio, float nearClip, float farClip)
  : m_Camera(fovDegrees * (static_cast<float>(M_PI) / 180.0f), aspectRatio, nearClip, farClip) {
    UpdateView();
  }
  
  void EditorCamera::SetViewportSize(uint32_t width, uint32_t height) {
    if (width == 0 || height == 0) return;
    m_ViewportWidth  = width;
    m_ViewportHeight = height;
    m_Camera.SetViewportSize(width, height);
  }
  
  Math::Vec3 EditorCamera::GetForwardDirection() const noexcept {
    const float cosPitch = std::cos(m_Pitch);
    return simd::normalize(Math::MakeVec3(
                                          -std::sin(m_Yaw) * cosPitch,
                                          -std::sin(m_Pitch),
                                          -std::cos(m_Yaw) * cosPitch
                                          ));
  }
  
  Math::Vec3 EditorCamera::GetRightDirection() const noexcept {
    return simd::normalize(simd::cross(GetForwardDirection(), Math::MakeVec3(0.0f, 1.0f, 0.0f)));
  }
  
  Math::Vec3 EditorCamera::GetUpDirection() const noexcept {
    return simd::normalize(simd::cross(GetRightDirection(), GetForwardDirection()));
  }
  
  void EditorCamera::UpdateView() {
    const Math::Vec3 forward = GetForwardDirection();
    const Math::Vec3 position = m_FocalPoint - forward * m_Distance;
    m_Camera.SetPosition(position);
    m_Camera.SetTarget(m_FocalPoint);
  }
  
  void EditorCamera::OnUpdate(TimeStep ts, bool viewportFocused) {
    if (!viewportFocused) {
      return;
    }
    
    const float dt = ts.GetSeconds();
    const float moveSpeed = InputBridge::IsKeyPressed(static_cast<uint16_t>(Key::Shift)) ? 18.0f : 8.0f;
    
    bool moved = false;
    if (InputBridge::IsKeyPressed(static_cast<uint16_t>(Key::W))) {
      m_FocalPoint += GetForwardDirection() * (moveSpeed * dt);
      moved = true;
    }
    if (InputBridge::IsKeyPressed(static_cast<uint16_t>(Key::S))) {
      m_FocalPoint -= GetForwardDirection() * (moveSpeed * dt);
      moved = true;
    }
    if (InputBridge::IsKeyPressed(static_cast<uint16_t>(Key::D))) {
      m_FocalPoint += GetRightDirection() * (moveSpeed * dt);
      moved = true;
    }
    if (InputBridge::IsKeyPressed(static_cast<uint16_t>(Key::A))) {
      m_FocalPoint -= GetRightDirection() * (moveSpeed * dt);
      moved = true;
    }
    if (InputBridge::IsKeyPressed(static_cast<uint16_t>(Key::E))) {
      m_FocalPoint.y += moveSpeed * dt;
      moved = true;
    }
    if (InputBridge::IsKeyPressed(static_cast<uint16_t>(Key::Q))) {
      m_FocalPoint.y -= moveSpeed * dt;
      moved = true;
    }
    
    if (moved) {
      UpdateView();
    }
  }
  
  void EditorCamera::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& event) { return OnMouseScroll(event); });
    dispatcher.Dispatch<MouseMovedEvent>([this](MouseMovedEvent& event) { return OnMouseMoved(event); });
    dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& event) { return OnMouseButtonPressed(event); });
    dispatcher.Dispatch<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent& event) { return OnMouseButtonReleased(event); });
  }
  
  bool EditorCamera::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
    if (e.GetMouseButton() == 1) { // Right-click: Orbit / Look
      m_IsOrbiting = true;
      m_FirstClick = true;
    } else if (e.GetMouseButton() == 0 && InputBridge::IsKeyPressed(static_cast<uint16_t>(Key::Shift))) {
      m_IsPanning  = true;
      m_FirstClick = true;
    }
    return false;
  }
  
  bool EditorCamera::OnMouseButtonReleased(MouseButtonReleasedEvent& e) {
    if (e.GetMouseButton() == 1) {
      m_IsOrbiting = false;
      m_FirstClick = true;
    } else if (e.GetMouseButton() == 0) {
      m_IsPanning  = false;
      m_FirstClick = true;
    }
    return false;
  }
  
  bool EditorCamera::OnMouseMoved(MouseMovedEvent& e) {
    const Math::Vec2 mousePos = Math::MakeVec2(e.GetX(), e.GetY());
    if (!m_IsOrbiting && !m_IsPanning) {
      m_InitialMousePosition = mousePos;
      return false;
    }
    
    if (m_FirstClick) {
      m_InitialMousePosition = mousePos;
      m_FirstClick = false;
      return false;
    }
    
    const Math::Vec2 delta = (mousePos - m_InitialMousePosition) * 0.004f;
    m_InitialMousePosition = mousePos;
    
    if (m_IsPanning) {
      MousePan(delta);
    } else if (m_IsOrbiting) {
      MouseRotate(delta);
    }
    
    UpdateView();
    return false;
  }
  
  bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e) {
    const float delta = std::clamp(e.GetYOffset(), -20.0f, 20.0f) * 0.06f;
    MouseZoom(delta);
    UpdateView();
    return false;
  }
  
  void EditorCamera::MousePan(const Math::Vec2& delta) {
    const float panSpeed = std::max(m_Distance * 0.6f, 1.0f);
    m_FocalPoint -= GetRightDirection() * (delta.x * panSpeed);
    m_FocalPoint -= GetUpDirection() * (delta.y * panSpeed);
  }
  
  void EditorCamera::MouseRotate(const Math::Vec2& delta) {
    m_Yaw   -= delta.x * 1.2f;
    m_Pitch += delta.y * 1.2f;
    m_Pitch  = std::clamp(m_Pitch, -1.48f, 1.48f);
  }
  
  void EditorCamera::MouseZoom(float delta) {
    const float zoomSpeed = std::max(m_Distance * 0.08f, 0.35f);
    m_Distance = std::clamp(m_Distance - delta * zoomSpeed, 1.0f, 150.0f);
  }
  
} // namespace KanVerse
