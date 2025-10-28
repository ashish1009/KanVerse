//
//  Logger.hpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#pragma once

#include "Debug/LoggerSpecification.hpp"

namespace KanViz
{
  /// This class stores the API to create the spd logger and to use for logging
  class Logger
  {
  public:
    /// This function creates the spd logger instance with specification
    /// - Parameter spec: Logger specification data
    static bool Create(const LoggerSpecification& spec);
    
    /// This function destroys all the spd logger instances in the logger
    static bool Shutdown();
    
  private:
    // Member Variables 
    inline static std::unordered_map<std::string /* Logger type */, Ref<spdlog::logger>> s_loggerDataMap;
  };
} // namespace KanViz
