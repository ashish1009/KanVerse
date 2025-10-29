//
//  OpenGLRendererContext.cpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#include "OpenGLRendererContext.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace KanViz
{
  OpenGLRendererContext::OpenGLRendererContext(GLFWwindow* windowPtr)
  : m_window(windowPtr)
  {
    IK_PROFILE();
    
    IK_ASSERT(m_window, "Window pointer cannot be null!");
    IK_LOG_INFO(LogModule::Renderer, "Creating Open GL Renderer Context.");
    
    // Create Renderer Context for GLFW Window
    // NOTE: This function makes the OpenGL or OpenGL ES context of the specified window on the calling thread.
    // - A context must only be made current on a single thread at a time.
    // - Each thread can have only a single context at a time.
    glfwMakeContextCurrent(m_window);
    if (glfwGetCurrentContext() != m_window)
    {
      IK_ASSERT(false, "Failed to make context current!");
    }
    
    // We pass GLAD the function to load the address of the OpenGL function pointers which is OS-specific. GLFW gives us glfwGetProcAddress that
    // defines the correct function based on which OS we're compiling for
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
      IK_ASSERT(false, "Failed to initialize GLAD.");
    }
    
    int major, minor;
    
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    
    IK_LOG_INFO(LogModule::Renderer, "OpenGL Version Detected: {}.{}", major, minor);
    IK_ASSERT(major >= 4 && minor >= 1, "OpenGL 4.1 or higher required!");
    
    IK_LOG_INFO(LogModule::Renderer, "OpenGL context initialized successfully.");
  }
  
  OpenGLRendererContext::~OpenGLRendererContext()
  {
    IK_PROFILE();
    IK_LOG_WARN(LogModule::Renderer, "Destroying Open GL Renderer Context.");
  }
  
  void OpenGLRendererContext::SwapBuffers()
  {
    IK_PERFORMANCE_FUNC("OpenGLRendererContext::SwapBuffers");
    glfwSwapBuffers(m_window);
  }
} // namespace KanViz
