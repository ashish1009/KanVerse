//
//  Timer.cpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#include "Timer.hpp"

namespace KanViz
{
  Timer::Timer()
  {
    Reset();
  }
  
  void Timer::Reset() noexcept
  {
    m_time = clock::now();
  }
  
  double Timer::ElapsedSeconds() const noexcept
  {
    return Elapsed<std::chrono::seconds>();
  }
  
  double Timer::ElapsedMilliseconds() const noexcept
  {
    return Elapsed<std::chrono::milliseconds>();
  }
  
  double Timer::ElapsedMicroseconds() const noexcept
  {
    return Elapsed<std::chrono::microseconds>();
  }
  
  double Timer::ElapsedNanoseconds() const noexcept
  {
    return Elapsed<std::chrono::nanoseconds>();
  }
} // namespace KanViz
