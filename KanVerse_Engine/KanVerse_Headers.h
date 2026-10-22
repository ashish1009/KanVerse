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

#include "Core/Time/TimeStep.hpp"
#include "Core/Time/Clock.hpp"

#include "Core/Math/Math.hpp"

// Audio
#include "Audio/AudioSystem.hpp"

// Renderer
#include "Renderer/Context/RenderContext.hpp"
