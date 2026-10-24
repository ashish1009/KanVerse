/*!
 * @file EditorCamera.hpp
 * @brief Free-fly and Maya/Blender-style orbit/pan/zoom camera for the KanVerse Studio 3D viewport.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Renderer/Camera/PerspectiveCamera.hpp"
#include "Core/Time/TimeStep.hpp"
#include "Platform/Events/Event.hpp"
#include "Platform/Events/MouseEvents.hpp"

namespace KanVerse {
  
  /*!
   * @class EditorCamera
   * @brief Interactive 3D viewport camera supporting WASD fly-through and orbital manipulation.
   */
  class EditorCamera {
  public:
    /*!
     * @brief Constructs an EditorCamera with specified vertical FOV and aspect ratio.
     * @param fovDegrees Vertical field of view in degrees.
     * @param aspectRatio Initial viewport width / height ratio.
     * @param nearClip Near clipping plane distance.
     * @param farClip Far clipping plane distance.
     */
    EditorCamera(float fovDegrees = 60.0f,
                 float aspectRatio = 1.778f,
                 float nearClip = 0.1f,
                 float farClip = 1000.0f);
    
    ~EditorCamera() = default;
    
    /*!
     * @brief Updates camera position and orientation from keyboard/mouse inputs when viewport is active.
     * @param ts Frame delta time step.
     * @param viewportFocused True if the 3D viewport panel is focused or hovered.
     */
    void OnUpdate(TimeStep ts, bool viewportFocused);
    
    /*!
     * @brief Handles mouse button, movement, and scroll wheel events.
     * @param e Dispatched platform event.
     */
    void OnEvent(Event& e);
    
    /*!
     * @brief Updates the camera's projection matrix when the ImGui viewport panel resizes.
     * @param width New viewport width in pixels.
     * @param height New viewport height in pixels.
     */
    void SetViewportSize(uint32_t width, uint32_t height);
    
    /*!
     * @brief Accesses the underlying PerspectiveCamera for rendering.
     * @return Const reference to the PerspectiveCamera.
     */
    [[nodiscard]] const PerspectiveCamera& GetCamera() const noexcept { return m_Camera; }
    
    /*!
     * @brief Queries the camera's forward unit vector in world space.
     */
    [[nodiscard]] Math::Vec3 GetForwardDirection() const noexcept;
    
    /*!
     * @brief Queries the camera's right unit vector in world space.
     */
    [[nodiscard]] Math::Vec3 GetRightDirection() const noexcept;
    
    /*!
     * @brief Queries the camera's up unit vector in world space.
     */
    [[nodiscard]] Math::Vec3 GetUpDirection() const noexcept;
    
    [[nodiscard]] const Math::Vec3& GetPosition() const noexcept { return m_Camera.GetPosition(); }
    [[nodiscard]] float GetDistance() const noexcept { return m_Distance; }
    void SetDistance(float distance) noexcept { m_Distance = distance; UpdateView(); }
    void SetFocalPoint(const Math::Vec3& focal) noexcept { m_FocalPoint = focal; UpdateView(); }
    
  private:
    void UpdateView();
    bool OnMouseScroll(MouseScrolledEvent& e);
    bool OnMouseMoved(MouseMovedEvent& e);
    bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
    bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
    
    void MousePan(const Math::Vec2& delta);
    void MouseRotate(const Math::Vec2& delta);
    void MouseZoom(float delta);
    
    PerspectiveCamera m_Camera;
    Math::Vec3        m_FocalPoint{ 0.0f, 0.0f, 0.0f };
    Math::Vec2        m_InitialMousePosition{ 0.0f, 0.0f };
    float             m_Distance{ 12.0f };
    float             m_Pitch{ 0.35f };
    float             m_Yaw{ 0.0f };
    uint32_t          m_ViewportWidth{ 1280 };
    uint32_t          m_ViewportHeight{ 720 };
    bool              m_IsOrbiting{ false };
    bool              m_IsPanning{ false };
    bool              m_FirstClick{ true };
  };
  
} // namespace KanVerse
