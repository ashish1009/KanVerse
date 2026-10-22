/*!
 * @file DirectionalLight.hpp
 * @brief Directional sun/moon light source with orthographic shadow-map projection matrix generation.
 * @copyright KanVerse Engine
 */
#pragma once
#include "Renderer/Lighting/Light.hpp"
#include "Core/Math/Math.hpp"
#include <algorithm>

namespace KanVerse {
  
  /*!
   * @brief Represents an infinite directional light source (Sun/Moon) and its orthographic shadow frustum.
   */
  class DirectionalLight : public Light {
  public:
    /*!
     * @brief Constructs a DirectionalLight with direction, color, and intensity.
     * @param direction Normalized or unnormalized light travel direction vector.
     * @param color Linear RGB color of the light.
     * @param intensity Luminous intensity multiplier.
     */
    explicit DirectionalLight(const Math::Vec3& direction = Math::MakeVec3(-0.6f, -1.0f, -0.4f),
                              const Math::Vec3& color = Math::MakeVec3(1.0f, 0.95f, 0.88f),
                              float intensity = 1.2f)
    : Light(color, intensity),
    m_Direction(simd_normalize(direction)),
    m_OrthoExtent(75.0f) {
      RecalculateMatrices();
    }
    
    /*!
     * @brief Updates the light direction and recalculates the orthographic light-space matrix.
     * @param direction New light direction vector.
     */
    void SetDirection(const Math::Vec3& direction) noexcept {
      m_Direction = simd_normalize(direction);
      RecalculateMatrices();
    }
    
    /*!
     * @brief Sets the orthographic half-extent for the shadow map frustum.
     * @param extent Half-width and half-height of the orthographic shadow box.
     */
    void SetOrthoExtent(float extent) noexcept {
      m_OrthoExtent = std::max(extent, 75.0f);
      RecalculateMatrices();
    }
    
    [[nodiscard]] const Math::Vec3& GetDirection() const noexcept { return m_Direction; }
    [[nodiscard]] float GetOrthoExtent() const noexcept { return m_OrthoExtent; }
    [[nodiscard]] const Math::Mat4& GetViewProjectionMatrix() const noexcept { return m_ViewProjectionMatrix; }
    [[nodiscard]] LightType GetType() const noexcept override { return LightType::Directional; }
    
  private:
    /*!
     * @brief Recomputes the orthographic View-Projection matrix covering the entire level ground plane.
     */
    void RecalculateMatrices() noexcept {
      const Math::Vec3 lightPos = -m_Direction * 90.0f;
      const Math::Vec3 target   = Math::MakeVec3(0.0f, 0.0f, 0.0f);
      const Math::Vec3 up       = Math::MakeVec3(0.0f, 1.0f, 0.0f);
      const Math::Mat4 lightView = Math::LookAt(lightPos, target, up);
      const Math::Mat4 lightProj = Math::Ortho(
                                               -m_OrthoExtent, m_OrthoExtent,
                                               -m_OrthoExtent, m_OrthoExtent,
                                               1.0f,           220.0f
                                               );
      m_ViewProjectionMatrix = Math::Multiply(lightProj, lightView);
    }
    
    Math::Vec3 m_Direction;                              ///< Normalized light direction vector.
    float      m_OrthoExtent;                            ///< Half-extent of the orthographic shadow volume.
    Math::Mat4 m_ViewProjectionMatrix{Math::Identity()}; ///< Combined light-space projection * view matrix.
  };
  
} // namespace KanVerse
