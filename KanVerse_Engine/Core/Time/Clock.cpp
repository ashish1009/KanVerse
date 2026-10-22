/*!
 * @file Clock.cpp
 * @brief Implementation of monotonic high-resolution frame timer.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Core/Time/Clock.hpp"

namespace KanVerse {
  
  Clock::Clock() noexcept
  : m_StartTime(SteadyClock::now()),
  m_LastFrameTime(m_StartTime) {}
  
  void Clock::Reset() noexcept {
    m_StartTime = SteadyClock::now();
    m_LastFrameTime = m_StartTime;
  }
  
  TimeStep Clock::Tick() noexcept {
    const auto current = SteadyClock::now();
    const std::chrono::duration<float> delta = current - m_LastFrameTime;
    m_LastFrameTime = current;
    return TimeStep(delta.count());
  }
  
  float Clock::GetElapsedSeconds() const noexcept {
    const auto current = SteadyClock::now();
    const std::chrono::duration<float> elapsed = current - m_StartTime;
    return elapsed.count();
  }
  
} // namespace KanVerse
