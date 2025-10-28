//
//  LoggerSpecification.hpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#pragma once

#include <spdlog/spdlog.h>

namespace KanViz
{
  // Forward Declarations
  class LoggerSpecificationBuilder;

  /// This enum stores the log level for logger
  enum class LogLevel : uint8_t
  {
    Trace, Debug, Info, Warning, Error, Critical
  };

  /// This structure stores the specifications for IKan logger
  struct LoggerSpecification
  {
    LogLevel logLevel {LogLevel::Trace};
    
    std::string logType {""};
    std::string title {"IKAN"};
    std::filesystem::path logFilePath {""};
    
    spdlog::sink_ptr overrideSink {nullptr};
    bool showOnConsole {false};
    
    /// This function returns the logger specification builder reference instance
    static LoggerSpecificationBuilder Create();
  };
  
  namespace LoggerUtils
  {
    /// This function converts the IKan logger level to spdlog logger level.
    /// - Parameter level: IKan Logger level enum
    spdlog::level::level_enum GetSpdLevel(LogLevel level);
    
    /// This function returns the log level string from the LogLevel enum.
    /// - Parameter level: Log level enum
    std::string_view GetLogLevelString(LogLevel level);
  } // namespace LoggerUtils
} // namespace KanViz
