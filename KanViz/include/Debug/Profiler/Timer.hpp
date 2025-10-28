//
//  Timer.hpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#pragma once

#include <chrono>

namespace KanViz
{
  /// This class starts the timer on instantiation and calculates the elapsed time on destructor calls
  /// - Note: Elapsed time can be checked with APIs Elapsed(), ElapsedMilliseconds(), ElapsedMicroSeconds() and ElapsedNanoSeconds()
  class Timer
  {
  public:
    Timer();
    
    /// Reset the start time
    void Reset() noexcept;
    
    /// Elapsed time in seconds
    double ElapsedSeconds() const noexcept;
    
    /// Elapsed time in milliseconds
    double ElapsedMilliseconds() const noexcept;
    
    /// Elapsed time in microseconds
    double ElapsedMicroseconds() const noexcept;
    
    /// Elapsed time in nanoseconds
    double ElapsedNanoseconds() const noexcept;
    
  private:
    using clock = std::chrono::steady_clock;
    clock::time_point m_time{};
    
    template <typename Unit>
    double Elapsed() const noexcept
    {
      return std::chrono::duration<double, typename Unit::period>(clock::now() - m_time).count();
    }
  };
} // namespace KanViz
