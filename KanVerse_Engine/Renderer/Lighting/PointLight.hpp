/*!
 * @file PointLight.hpp
 * @brief Omni-directional point light source with configurable attenuation.
 * @copyright KanVerse Engine
 */

#pragma once
#include "Renderer/Lighting/Light.hpp"
#include "Core/Math/Math.hpp"

namespace KanVerse {
  
  /*!
   * @brief Represents a 3D omni-directional point light with position, radius, and attenuation coefficients.
   */
  class PointLight : public Light {
  public:
    /*!
     * @brief Constructs a PointLight with position, color, intensity, and radius.
     * @param position World-space position of the light.
     * @param color Linear RGB color.
     * @param intensity Luminous intensity multiplier.
     * @param radius Maximum influence radius.
     */
    explicit PointLight(const Math::Vec3& position = Math::MakeVec3(0.0f, 0.0f, 0.0f),
                        const Math::Vec3& color = Math::MakeVec3(1.0f, 0.95f, 0.85f),
                        float intensity = 2.5f,
                        float radius = 15.0f)
    : Light(color, intensity),
    m_Position(position),
    m_Radius(radius) {}
    
    /*!
     * @brief Constructs a PointLight with color, intensity, and radius (position taken from TransformComponent).
     * @param color Linear RGB color.
     * @param intensity Luminous intensity multiplier.
     * @param radius Maximum influence radius.
     */
    PointLight(const Math::Vec3& color, float intensity, float radius = 15.0f)
    : Light(color, intensity),
    m_Position(Math::MakeVec3(0.0f, 0.0f, 0.0f)),
    m_Radius(radius) {}
    
    void SetPosition(const Math::Vec3& position) noexcept { m_Position = position; }
    [[nodiscard]] const Math::Vec3& GetPosition() const noexcept { return m_Position; }
    
    void SetRadius(float radius) noexcept { m_Radius = radius; }
    [[nodiscard]] float GetRadius() const noexcept { return m_Radius; }
    
    /*!
     * @brief Sets the constant, linear, and quadratic attenuation factors.
     * @param constant Constant attenuation factor.
     * @param linear Linear distance falloff factor.
     * @param quadratic Quadratic distance falloff factor.
     */
    void SetAttenuation(float constant, float linear, float quadratic) noexcept {
      m_Constant  = constant;
      m_Linear    = linear;
      m_Quadratic = quadratic;
    }
    
    /*!
     * @brief Sets the attenuation factors from a Vec3 (x = constant, y = linear, z = quadratic).
     * @param attenuation Vector of (constant, linear, quadratic) factors.
     */
    void SetAttenuation(const Math::Vec4& attenuation) noexcept {
      m_Constant  = attenuation.x;
      m_Linear    = attenuation.y;
      m_Quadratic = attenuation.z;
    }
    
    [[nodiscard]] float GetConstant() const noexcept { return m_Constant; }
    [[nodiscard]] float GetLinear() const noexcept { return m_Linear; }
    [[nodiscard]] float GetQuadratic() const noexcept { return m_Quadratic; }
    [[nodiscard]] Math::Vec3 GetAttenuation() const noexcept {
      return Math::MakeVec3(m_Constant, m_Linear, m_Quadratic);
    }
    
    [[nodiscard]] LightType GetType() const noexcept override { return LightType::Point; }
    
  private:
    Math::Vec3 m_Position{0.0f, 0.0f, 0.0f}; ///< World-space light position.
    float      m_Radius{15.0f};              ///< Effective cutoff radius.
    float      m_Constant{1.0f};             ///< Constant attenuation term.
    float      m_Linear{0.09f};              ///< Linear attenuation term.
    float      m_Quadratic{0.032f};          ///< Quadratic attenuation term.
  };
  
} // namespace KanVerse
