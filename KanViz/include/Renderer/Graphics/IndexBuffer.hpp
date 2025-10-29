//
//  IndexBuffer.hpp
//  KanViz
//
//  Created by Ashish . on 04/11/24.
//

#pragma once

#include "Renderer/Core/Renderer.hpp"
#include "Renderer/Graphics/Factories.hpp"

namespace KanViz
{
  static constexpr uint32_t SizeOfSingleIndices = sizeof(uint32_t);

  /// This is the interface class for storing graphics index buffer
  class IndexBuffer
  {
  public:
    /// Default virtual destructor
    virtual ~IndexBuffer() = default;
    
    /// This function binds the Vertex Buffer before rendering
    virtual void Bind() const = 0;
    /// This function unbinds the Vertex Buffer after rendering
    virtual void Unbind() const = 0;
    
    /// This function returns the Number of Indices used by this Index Buffer
    virtual uint32_t GetCount() const = 0;
    /// This function returns the size of Index Buffer in GPU
    virtual uint32_t GetSize() const = 0;
    /// This function returns the renderer ID of Index Buffer
    virtual RendererID GetRendererID() const = 0;
  };
} // namespace KanViz
