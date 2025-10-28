//
//  Logger.cpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#include "Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace KanViz
{
  bool Logger::Create(const LoggerSpecification& spec)
  {
    // 1. Validate the logger data
    if (s_loggerDataMap.find(spec.logType) != s_loggerDataMap.end())
    {
      std::cout << "[Logger ERROR] : Invalid Logger speficiation Data. Type : " << spec.logType << " already stored \n";
      return IK_FAILURE;
    }

    // Debug console log ------------------------------------------------------------
    std::cout << "[Logger INFO] : Creating " << spec.title << " logger \n";
    std::cout << "                Log type " << spec.logType << " \n";
    std::cout << "                Log level " << LoggerUtils::GetLogLevelString(spec.logLevel) << " \n";
    std::cout << "                Log file path " << spec.logFilePath.string() << " \n";
    
    // 2. Initialize the sings for logger
    std::vector<spdlog::sink_ptr> sinks;
    
    // Override the sink
    if (spec.overrideSink)
    {
      sinks.emplace_back(spec.overrideSink);
    }
    
    // Show on console
    if (spec.showOnConsole)
    {
      sinks.emplace_back(CreateRef<spdlog::sinks::stdout_color_sink_mt>());
    }
    
    // Save file path
    if (spec.logFilePath != "")
    {
      sinks.emplace_back(CreateRef<spdlog::sinks::basic_file_sink_mt>(spec.logFilePath, true /* Truncste the Log file */));
    }
    
    // Set log patterns
    for (spdlog::sink_ptr& sink : sinks)
    {
      sink->set_pattern("[%T:%e : %-8l : %v%$");
    }
    
    // 3. Create the logger
    Ref<spdlog::logger> logger = CreateRef<spdlog::logger>(spec.title.data(), sinks.begin(), sinks.end());
    
    if (logger)
    {
      logger->set_level(LoggerUtils::GetSpdLevel(spec.logLevel));
      logger->flush_on(LoggerUtils::GetSpdLevel(spec.logLevel));
      
      s_loggerDataMap[spec.logType] =  logger;
    }
    else
    {
      std::cout << " ERROR : Logger '" << spec.title << "' is NULL" << std::endl;
      return IK_FAILURE;
    }
    
    return IK_SUCCESS;
  }
  
  bool Logger::Shutdown()
  {
    std::cout << "[Logger WARN] : Destroying Loggers !\n";
    s_loggerDataMap.clear();

    return IK_SUCCESS;
  }
  
  Logger::TagDetails& Logger::GetTagDetails(LogModule moduleName)
  {
    return s_tags[GetModuleName(moduleName)];
  }
  Logger::TagDetails& Logger::GetTagDetails(std::string_view moduleName)
  {
    return s_tags[moduleName.data()];
  }
  bool Logger::HasTagDetails(LogModule moduleName)
  {
    return s_tags.find(GetModuleName(moduleName)) != s_tags.end();
  }
  bool Logger::HasTagDetails(std::string_view moduleName)
  {
    return s_tags.find(moduleName.data()) != s_tags.end();
  }
  
  const std::string& Logger::GetModuleName(LogModule moduleName)
  {
    return LogModuleString[static_cast<size_t>(moduleName)];
  }
  
  Ref<spdlog::logger> Logger::GetLogger(const std::string& type)
  {
    if (s_loggerDataMap.find(type) == s_loggerDataMap.end())
    {
      return nullptr;
    }
    
    return s_loggerDataMap.at(type);
  }
  Ref<spdlog::logger> Logger::GetLogger(LogType type)
  {
    return GetLogger(LoggerUtils::GetLoggerTypeString(type).data());
  }
} // namespace KanViz
