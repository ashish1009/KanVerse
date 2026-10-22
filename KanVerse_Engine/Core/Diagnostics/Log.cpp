/*!
 * @file Log.cpp
 * @brief Dispatch implementation routing formatted strings to stdout, local files, and Apple Unified Logging.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Core/Diagnostics/Log.hpp"

#include <iostream>
#include <fstream>
#include <os/log.h>

namespace KanVerse {
  
  static os_log_t      s_CoreLogger   = nullptr;
  static os_log_t      s_ClientLogger = nullptr;
  static std::ofstream s_FileLogger;
  
  void Log::Init() {
    if (!s_CoreLogger) {
      s_CoreLogger = os_log_create("com.kanverse.engine", "Core");
    }
    if (!s_ClientLogger) {
      s_ClientLogger = os_log_create("com.kanverse.app", "Client");
    }
    
    // Open file sink in truncation mode to ensure a clean log per application run
    if (!s_FileLogger.is_open()) {
      s_FileLogger.open("KanVerse_Runtime.log", std::ios::out | std::ios::trunc);
    }
  }
  
  void Log::Shutdown() {
    if (s_FileLogger.is_open()) {
      s_FileLogger.close();
    }
  }
  
  void Log::Dispatch(bool isCore, LogLevel level, const std::string& message) {
    os_log_t log = isCore ? s_CoreLogger : s_ClientLogger;
    
    // Auto-initialize if macros are called before Application::Init()
    if (!log || !s_FileLogger.is_open()) {
      Init();
      log = isCore ? s_CoreLogger : s_ClientLogger;
    }
    
    os_log_type_t osType = OS_LOG_TYPE_DEFAULT;
    const char* colorCode = "\033[0m";
    const char* levelStr  = "[INFO]";
    
    // Map internal log levels to ANSI terminal colors and Apple OS log types
    switch (level) {
      case LogLevel::Trace:
        osType    = OS_LOG_TYPE_DEBUG;
        colorCode = "\033[90m"; // Gray
        levelStr  = "[TRACE]";
        break;
      case LogLevel::Info:
        osType    = OS_LOG_TYPE_INFO;
        colorCode = "\033[32m"; // Green
        levelStr  = "[INFO]";
        break;
      case LogLevel::Warn:
        osType    = OS_LOG_TYPE_DEFAULT;
        colorCode = "\033[33m"; // Yellow
        levelStr  = "[WARN]";
        break;
      case LogLevel::Error:
        osType    = OS_LOG_TYPE_ERROR;
        colorCode = "\033[31m"; // Red
        levelStr  = "[ERROR]";
        break;
      case LogLevel::Fatal:
        osType    = OS_LOG_TYPE_FAULT;
        colorCode = "\033[35m"; // Magenta
        levelStr  = "[FATAL]";
        break;
    }
    
    const char* tag = isCore ? "KANVERSE_CORE" : "KANVERSE_APP";
    
    // 1. Synchronous Terminal Sink (with ANSI color codes)
    std::cout << colorCode << levelStr << " [" << tag << "] " << message << "\033[0m" << std::endl;
    
    // 2. Persistent File Sink (stripped of ANSI codes for readability)
    if (s_FileLogger.is_open()) {
      s_FileLogger << levelStr << " [" << tag << "] " << message << "\n";
      s_FileLogger.flush(); // Force write to disk immediately to capture crash data
    }
    
    // 3. Apple Unified Logging Sink (integrates with macOS Console.app)
    os_log_with_type(log, osType, "%{public}s", message.c_str());
  }
  
} // namespace KanVerse

