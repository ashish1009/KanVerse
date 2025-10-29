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
  };
} // namespace KanViz
