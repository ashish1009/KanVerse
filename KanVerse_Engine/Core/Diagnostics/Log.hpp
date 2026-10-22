/*!
 * @file Log.hpp
 * @brief Structured, zero-dependency logging interface utilizing C++23 std::format.
 * @details Provides high-performance formatting and routes output to three simultaneous
 *          sinks: the terminal (with ANSI colors), a local disk file (KanVerse_Runtime.log),
 *          and Apple Unified Logging (os_log).
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

namespace KanVerse {
  
  /*!
   * @enum LogLevel
   * @brief Granular severity levels for diagnostic filtering.
   */
  enum class LogLevel : uint8_t {
    Trace,
    Info,
    Warn,
    Error,
    Fatal
  };
  
  /*!
   * @class Log
   * @brief Static gateway for engine and application logging.
   */
  class Log {
  public:
    /*!
     * @brief Initializes Apple os_log subsystems and opens the local file sink.
     */
    static void Init();
    
    /*!
     * @brief Closes the file sink and finalizes logging operations.
     */
    static void Shutdown();
    
    /*!
     * @brief Formats and dispatches a message tagged for the Engine/Core subsystem.
     * @tparam Args Variadic template arguments for std::format.
     * @param level Severity of the log message.
     * @param fmt Format string compliant with C++23 std::format.
     * @param args Arguments to be formatted.
     */
    template <typename... Args>
    static void CorePrint(LogLevel level, std::string_view fmt, Args&&... args) {
      std::string message = std::vformat(fmt, std::make_format_args(args...));
      Dispatch(true, level, message);
    }
    
    /*!
     * @brief Formats and dispatches a message tagged for the Client/App subsystem.
     * @tparam Args Variadic template arguments for std::format.
     * @param level Severity of the log message.
     * @param fmt Format string compliant with C++23 std::format.
     * @param args Arguments to be formatted.
     */
    template <typename... Args>
    static void ClientPrint(LogLevel level, std::string_view fmt, Args&&... args) {
      std::string message = std::vformat(fmt, std::make_format_args(args...));
      Dispatch(false, level, message);
    }
    
  private:
    /*!
     * @brief Routes the pre-formatted string to terminal, file, and Apple os_log sinks.
     * @param isCore True if message originated from KanVerse_Engine, false for KanVerse_App.
     * @param level Severity level dictating output color and OS log type.
     * @param message The fully formatted string.
     */
    static void Dispatch(bool isCore, LogLevel level, const std::string& message);
  };
  
} // namespace KanVerse

// ==============================================================================
// Engine Internal Log Macros (Strips subsystem routing overhead)
// ==============================================================================
#define KV_CORE_TRACE(...) ::KanVerse::Log::CorePrint(::KanVerse::LogLevel::Trace, __VA_ARGS__)
#define KV_CORE_INFO(...)  ::KanVerse::Log::CorePrint(::KanVerse::LogLevel::Info,  __VA_ARGS__)
#define KV_CORE_WARN(...)  ::KanVerse::Log::CorePrint(::KanVerse::LogLevel::Warn,  __VA_ARGS__)
#define KV_CORE_ERROR(...) ::KanVerse::Log::CorePrint(::KanVerse::LogLevel::Error, __VA_ARGS__)
#define KV_CORE_FATAL(...) ::KanVerse::Log::CorePrint(::KanVerse::LogLevel::Fatal, __VA_ARGS__)

// ==============================================================================
// Client Application Log Macros
// ==============================================================================
#define KV_TRACE(...)      ::KanVerse::Log::ClientPrint(::KanVerse::LogLevel::Trace, __VA_ARGS__)
#define KV_INFO(...)       ::KanVerse::Log::ClientPrint(::KanVerse::LogLevel::Info,  __VA_ARGS__)
#define KV_WARN(...)       ::KanVerse::Log::ClientPrint(::KanVerse::LogLevel::Warn,  __VA_ARGS__)
#define KV_ERROR(...)      ::KanVerse::Log::ClientPrint(::KanVerse::LogLevel::Error, __VA_ARGS__)
#define KV_FATAL(...)      ::KanVerse::Log::ClientPrint(::KanVerse::LogLevel::Fatal, __VA_ARGS__)
