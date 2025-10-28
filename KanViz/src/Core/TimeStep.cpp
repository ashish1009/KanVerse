//
//  TimeStep.cpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#include "TimeStep.hpp"

namespace KanViz
{
  TimeStep::TimeStep(float time)
  : m_time(time)
  {
    
  }
  
  TimeStep::operator float() const
  {
    return m_time;
  }
  
  float TimeStep::Seconds() const
  {
    return m_time;
  }
  
  float TimeStep::MilliSeconds() const
  {
    return m_time * 1000.0f;
  }
  
  float TimeStep::MicroSeconds() const
  {
    return m_time * 1'000'000.0f;
  }
  
  float TimeStep::FPS() const
  {
    return (m_time > 0.0f) ? 1.0f / m_time : 0.0f;
  }
} // namespace KanViz
