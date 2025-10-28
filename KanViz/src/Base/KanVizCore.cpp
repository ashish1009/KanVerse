//
//  KanVizCore.cpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#include "KanVizCore.hpp"

#include "Debug/Logger.hpp"
#include "Debug/LoggerSpecificationBuilder.hpp"

namespace KanViz
{
  bool CoreEngine::Initialize()
  {
    // Create the core logger
    const LoggerSpecification& loggerSpec = LoggerSpecification::Create().Type(LogType::Core).Level(LogLevel::Trace).Name("KanVest-Logger").SaveAt(LogFilePath).ShowOnConsole(true);
    if (!Logger::Create(loggerSpec))
    {
      return IK_FAILURE;
    }

    return IK_SUCCESS;
  }
  
  bool CoreEngine::Shutdown()
  {
    // Shutdown the core logger
    if (!Logger::Shutdown())
    {
      return IK_FAILURE;
    }

    return IK_SUCCESS;
  }
} // namespace KanViz
