/*!
 * @file Renderer.mm
 * @brief Metal implementation of the KanVerse Renderer with opaque, transparent, shadow, IBL, and point light support.
 * @copyright KanVerse Engine
 */

#include "Renderer.hpp"
#include "Renderer/Renderer/RenderCommand.hpp"
#include "Core/Diagnostics/Log.hpp"
#include "Core/Diagnostics/Assert.hpp"
#include "Core/Application/Application.hpp"
#include "Platform/Window/NativeWindow.hpp"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#import <MetalKit/MetalKit.h>

namespace KanVerse {
  std::unique_ptr<Renderer::RendererStorage> Renderer::s_Storage = nullptr;

  void Renderer::Init() {
    
  }
  
  void Renderer::Shutdown() {
    
  }
} // namespace KanVerse
