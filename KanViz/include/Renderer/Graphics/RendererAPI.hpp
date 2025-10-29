//
//  RendererAPI.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Renderer/Graphics/Factories.hpp"

namespace KanViz
{
  /// This class is the interface for all the renderer APIs.
  /// It defines the core functions that any rendering backend must implement.
  class RendererAPI
  {
  public:
    /// Default virtual destructor for proper cleanup of derived classes.
    virtual ~RendererAPI() = default;
    
    /// This function checks the renderer API error
    /// - Parameter context: Error message to be printed
    virtual void CheckError(std::string_view context) const = 0;
    
    /// This function sets the clear color and clears all renderer bits.
    /// - Parameter color: The new clear color of the screen.
    virtual void SetClearColor(const glm::vec4& color) const = 0;
    /// This function clear all the bits of renderer screen
    virtual void ClearBits() const = 0;
    /// This function clears the color bits
    virtual void ClearColorBits() const = 0;
    /// This function clears the depth bits
    virtual void ClearDepthBits() const = 0;
    /// This function clears the stencil bits
    virtual void ClearStencilBits() const = 0;
    
    /// This function update the renderer viewport size
    /// - Parameters:
    ///   - width: new width of renderer viewport
    ///   - height: new height of renderer viewport
    virtual void SetViewport(uint32_t width, uint32_t height) const = 0;
    
    /// This function enables the stencil pass
    virtual void EnableStencilPass() const = 0;
    /// This function disables the stencil pass
    virtual void DisableStencilPass() const = 0;
    /// This function change depth function
    /// - Parameter func: depth function type
    virtual void DepthFunc(GlDepthFunc func) const = 0;
  };
} // namespace KanViz
