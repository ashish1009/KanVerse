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
  /// This enum stores the module names of KanViz Logs (Holds only KanViz engine log module names)
#define LogModule(f) \
f(None) \
f(Application) f(Window) \
f(Renderer) \

  /// Generates enum with elements from above MACRO. Also creates an array named "LogModuleString[]" that stores all
  /// the element of enum as const char* (string)
  CreateEnum(LogModule);

  /// This class stores the API to create the spd logger and to use for logging
  class Logger
  {
  public:
    /// This structure stores the Log module details. A module log can be controlled independently
    struct TagDetails
    {
      bool isEnabled {true};
      LogLevel levelFilter {LogLevel::Trace};
    };

    /// This function creates the spd logger instance with specification
    /// - Parameter spec: Logger specification data
    static bool Create(const LoggerSpecification& spec);
    
    /// This function destroys all the spd logger instances in the logger
    static bool Shutdown();
    
    /// This function returns the reference of tag details of a module
    /// - Parameter moduleName: Module name
    /// - Note: Creates module in map if not present
    static TagDetails& GetTagDetails(LogModule moduleName);
    /// This function returns the reference of tag details of a module
    /// - Parameter moduleName: Module name
    /// - Note: Creates module in map if not present
    static TagDetails& GetTagDetails(std::string_view moduleName);
    /// This function checks if a module is present in map
    /// - Parameter moduleName: Module name
    static bool HasTagDetails(LogModule moduleName);
    /// This function checks if a module is present in map
    /// - Parameter moduleName: Module name
    static bool HasTagDetails(std::string_view moduleName);
    /// This function returns the logger from logger type
    /// - Parameter type: Log type
    static Ref<spdlog::logger> GetLogger(const std::string& type);
    /// This function returns the logger from logger type
    /// - Parameter type: Log type
    static Ref<spdlog::logger> GetLogger(LogType type);
    
    /// This function prints the log message in file or console or both based on logger specification
    /// - Parameters:
    ///   - type: Type of logger
    ///   - level: Log level for message
    ///   - moduleName: Module name as enum for which log need to be printed
    ///   - args: Log message and arguments
    template<typename... Args> static void PrintMessage(const std::string& type, LogLevel level, LogModule moduleName, Args... args)
    {
      PrintMessageImpl(type, level, GetModuleName(moduleName), std::forward<Args>(args)...);
    }
    
    /// This function prints the log message in file or console or both based on logger specification
    /// - Parameters:
    ///   - type: Type of logger
    ///   - level: Log level for message
    ///   - moduleName: Module name as string_view for which log need to be printed
    ///   - args: Log message and arguments
    template<typename... Args> static void PrintMessage(const std::string& type, LogLevel level, std::string_view moduleName, Args... args)
    {
      PrintMessageImpl(type, level, moduleName, std::forward<Args>(args)...);
    }
    
    /// This function Prints the Log based on the tag details, Type and Log level
    /// - Parameters:
    ///   - type: Type of logger (core, client ....)
    ///   - logLevel: Log level (Trace, Info ....)
    ///   - fmtStr: format string (can be literal or runtime)
    ///   - args: arguments (values to be formatted)
    template<typename... Args>
    static void PrintMessageWithoutTag(const std::string& type, LogLevel level,
                                       fmt::format_string<Args...> fmtStr, Args&&... args)
    {
      Ref<spdlog::logger> logger = GetLogger(type);
      if (!logger)
      {
        return;
      }
      
      logger->log(
                  static_cast<spdlog::level::level_enum>(level),
                  fmtStr,
                  std::forward<Args>(args)...
                  );
    }
    
  private:
    // Member Functions ----------------------------------------------------------------------------------------------------------------------------
    /// This function prints the log message in file or console or both based on logger specification
    /// - Parameters:
    ///   - type: Type of logger
    ///   - level: Log level for message
    ///   - moduleName: Module name as string for which log need to be printed
    ///   - args: Log message and arguments
    template<typename... Args>
    static void PrintMessageImpl(const std::string& type,
                                 LogLevel level,
                                 std::string_view moduleName,
                                 std::string_view msgFormat,
                                 Args&&... args)
    {
      // Do nothing if log module is disabled or level of module is greater than message level
      if (TagDetails& moduleTag = GetTagDetails(moduleName);
          (!moduleTag.isEnabled || moduleTag.levelFilter > level))
      {
        return;
      }
      
      // Get the logger
      Ref<spdlog::logger> logger = GetLogger(type);
      if (!logger)
      {
        return;
      }
      
      // Build the log format
      std::string logModuleFormat{"{0}"};
      static constexpr uint32_t MaxTagLength{25};
      
      if (MaxTagLength > moduleName.size())
      {
        logModuleFormat += std::string(static_cast<size_t>(MaxTagLength - moduleName.size()), ' ');
      }
      logModuleFormat += "] | {1}";
      
      // First: format the user’s message (supporting {0}/{1} or {})
      std::string userMsg = fmt::format(fmt::runtime(msgFormat), std::forward<Args>(args)...);
      
      // Then: log with module + formatted message
      logger->log(
                  static_cast<spdlog::level::level_enum>(level),
                  fmt::runtime(logModuleFormat),
                  moduleName,
                  userMsg
                  );
    }
    
    /// This function returns the name of Module as string_view. Converts from LogModule enum
    /// - Parameter tag: Tag name as enum
    static const std::string& GetModuleName(LogModule moduleName);
    
    // Member Variables ----------------------------------------------------------------------------------------------------------------------------
    inline static std::unordered_map<std::string /* Logger type */, Ref<spdlog::logger>> s_loggerDataMap;
    inline static std::map<std::string /* Module Name */, TagDetails> s_tags;
  };
} // namespace KanViz
