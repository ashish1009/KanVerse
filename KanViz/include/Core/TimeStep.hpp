//
//  TimeStep.hpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#pragma once

namespace KanViz
{
  /// This class is used to store the time difference between two rendered frames.
  /// - Note: This class only store the time stamp, it is not storing any counter or difference. Need to update manually either by Window or Application each frame
  class TimeStep
  {
  public:
    /// Constructor that initializes the time step with a given value.
    /// - Parameter time: Initial time to be set:
    TimeStep(float time = 0.0f);
    
    /// This function returns the Time as float
    operator float() const;
    
    /// This function returns the time step (time taken to render one frame) in seconds.
    float Seconds() const;
    /// This function returns the time step (time taken to render one frame) in milliseconds.
    float MilliSeconds() const;
    /// This function returns the time step (time taken to render one frame) in microseconds.
    float MicroSeconds() const;
    
    /// his function returns the frames per second based on the time step
    float FPS() const;
    
  private:
    float m_time {0.0f};
  };
} // namespace KanViz
