//
//  VertexBuffer.hpp
//  KanViz
//
//  Created by Ashish . on 04/11/24.
//

#pragma once

#include "Renderer/Core/Renderer.hpp"
#include "Renderer/Graphics/Factories.hpp"

namespace KanViz
{
  /// This is the interface class for storing graphics vertex buffer
  class VertexBuffer
  {
  public:
    /// Default virtual destructor
    virtual ~VertexBuffer() = default;
    
    /// This function updates the date in buffer dynamically
    /// - Parameters:
    ///   - data: Data pointer to be stored in GPU
    ///   - size: size of data
    ///   - offset: offset of data
    virtual void SetData(void* data, uint32_t size, uint32_t offset = 0) = 0;
    
    /// This function binds the Vertex Buffer before rendering
    virtual void Bind() const = 0;
    /// This function unbinds the Vertex Buffer after rendering
    virtual void Unbind() const = 0;
    
    /// This function returns the size of Vertex Buffer in GPU
    virtual uint32_t GetSize() const = 0;
    /// This function returns the renderer ID of Vertex Buffer
    virtual RendererID GetRendererID() const = 0;

  };
} // namespace KanViz
