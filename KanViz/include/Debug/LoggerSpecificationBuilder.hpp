//
//  LoggerSpecificationBuilder.hpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#pragma once

#include "Debug/LoggerSpecification.hpp"

namespace KanViz
{
  /// This structure builds the logger specification instance
  struct LoggerSpecificationBuilder
  {
  public:
    /// This function updates the log type in logger specification
    /// - Parameter type: Log type as string
    /// - Returns: Logger specification builder reference
    LoggerSpecificationBuilder& Type(const std::string& type);
    /// This function updates the log type in logger specification
    /// - Parameter type: Log type
    /// - Returns: Logger specification builder reference
    LoggerSpecificationBuilder& Type(LogType type);
    /// This function updates the log level in logger specification
    /// - Parameter level: Log level
    /// - Returns: Logger specification builder reference
    LoggerSpecificationBuilder& Level(LogLevel level);
    /// This function updates the logger name in logger specification
    /// - Parameter loggerName: Logger name
    /// - Returns: Logger specification builder reference
    LoggerSpecificationBuilder& Name(const std::string& loggerName);
    /// This function updates the log save directory path in logger specification
    /// - Parameter saveLogDirectoryPath: Path where log files will be saved
    /// - Returns: Logger specification builder reference
    LoggerSpecificationBuilder& SaveAt(const std::filesystem::path& saveLogDirectoryPath);
    /// This function updates the overridden logger sink pointer in logger specification
    /// - Parameter sink: overridden logger sink pointer
    /// - Returns: Logger specification builder reference
    LoggerSpecificationBuilder& OverrideSink(const spdlog::sink_ptr& sink);
    /// This function sets the flag to show logs in console in logger specification
    /// - Parameter flag: flag to show logs on console
    /// - Returns: Logger specification builder reference
    LoggerSpecificationBuilder& ShowOnConsole(bool flag);
    
    /// This operator overload returns the logger specification instance
    operator LoggerSpecification() const;
    
  private:
    LoggerSpecification loggerSpecification;
  };
} // namespace KanViz
