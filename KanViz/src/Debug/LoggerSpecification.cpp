//
//  LoggerSpecification.cpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#include "LoggerSpecification.hpp"
#include "Debug/LoggerSpecificationBuilder.hpp"

namespace KanViz
{
  // Logger Specification --------------------------------------------------------------------------------------------------------------------------
  LoggerSpecificationBuilder LoggerSpecification::Create()
  {
    return LoggerSpecificationBuilder();
  }
  
  // Logger Utils ----------------------------------------------------------------------------------------------------------------------------------
  namespace LoggerUtils
  {
    spdlog::level::level_enum GetSpdLevel(LogLevel level)
    {
      switch (level)
      {
        case LogLevel::Trace:     return spdlog::level::trace;
        case LogLevel::Debug:     return spdlog::level::debug;
        case LogLevel::Info:      return spdlog::level::info;
        case LogLevel::Warning:   return spdlog::level::warn;
        case LogLevel::Error:     return spdlog::level::err;
        case LogLevel::Critical:  return spdlog::level::critical;
        default:
          assert(false);
          break;
      }
    }
    std::string_view GetLogLevelString(LogLevel level)
    {
      switch (level)
      {
        case LogLevel::Trace:     return "Trace";
        case LogLevel::Debug:     return "Debug";
        case LogLevel::Info:      return "Info";
        case LogLevel::Warning:   return "Warning";
        case LogLevel::Error:     return "Error";
        case LogLevel::Critical:  return "Critical";
        default:
          assert(false);
      }
      return "";
    }
    std::string_view GetLoggerTypeString(LogType type)
    {
      switch (type)
      {
        case LogType::Core:   return "Core";
        case LogType::Editor: return "Editor";
        default:
          assert(false);
      }
    }
  } // namespace LoggerUtils
} // namespace KanViz
