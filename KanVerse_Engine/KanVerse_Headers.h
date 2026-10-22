/*!
 * @file KanVerse_Headers.h
 * @brief KanVerse_Engine mendatory header files
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

// To remove documentation warning
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wformat-security"

// C++ Files
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <cmath>

// Core/Diagnostics
#include "Core/Diagnostics/Assert.hpp"
#include "Core/Diagnostics/Log.hpp"

// Core
#include "Core/Memory/MemoryTracking.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Time/TimeStep.hpp"
#include "Core/Layers/Layer.hpp"
#include "Core/Layers/LayerStack.hpp"
#include "Core/Application/Application.hpp"

// Audio
#include "Audio/AudioSystem.hpp"

// Platform
#include "Platform/Events/Event.hpp"
#include "Platform/Events/WindowEvents.hpp"
#include "Platform/Events/KeyEvents.hpp"
#include "Platform/Events/MouseEvents.hpp"
#include "Platform/Input/GameControllerBridge.hpp"
#include "Platform/Input/KeyCodes.hpp"

// Renderer
#include "Renderer/Renderer/Renderer.hpp"
#include "Renderer/Camera/CameraController.hpp"
#include "Renderer/Camera/SpringArm.hpp"
#include "Renderer/Framebuffer/Framebuffer.hpp"
#include "Renderer/Geometry/Mesh.hpp"
#include "Renderer/Materials/Material.hpp"
#include "Renderer/Pipeline/Pipeline.hpp"
#include "Renderer/Pipeline/Shader.hpp"
#include "Renderer/Textures/Texture2D.hpp"
#include "Renderer/Textures/TextureCube.hpp"
#include "Renderer/Asset/AssetManager.hpp"
