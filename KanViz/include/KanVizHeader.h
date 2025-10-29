//
//  KanVizHeader.h
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#pragma once

// To remove documentation warning in entt::entity
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wformat-security"

// C++ Files
#include <iostream>
#include <filesystem>
#include <map>

// glm math library
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/noise.hpp>

// Engine Files
#include <Base/Configuration.h>
#include <Base/KanVizCore.hpp>
#include <Base/AssertAPI.h>
#include <Base/DesignHelper.h>
#include <Base/Buffer.hpp>

#include <Debug/Logger.hpp>
#include <Debug/LoggerAPI.h>
#include <Debug/LoggerSpecificationBuilder.hpp>

#include <Debug/Profiler/Timer.hpp>
#include <Debug/Profiler/Profiler.hpp>
#include <Debug/Profiler/ScopedProfiler.hpp>
#include <Debug/Profiler/PerformanceProfiler.hpp>

#include <Core/Application.hpp>
#include <Core/ApplicationFactory.hpp>
#include <Core/TimeStep.hpp>
#include <Core/Layer.hpp>

#include <Core/Event/Events.h>
#include <Core/Event/ApplicationEvents.h>
#include <Core/Event/MouseEvents.h>
#include <Core/Event/KeyboardEvents.h>

#include <Renderer/Core/RendererType.hpp>
