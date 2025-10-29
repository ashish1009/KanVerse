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
    
    /// This function resizes the framebuffer
    /// - Parameters:
    ///   - framebufferWidth: framebuffer width
    ///   - framebufferHeight: framebuffer height
    void ResizeFramebuffer(int32_t framebufferWidth, int32_t framebufferHeight) const override;
    
    /// This function returns the device instance
    void* GetDevice() const noexcept override;
    /// This function returns the layer instance
    void* GetLayer() const noexcept override;
    /// This function returns the command queue instance
    void* GetCommandQueue() const noexcept override;
    /// This function returns the command buffer instance
    void* GetCommandBuffer() const noexcept override;
    /// This function returns the drawable instance
    void* GetDrawable() const noexcept override;
    /// This function returns the Renderer pass descriptor instance
    void* GetImGuiPassDescriptor() const noexcept override;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLRendererContext);
    
  private:
    GLFWwindow* m_window;  //< Pointer to the associated GLFW window.
  };
} // namespace KanViz
