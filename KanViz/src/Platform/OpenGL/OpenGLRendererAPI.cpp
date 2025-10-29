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
} // namespace KanViz
