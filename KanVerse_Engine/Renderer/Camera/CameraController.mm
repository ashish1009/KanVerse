/*!
 * @file CameraController.mm
 * @brief Implementation of interactive orbit camera controller with non-linear screen shake trauma.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/Camera/CameraController.hpp"

#include <cmath>
#include <algorithm>

namespace KanVerse {
  
  CameraController::CameraController(float fovDegrees, float aspectRatio)
  : m_Camera(fovDegrees * (static_cast<float>(M_PI) / 180.0f), aspectRatio, 0.1f, 1000.0f),
  m_FocalTarget{0.0f, 0.0f, 0.0f},
  m_Distance(32.0f),
  m_Pitch(0.42f),
  m_Yaw(0.0f),
  m_Trauma(0.0f),
  m_ShakeTime(0.0f),
  m_FirstClick(true) {
    UpdateCameraPosition();
  }
  
  void CameraController::OnUpdate(TimeStep ts) {
    const float dt = ts.GetSeconds();
    m_ShakeTime += dt * 45.0f; // High-frequency oscillation step for snappy micro-jitter
    
    // Decay trauma non-linearly over time
    if (m_Trauma > 0.0f) {
      m_Trauma = std::max(0.0f, m_Trauma - 1.6f * dt);
      UpdateCameraPosition();
    }
  }
  
  void CameraController::SetTarget(const simd::float3& target) {
    m_FocalTarget = target;
    UpdateCameraPosition();
  }
  
  void CameraController::AddTrauma(float amount) noexcept {
    // Accumulate trauma and clamp to normalized ceiling
    m_Trauma = std::clamp(m_Trauma + amount, 0.0f, 1.0f);
  }
  
  void CameraController::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) {
      return OnWindowResized(event);
    });
    dispatcher.Dispatch<MouseMovedEvent>([this](MouseMovedEvent& event) {
      return OnMouseMoved(event);
    });
    dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& event) {
      return OnMouseScrolled(event);
    });
    dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& event) {
      return OnMouseButtonPressed(event);
    });
    dispatcher.Dispatch<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent& event) {
      return OnMouseButtonReleased(event);
    });
  }
  
  bool CameraController::OnWindowResized(WindowResizeEvent& e) {
    m_Camera.SetViewportSize(e.GetWidth(), e.GetHeight());
    return false;
  }
  
  bool CameraController::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
    if (e.GetMouseButton() == 0 || e.GetMouseButton() == 1) {
      m_IsDragging = true;
      m_FirstClick = true; // Prevent delta spike from stale coordinates
    }
    return false;
  }
  
  bool CameraController::OnMouseButtonReleased(MouseButtonReleasedEvent& e) {
    if (e.GetMouseButton() == 0 || e.GetMouseButton() == 1) {
      m_IsDragging = false;
      m_FirstClick = true;
    }
    return false;
  }
  
  bool CameraController::OnMouseMoved(MouseMovedEvent& e) {
    if (!m_IsDragging) {
      m_LastMouseX = e.GetX();
      m_LastMouseY = e.GetY();
      return false;
    }
    
    if (m_FirstClick) {
      m_LastMouseX = e.GetX();
      m_LastMouseY = e.GetY();
      m_FirstClick = false;
      return false;
    }
    
    const float deltaX = e.GetX() - m_LastMouseX;
    const float deltaY = e.GetY() - m_LastMouseY;
    m_LastMouseX = e.GetX();
    m_LastMouseY = e.GetY();
    
    OnMousePan(deltaX, deltaY);
    return false;
  }
  
  bool CameraController::OnMouseScrolled(MouseScrolledEvent& e) {
    const float clampedDelta = std::clamp(e.GetYOffset(), -20.0f, 20.0f);
    OnMouseZoom(clampedDelta * 0.05f);
    return false;
  }
  
  void CameraController::OnMousePan(float deltaX, float deltaY) {
    constexpr float sensitivity = 0.005f;
    m_Yaw   -= deltaX * sensitivity;
    m_Pitch += deltaY * sensitivity;
    
    // Avoid gimbal lock near polar poles
    constexpr float maxPitch = 1.50f;
    constexpr float minPitch = -1.50f;
    m_Pitch = std::clamp(m_Pitch, minPitch, maxPitch);
    
    UpdateCameraPosition();
  }
  
  void CameraController::OnMouseZoom(float deltaZoom) {
    const float zoomSpeed = std::max(m_Distance * 0.05f, 0.35f);
    m_Distance -= deltaZoom * zoomSpeed;
    m_Distance = std::clamp(m_Distance, 2.0f, 75.0f);
    UpdateCameraPosition();
  }
  
  void CameraController::SetDistance(float distance) noexcept {
    m_Distance = std::clamp(distance, 2.0f, 75.0f);
    UpdateCameraPosition();
  }
  
  void CameraController::UpdateCameraPosition() {
    const float cosPitch = std::cos(m_Pitch);
    const float sinPitch = std::sin(m_Pitch);
    const float cosYaw   = std::cos(m_Yaw);
    const float sinYaw   = std::sin(m_Yaw);
    
    // Base orbital position in Cartesian space
    simd::float3 eyePos = m_FocalTarget + simd_make_float3(
                                                           m_Distance * cosPitch * sinYaw,
                                                           m_Distance * sinPitch,
                                                           m_Distance * cosPitch * cosYaw
                                                           );
    
    // Apply quadratic trauma shake for organic, impactful screen rumble
    if (m_Trauma > 1e-4f) {
      const float shakeFactor = m_Trauma * m_Trauma * 0.28f;
      const float offsetX = std::sin(m_ShakeTime * 1.1f) * shakeFactor;
      const float offsetY = std::cos(m_ShakeTime * 1.4f) * shakeFactor;
      const float offsetZ = std::sin(m_ShakeTime * 1.7f) * shakeFactor;
      
      eyePos += simd_make_float3(offsetX, offsetY, offsetZ);
      m_Camera.SetTarget(m_FocalTarget + simd_make_float3(offsetX * 0.5f, offsetY * 0.5f, offsetZ * 0.5f));
    } else {
      m_Camera.SetTarget(m_FocalTarget);
    }
    
    m_Camera.SetPosition(eyePos);
  }
  
} // namespace KanVerse
