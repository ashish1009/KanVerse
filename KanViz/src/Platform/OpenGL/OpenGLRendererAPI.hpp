//
//  OpenGLRendererAPI.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Renderer/Core/Renderer.hpp"

#include "Renderer/Graphics/RendererAPI.hpp"

namespace KanViz
{
  /// This class implements the Renderer APIs for OpenGL.
  class OpenGLRendererAPI : public RendererAPI
  {
  public:
    /// This constructor creates the instance of Renderer API.
    OpenGLRendererAPI();
    /// This destructor destroys the Renderer API instance.
    virtual ~OpenGLRendererAPI();
    
    /// This function checks for errors in the renderer API.
    /// - Parameter context: Error message to be printed
    void CheckError(std::string_view context) const override;
    
    /// This function clears all the renderer bits and sets the clear color as 'color'.
    /// - Parameter color: The new clear color of the screen.
    void SetClearColor(const glm::vec4& color) const override;
    /// Clears all the buffers (color, depth, and stencil).
    void ClearBits() const override;
    /// This function clear the color bits of renderer screen.
    void ClearColorBits() const override;
    /// This function clears the depth bits.
    void ClearDepthBits() const override;
    /// This function clears the stencil bits.
    void ClearStencilBits() const override;
    
    /// This function update the renderer viewport size.
    /// - Parameters:
    ///   - width: new width of renderer viewport.
    ///   - height: new height of renderer viewport.
    void SetViewport(uint32_t width, uint32_t height) const override;
    
    /// This function enables the stencil pass
    void EnableStencilPass() const override;
    /// This function disables the stencil pass
    void DisableStencilPass() const override;
    /// This function change depth function
    /// - Parameter func: depth function type
    void DepthFunc(GlDepthFunc func) const override;

  };
} // namespace KanViz
