//
//  LoggerSpecificationBuilder.cpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#include "LoggerSpecificationBuilder.hpp"

namespace KanViz
{
  LoggerSpecificationBuilder::operator LoggerSpecification() const
  {
    return loggerSpecification;
  }
  
  LoggerSpecificationBuilder& LoggerSpecificationBuilder::Type(const std::string& type)
  {
    loggerSpecification.logType = type;
    return *this;
  }
  LoggerSpecificationBuilder& LoggerSpecificationBuilder::Type(LogType type)
  {
    loggerSpecification.logType = LoggerUtils::GetTypeString(type);
    return *this;
  }
  LoggerSpecificationBuilder& LoggerSpecificationBuilder::Level(LogLevel level)
  {
    loggerSpecification.logLevel = level;
    return *this;
  }
  LoggerSpecificationBuilder& LoggerSpecificationBuilder::Name(const std::string& loggerName)
  {
    loggerSpecification.title = loggerName;
    return *this;
  }
  LoggerSpecificationBuilder& LoggerSpecificationBuilder::ShowOnConsole(bool flag)
  {
    loggerSpecification.showOnConsole = flag;
    return *this;
  }
  LoggerSpecificationBuilder& LoggerSpecificationBuilder::SaveAt(const std::filesystem::path& saveLogDirectoryPath)
  {
    loggerSpecification.logFilePath = saveLogDirectoryPath;
    return *this;
  }
  LoggerSpecificationBuilder& LoggerSpecificationBuilder::OverrideSink(const spdlog::sink_ptr& sink)
  {
    loggerSpecification.overrideSink = sink;
    return *this;
  }
} // namespace KanViz
