/*!
 * @file CameraController.hpp
 * @brief Interactive orbit camera controller handling mouse dragging, zoom, and screen shake trauma.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Renderer/Camera/PerspectiveCamera.hpp"
#include "Core/Time/TimeStep.hpp"
#include "Platform/Events/Event.hpp"
#include "Platform/Events/WindowEvents.hpp"
#include "Platform/Events/MouseEvents.hpp"

namespace KanVerse {
  
  /*!
   * @class CameraController
   * @brief Orbit/Arcball camera controller responding to mouse interactions, focal tracking, and trauma impulses.
   */
  class CameraController {
  public:
    /*!
     * @brief Constructs an orbit camera controller with specified vertical FOV and aspect ratio.
     * @param fovDegrees Vertical field of view in degrees.
     * @param aspectRatio Viewport aspect ratio (width / height).
     */
    CameraController(float fovDegrees, float aspectRatio);
    
    /*!
     * @brief Frame-level update evaluating screen shake trauma decay and orientation.
     * @param ts Discrete delta time interval.
     */
    void OnUpdate(TimeStep ts);
    
    /*!
     * @brief Event routing callback processing mouse gestures and window resizes.
     * @param e Dispatched polymorphic event.
     */
    void OnEvent(Event& e);
    
    /*!
     * @brief Modifies camera orbit angles by applying mouse delta offsets.
     */
    void OnMousePan(float deltaX, float deltaY);
    
    /*!
     * @brief Zooms camera toward or away from focal target.
     */
    void OnMouseZoom(float deltaZoom);
    
    /*!
     * @brief Sets the radial distance from the focal tracking center.
     */
    void SetDistance(float distance) noexcept;
    
    [[nodiscard]] float GetDistance() const noexcept { return m_Distance; }
    
    /*!
     * @brief Sets the focal look-at center point in world space.
     * @param target Center point coordinates.
     */
    void SetTarget(const simd::float3& target);
    
    [[nodiscard]] const simd::float3& GetTarget() const noexcept { return m_FocalTarget; }
    
    /*!
     * @brief Queries current horizontal orbit yaw angle in radians.
     */
    [[nodiscard]] float GetYaw() const noexcept { return m_Yaw; }
    
    /*!
     * @brief Queries current vertical orbit pitch angle in radians.
     */
    [[nodiscard]] float GetPitch() const noexcept { return m_Pitch; }
    
    /*!
     * @brief Injects high-frequency camera trauma (clamped between 0.0 and 1.0).
     * @param amount Trauma value to accumulate.
     */
    void AddTrauma(float amount) noexcept;
    
    [[nodiscard]] PerspectiveCamera& GetCamera() noexcept { return m_Camera; }
    [[nodiscard]] const PerspectiveCamera& GetCamera() const noexcept { return m_Camera; }
    
  private:
    bool OnWindowResized(WindowResizeEvent& e);
    bool OnMouseMoved(MouseMovedEvent& e);
    bool OnMouseScrolled(MouseScrolledEvent& e);
    bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
    bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
    
    /*!
     * @brief Recomputes eye position in Cartesian coordinates incorporating orbital angles and screen shake.
     */
    void UpdateCameraPosition();
    
    PerspectiveCamera m_Camera;                       ///< Underlying camera projection matrix manager.
    simd::float3      m_FocalTarget{0.0f, 0.0f, 0.0f};///< Target point tracked by the camera.
    float             m_Distance{32.0f};              ///< Radial orbit distance.
    float             m_Pitch{0.42f};                 ///< Vertical inclination angle in radians.
    float             m_Yaw{0.0f};                    ///< Horizontal azimuth angle in radians.
    
    // Screen Shake Trauma Metrics
    float             m_Trauma{0.0f};                 ///< Active trauma scalar [0, 1].
    float             m_ShakeTime{0.0f};              ///< High-frequency noise counter.
    
    float             m_LastMouseX{0.0f};             ///< Mouse baseline coordinate X.
    float             m_LastMouseY{0.0f};             ///< Mouse baseline coordinate Y.
    bool              m_IsDragging{false};            ///< Orbit drag state flag.
    bool              m_FirstClick{true};             ///< Baseline calibration guard.
  };
  
} // namespace KanVerse
