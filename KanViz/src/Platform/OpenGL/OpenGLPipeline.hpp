//
//  OpenGLPipeline.hpp
//  KanViz
//
//  Created by Ashish . on 25/11/24.
//

#pragma once

#include "Renderer/Graphics/Pipeline.hpp"

namespace KanViz
{
  /// This class implements the APIs for creating and storing pipeline data.
  class OpenGLPipeline : public Pipeline
  {
  public:
    /// This consturctor creates the pipeline with the specification.
    /// - Parameter spec: Pipeline specification.
    OpenGLPipeline(const PipelineSpecification& spec);
    /// This destructior destroys the pipeline data.
    ~OpenGLPipeline();
    
    /// This function binds the current Pipeline to the renderer.
    void Bind() const override;
    /// This function unbinds the current Pipeline from the renderer.
    void Unbind() const override;
    
    /// This function adds the attribute for instancing
    void AddInstaceAttribute() const override;

    /// This function returns the renderer ID of Pipeline.
    RendererID GetRendererID() const override;
    
    /// This function returns the const pipeline specification.
    const PipelineSpecification& GetSpecification() const override;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLPipeline);
    
  private:
    RendererID m_rendererID {0};
    PipelineSpecification m_specification;
  };
} // namespace KanViz
