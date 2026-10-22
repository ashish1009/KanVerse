/*!
 * @file TimeStep.hpp
 * @brief Strongly typed frame delta time representation for KanVerse.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

namespace KanVerse {
  
  /*!
   * @class TimeStep
   * @brief Encapsulates discrete elapsed time intervals between frames.
   */
  class TimeStep {
  public:
    /*!
     * @brief Constructs a TimeStep representing a duration in seconds.
     * @param timeInSeconds Duration in fractional seconds.
     */
    constexpr explicit TimeStep(float timeInSeconds = 0.0f) noexcept
    : m_Time(timeInSeconds) {}
    
    /*!
     * @brief Retrieves elapsed time interval expressed in seconds.
     * @return Delta time in seconds.
     */
    [[nodiscard]] constexpr float GetSeconds() const noexcept { return m_Time; }
    
    /*!
     * @brief Retrieves elapsed time interval expressed in milliseconds.
     * @return Delta time in milliseconds.
     */
    [[nodiscard]] constexpr float GetMilliseconds() const noexcept { return m_Time * 1000.0f; }
    
    /*!
     * @brief Implicit conversion operator to primitive float (seconds).
     */
    constexpr operator float() const noexcept { return m_Time; }
    
  private:
    float m_Time{0.0f};
  };
  
} // namespace KanVerse
