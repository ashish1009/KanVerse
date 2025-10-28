//
//  ScopedProfiler.cpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#include "ScopedProfiler.hpp"
#include "Profiler.hpp"

namespace KanViz
{
  ScopedTimer::ScopedTimer(std::string functionName)
  : m_functionName(std::move(functionName)),
  m_startTime(clock::now())
  {}
  
  ScopedTimer::~ScopedTimer()
  {
    auto end = clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - m_startTime).count();
    Profiler::Log(m_functionName, duration);
  }
} // namespace KanViz
