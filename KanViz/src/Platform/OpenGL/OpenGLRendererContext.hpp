//
//  OpenGLRendererContext.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Renderer/Graphics/RendererContext.hpp"

namespace KanViz
{
  /// This class implements the OpenGL renderer context.
  /// It manages the rendering context for OpenGL using a GLFW window.
  class OpenGLRendererContext : public RendererContext
  {
  public:
    /// Constructs an OpenGL renderer context instance with the specified GLFW window.
    /// - Parameter windowPtr: Pointer to the GLFW window to associate with this context.
    /// - Note: Caller is responsible for the lifetime of windowPtr. It should remain valid for the lifetime of this context.
    OpenGLRendererContext(GLFWwindow* windowPtr);
    
    /// Destroys the OpenGL renderer context instance and releases associated resources.
    ~OpenGLRendererContext();
    
    /// Swaps the front and back buffers.
    /// - Note: This function should be called each frame to update the display and clear the buffers.
    void SwapBuffers() override;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLRendererContext);
    
  private:
    GLFWwindow* m_window;  //< Pointer to the associated GLFW window.
  };
} // namespace KanViz
