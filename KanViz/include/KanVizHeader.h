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
#include <queue>

// glm math library
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/noise.hpp>

// UI Vendor files
#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

// Engine Files
#include <Base/Configuration.h>
#include <Base/KanVizCore.hpp>
#include <Base/AssertAPI.h>
#include <Base/DesignHelper.h>
#include <Base/Buffer.hpp>

#include <Utils/StringUtils.hpp>
#include <Utils/FileSystemUtils.hpp>
#include <Utils/MathUtils.hpp>
#include <Utils/YAMLUtils.hpp>

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
#include <Renderer/Core/Renderer.hpp>
#include <Renderer/Core/RendererCommandQueue.hpp>
#include <Renderer/Core/RendererStats.hpp>

#include <Renderer/Graphics/Specifications.h>
#include <Renderer/Graphics/Factories.hpp>
#include <Renderer/Graphics/VertexBuffer.hpp>
#include <Renderer/Graphics/IndexBuffer.hpp>
#include <Renderer/Graphics/Shader.hpp>
#include <Renderer/Graphics/ShaderLibrary.hpp>
#include <Renderer/Graphics/Texture.hpp>
#include <Renderer/Graphics/Pipeline.hpp>
#include <Renderer/Graphics/FrameBuffer.hpp>
