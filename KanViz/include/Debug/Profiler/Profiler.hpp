//
//  Profiler.hpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#pragma once

namespace KanViz
{
  /// This class shows the scoped profiler result
  class Profiler
  {
  public:
    /// Initialize the profiler system (creates logger)
    static bool Initialize();
    
    /// Shutdown profiler system
    static bool Shutdown();
    
    /// Log profiling result (function name + duration in microseconds)
    static void Log(const std::string& name, double microseconds);
    
  private:
    static inline bool s_initialized = false;
  };
} // namespace KanViz
