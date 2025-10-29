//
//  RendererContext.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Renderer/Graphics/Factories.hpp"

namespace KanViz
{
  /// This is the interface class to create renderer context instance
  class RendererContext
  {
  public:
    /// Default virtual destructor for proper cleanup of derived classes.
    virtual ~RendererContext() = default;
    /// This function swaps the front and back buffers to display the rendered frame.
    /// - Note: This should be called at the end of each frame to update the display.
    virtual void SwapBuffers() = 0;
    
    /// This function resizes the framebuffer
    /// - Parameters:
    ///   - framebufferWidth: framebuffer width
    ///   - framebufferHeight: framebuffer height
    virtual void ResizeFramebuffer(int32_t framebufferWidth, int32_t framebufferHeight) const = 0;
    
    /// This function returns the device instance
    virtual void* GetDevice() const noexcept = 0;
    /// This function returns the layer instance
    virtual void* GetLayer() const noexcept = 0;
    /// This function returns the command queue instance
    virtual void* GetCommandQueue() const noexcept = 0;
    /// This function returns the command buffer instance
    virtual void* GetCommandBuffer() const noexcept = 0;
    /// This function returns the drawable instance
    virtual void* GetDrawable() const noexcept = 0;
    /// This function returns the Renderer pass descriptor instance
    virtual void* GetImGuiPassDescriptor() const noexcept = 0;
  };
} // namespace KanViz
