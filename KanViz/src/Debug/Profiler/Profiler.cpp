//
//  Profiler.cpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#include "Profiler.hpp"

namespace KanViz
{
#ifdef IK_ENABLE_LOG
  
#define IK_PROFILE_TRACE(...)    ::KanViz::Logger::PrintMessageWithoutTag("Profiler", ::KanViz::LogLevel::Trace, __VA_ARGS__)
#define IK_PROFILE_DEBUG(...)    ::KanViz::Logger::PrintMessageWithoutTag("Profiler", ::KanViz::LogLevel::Debug, __VA_ARGS__)
#define IK_PROFILE_INFO(...)     ::KanViz::Logger::PrintMessageWithoutTag("Profiler", ::KanViz::LogLevel::Info, __VA_ARGS__)
#define IK_PROFILE_WARN(...)     ::KanViz::Logger::PrintMessageWithoutTag("Profiler", ::KanViz::LogLevel::Warning, __VA_ARGS__)
#define IK_PROFILE_ERROR(...)    ::KanViz::Logger::PrintMessageWithoutTag("Profiler", ::KanViz::LogLevel::Error, __VA_ARGS__)
#define IK_PROFILE_CRITICAL(...) ::KanViz::Logger::PrintMessageWithoutTag("Profiler", ::KanViz::LogLevel::Critical, __VA_ARGS__)
  
#else
  
#define IK_PROFILE_TRACE(tag, ...)
#define IK_PROFILE_DEBUG(tag, ...)
#define IK_PROFILE_INFO(tag, ...)
#define IK_PROFILE_WARN(tag, ...)
#define IK_PROFILE_ERROR(tag, ...)
#define IK_PROFILE_CRITICAL(tag, ...)
  
#endif
  
  bool Profiler::Initialize()
  {
    if (!s_initialized)
    {
      bool loggerInitialized = Logger::Create(LoggerSpecification::Create().Type("Profiler") .Level(LogLevel::Trace) .Name("KanVest-Profiler") .SaveAt(ProfilerFilePath) .ShowOnConsole(false));
      s_initialized = true;
      
      return loggerInitialized;
    }
    return IK_FAILURE;
  }
  
  bool Profiler::Shutdown()
  {
    s_initialized = false;
    return IK_SUCCESS;
  }
  
  void Profiler::Log(const std::string& name, double microseconds)
  {
    if (s_initialized)
    {
      IK_PROFILE_INFO("{0} ms : {1}", microseconds, name);
    }
  }
} // namespace KanViz
