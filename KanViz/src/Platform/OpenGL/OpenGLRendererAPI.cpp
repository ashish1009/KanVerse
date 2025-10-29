//
//  OpenGLRendererAPI.cpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#include "OpenGLRendererAPI.hpp"

#include <glad/glad.h>

namespace KanViz
{
  namespace RendererUtils
  {
    /// Convert Renderer GL Func to Open GL Func
    /// - Parameter func: Renderer Func enum
    static GLint FucToGlFunc(GlDepthFunc func)
    {
      switch (func)
      {
        case GlDepthFunc::Always : return GL_ALWAYS;
        case GlDepthFunc::Never : return GL_NEVER;
        case GlDepthFunc::Less : return GL_LESS;
        case GlDepthFunc::Equal : return GL_EQUAL;
        case GlDepthFunc::LEqual : return GL_LEQUAL;
        case GlDepthFunc::Greater : return GL_GREATER;
        case GlDepthFunc::LGreater : return GL_GEQUAL;
        case GlDepthFunc::NotEqual : return GL_NOTEQUAL;
        default:
          IK_ASSERT(false, "Invalid Type");
      }
    }
  }

  static std::string_view GetGLErrorString(GLenum error)
  {
    switch (error)
    {
      case GL_INVALID_OPERATION: return "Invalid operation";
      case GL_INVALID_ENUM: return "Invalid enum";
      case GL_INVALID_VALUE: return "Invalid value";
      case GL_OUT_OF_MEMORY: return "Out of memory";
        // Add other cases as needed
      default: return "Unknown error";
    }
  }
  
  OpenGLRendererAPI::OpenGLRendererAPI()
  {
    IK_PROFILE();
    IK_LOG_INFO(LogModule::Renderer, "Creating an OpenGL Renderer API instance.");
    
    // Enable depth testing and set depth function
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // For text rendering
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_MULTISAMPLE);
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  
  OpenGLRendererAPI::~OpenGLRendererAPI()
  {
    IK_PROFILE();
    IK_LOG_WARN(LogModule::Renderer, "Destroying the OpenGL Renderer API instance.");
  }
  
  void OpenGLRendererAPI::CheckError(std::string_view context) const
  {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
      IK_LOG_CRITICAL(LogModule::Renderer, "OpenGL Error in {0}: {1}", context, GetGLErrorString(error));
#if 0
      IK_ASSERT(false, "Open GL Error");`
#endif
    }
  }
  
  void OpenGLRendererAPI::SetClearColor(const glm::vec4& color) const
  {
    Renderer::Submit([color]() {
      glClearColor(color.r, color.g, color.b, color.a);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  void OpenGLRendererAPI::ClearBits() const
  {
    Renderer::Submit([]() {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  void OpenGLRendererAPI::ClearColorBits() const
  {
    Renderer::Submit([]() {
      glClear(GL_COLOR_BUFFER_BIT);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  void OpenGLRendererAPI::ClearDepthBits() const
  {
    Renderer::Submit([]() {
      glClear(GL_DEPTH_BUFFER_BIT);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  void OpenGLRendererAPI::ClearStencilBits() const
  {
    Renderer::Submit([]() {
      glClear(GL_STENCIL_BUFFER_BIT);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  void OpenGLRendererAPI::SetViewport(uint32_t width, uint32_t height) const
  {
    Renderer::Submit([width, height](){
      glViewport(0, 0, (GLsizei)width, (GLsizei)height);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  void OpenGLRendererAPI::EnableStencilPass() const
  {
    Renderer::Submit([](){
      glEnable(GL_STENCIL_TEST);
      glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
      glStencilMask(0x00);
      glDisable(GL_DEPTH_TEST);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  void OpenGLRendererAPI::DisableStencilPass() const
  {
    Renderer::Submit([](){
      glStencilMask(0xFF);
      glStencilFunc(GL_ALWAYS, 0, 0xFF);
      glEnable(GL_DEPTH_TEST);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  void OpenGLRendererAPI::DepthFunc(GlDepthFunc func) const
  {
    Renderer::Submit([func](){
      glDepthFunc(RendererUtils::FucToGlFunc(func));
      IK_CHECK_RENDERER_ERROR()
    });
  }
} // namespace KanViz
