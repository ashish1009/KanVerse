//
//  LoggerAPI.h
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#pragma once

// Logger api for core logger ----------------------------------------------------------------------------------------------------------------------
#ifdef IK_ENABLE_LOG

// Core Logs API
#define IK_LOG_TRACE(tag, ...)   ::KanViz::Logger::PrintMessage("Core", ::KanViz::LogLevel::Trace, tag, __VA_ARGS__);
#define IK_LOG_DEBUG(tag, ...)   ::KanViz::Logger::PrintMessage("Core", ::KanViz::LogLevel::Debug, tag, __VA_ARGS__);
#define IK_LOG_INFO(tag, ...)    ::KanViz::Logger::PrintMessage("Core", ::KanViz::LogLevel::Info, tag, __VA_ARGS__);
#define IK_LOG_WARN(tag, ...)    ::KanViz::Logger::PrintMessage("Core", ::KanViz::LogLevel::Warning, tag, __VA_ARGS__);
#define IK_LOG_ERROR(tag, ...)   ::KanViz::Logger::PrintMessage("Core", ::KanViz::LogLevel::Error, tag, __VA_ARGS__);
#define IK_LOG_CRITICAL(tag, ...)::KanViz::Logger::PrintMessage("Core", ::KanViz::LogLevel::Critical, tag, __VA_ARGS__);

#else

#define IK_LOG_TRACE(...)
#define IK_LOG_DEBUG(...)
#define IK_LOG_INFO(...)
#define IK_LOG_WARN(...)
#define IK_LOG_ERROR(...)
#define IK_LOG_CRITICAL(...)

#endif

