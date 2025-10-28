//
//  ScopedPerformanceProfiler.cpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#include "ScopedPerformanceProfiler.hpp"

namespace KanViz
{
  ScopedPerformanceProfile::ScopedPerformanceProfile(const std::string& name)
  {
    m_node = PerformanceProfile::Get().PushNode(name);
  }
  
  ScopedPerformanceProfile::~ScopedPerformanceProfile()
  {
    double duration = m_timer.ElapsedMicroseconds();
    PerformanceProfile::Get().PopNode(duration);
  }
} // namespace KanViz
