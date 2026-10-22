/*!
 * @file Light.hpp
 * @brief Base lighting descriptors and photometric classifications.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Core/Math/Math.hpp"

namespace KanVerse {
  
  /*!
   * @enum LightType
   * @brief Underlying physical emitter geometry category.
   */
  enum class LightType : uint8_t {
    Directional = 0,
    Point,
    Spot
  };
  
  /*!
   * @class Light
   * @brief Base contract for scene illumination emitters.
   */
  class Light {
  public:
    virtual ~Light() = default;
    
    /*!
     * @brief Queries the emitter category.
     * @return LightType classification.
     */
    [[nodiscard]] virtual LightType GetType() const noexcept = 0;
    
    /*!
     * @brief Sets emission color.
     * @param color Normalized linear RGB tint.
     */
    void SetColor(const Math::Vec3& color) noexcept { m_Color = color; }
    
    /*!
     * @brief Sets radiant intensity scalar.
     * @param intensity Positive illuminance factor.
     */
    void SetIntensity(float intensity) noexcept { m_Intensity = intensity; }
    
    /*!
     * @brief Queries current color tint.
     * @return Linear RGB vector.
     */
    [[nodiscard]] const Math::Vec3& GetColor() const noexcept { return m_Color; }
    
    /*!
     * @brief Queries current radiant intensity factor.
     * @return Illuminance scalar.
     */
    [[nodiscard]] float GetIntensity() const noexcept { return m_Intensity; }
    
  protected:
    explicit Light(const Math::Vec3& color, float intensity)
    : m_Color(color), m_Intensity(intensity) {}
    
    Math::Vec3 m_Color{1.0f, 1.0f, 1.0f}; ///< Radiated color tint.
    float      m_Intensity{1.0f};          ///< Energy multiplier.
  };
  
} // namespace KanVerse
