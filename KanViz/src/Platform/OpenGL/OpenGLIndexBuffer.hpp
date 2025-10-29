//
//  OpenGLIndexBuffer.hpp
//  KanViz
//
//  Created by Ashish . on 25/11/24.
//

#pragma once

#include "Renderer/Graphics/IndexBuffer.hpp"

namespace KanViz 
{
  /// This class implements the APIs for creating and storing index buffer data.
  class OpenGLIndexBuffer : public IndexBuffer
  {
  public:
    /// This Constructor create the Index Buffer with Indices data and its size.
    /// - Parameters:
    ///   - data: data to be filled in index buffer.
    ///   - size: size of data in index buffer.
    OpenGLIndexBuffer(void *data, uint32_t size);
    /// This destructor destroyes the Open GL idnex Buffer.
    ~OpenGLIndexBuffer();
    
    /// This function binds the Vertex Buffer before rendering.
    void Bind() const override;
    /// This function unbinds the Vertex Buffer after rendering.
    void Unbind() const override;
    
    /// This function returns the Number of Indices used by this Index Buffer.
    uint32_t GetCount() const override;
    /// This function returns the size of Index Buffer in GPU.
    uint32_t GetSize() const override;
    /// This function returns the renderer ID of Index Buffer.
    RendererID GetRendererID() const override;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLIndexBuffer);
    
  private:
    RendererID m_rendererID {0};
    uint32_t m_size {0};
  };
} // namespace KanViz
