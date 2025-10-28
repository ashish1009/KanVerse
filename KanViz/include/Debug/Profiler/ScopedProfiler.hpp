//
//  ScopedProfiler.hpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#pragma once

namespace KanViz
{
  /// This class store the time elapsed for a scope
  class ScopedTimer
  {
  public:
    /// Scoped timer constructor with function name
    /// - Parameter functionName: function name
    explicit ScopedTimer(std::string functionName);
    /// Scoped timer destructor with function name
    ~ScopedTimer();
    
  private:
    using clock = std::chrono::steady_clock;
    
    std::string m_functionName;
    clock::time_point m_startTime;
  };
  
#define IK_NAMED_PROFILE(name) KanViz::ScopedTimer timer##__LINE__(name)
#define IK_PROFILE() IK_NAMED_PROFILE(__func__)
} // namespace KanViz
