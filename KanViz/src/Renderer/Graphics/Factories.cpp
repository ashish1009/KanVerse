//
//  Factories.cpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#include "Factories.hpp"

#include "Renderer/Core/Renderer.hpp"

#include "Platform/OpenGL/OpenGLRendererContext.hpp"
#include "Platform/OpenGL/OpenGLRendererAPI.hpp"

namespace KanViz
{
  /// Validates the current renderer API type.
  /// - Note: Asserts if the renderer API is invalid and logs critical errors.
  static void ValidateRendererAPI()
  {
    IK_LOG_CRITICAL(LogModule::Renderer, "Renderer API Type is not set or is set as invalid");
    IK_LOG_CRITICAL(LogModule::Renderer, "Call Renderer::SetCurrentRendererAPI(RendererType) before any renderer initialization to set Renderer API type");
    IK_LOG_CRITICAL(LogModule::Renderer, "'RendererType should not be RendererType::Invalid");
    
    IK_ASSERT(false, "Renderer API type is invalid; it must be set before initialization.");
  }
  
  Scope<RendererContext> RendererContextFactory::Create(GLFWwindow* windowPtr)
  {
    switch(Renderer::GetCurrentRendererAPI())
    {
      case RendererType::OpenGL:
        return CreateScope<OpenGLRendererContext>(windowPtr);
      case RendererType::Invalid:
      default:
        ValidateRendererAPI();
        return nullptr; // Adding for Release mode code where assert MACRO is not defined
    }
  }
  
  Scope<RendererAPI> RendererAPIFactory::Create()
  {
    switch (Renderer::GetCurrentRendererAPI())
    {
      case RendererType::OpenGL:
        return CreateScope<OpenGLRendererAPI>();
      case RendererType::Invalid:
      default:
        ValidateRendererAPI();
        return nullptr; // Adding for Release mode code where assert MACRO is not defined
    }
  }
} // namespace KanViz
