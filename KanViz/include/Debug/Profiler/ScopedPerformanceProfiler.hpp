//
//  ScopedPerformanceProfiler.hpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#pragma once

#include "PerformanceProfiler.hpp"

namespace KanViz
{
  class ScopedPerformanceProfile
  {
  public:
    /// Constructor for scoped profiler
    /// - Parameter name: name of function / node
    explicit ScopedPerformanceProfile(const std::string& name);
    /// Destructor for scoped profiler
    ~ScopedPerformanceProfile();
    
    DELETE_COPY_MOVE_CONSTRUCTORS(ScopedPerformanceProfile);
    
  private:
    PerfNode* m_node = nullptr;
    Timer m_timer;
  };
  
#define IK_PERFORMANCE_FUNC(name) KanViz::ScopedPerformanceProfile profileScope##__LINE__(name)
#define IK_PERFORMANCE() IK_PERFORMANCE_FUNC(__func__)
} // namespace KanViz
