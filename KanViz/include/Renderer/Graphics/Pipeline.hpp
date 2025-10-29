//
//  Pipeline.hpp
//  KanViz
//
//  Created by Ashish . on 04/11/24.
//

#pragma once

#include "Renderer/Core/Renderer.hpp"
#include "Renderer/Graphics/Factories.hpp"

namespace KanViz
{
  /// This class is the interface for creating pipeline for renderer
  class Pipeline
  {
  public:
    /// Default virtual destructor
    virtual ~Pipeline() = default;
    
    /// This function binds the current Pipeline to the renderer
    virtual void Bind() const = 0;
    /// This function unbinds the current Pipeline from the renderer
    virtual void Unbind() const = 0;
    
    /// This function adds the attribute for instancing
    virtual void AddInstaceAttribute() const = 0;

    /// This function returns the renderer ID of Vertex Buffer
    virtual RendererID GetRendererID() const = 0;
    
    /// This function returns the const specification
    virtual const PipelineSpecification& GetSpecification() const = 0;
  };
} // namespace KanViz
