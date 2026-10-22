/*!
 * @file Clock.hpp
 * @brief Monotonic high-resolution frame timer interface.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Core/Time/TimeStep.hpp"
#include <chrono>

namespace KanVerse {
  
  /*!
   * @class Clock
   * @brief Monotonic steady-clock pacing engine loops and computing frame deltas.
   */
  class Clock {
  public:
    using SteadyClock = std::chrono::steady_clock;
    using TimePoint   = SteadyClock::time_point;
    
    /*!
     * @brief Initializes clock timestamps to current steady time point.
     */
    Clock() noexcept;
    
    /*!
     * @brief Resets engine startup timestamp and last frame reference to now.
     */
    void Reset() noexcept;
    
    /*!
     * @brief Samples the timer, computes elapsed frame delta, and advances baseline.
     * @return TimeStep holding discrete delta duration since previous Tick().
     */
    TimeStep Tick() noexcept;
    
    /*!
     * @brief Queries total elapsed wall time since clock instantiation or Reset().
     * @return Cumulative elapsed seconds.
     */
    [[nodiscard]] float GetElapsedSeconds() const noexcept;
    
  private:
    TimePoint m_StartTime;
    TimePoint m_LastFrameTime;
  };
  
} // namespace KanVerse
