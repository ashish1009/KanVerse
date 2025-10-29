//
//  OpenGLVertexBuffer.hpp
//  KanViz
//
//  Created by Ashish . on 18/11/24.
//


#include "Renderer/Graphics/VertexBuffer.hpp"

namespace KanViz
{
  /// This is the Open GL implementation class for storing graphics texture
  class OpenGLVertexBuffer : public VertexBuffer
  {
  public:
    /// This Constructor creates the vertex buffer. Buffer data is passed and stored as static in GPU.
    /// - Parameters:
    ///   - data: Data of vertex Buffer
    ///   - size: Size of vertex Buffer
    OpenGLVertexBuffer(void* data, uint32_t size);
    /// This Constructor creates the vertex buffer. Buffer size is passed to reserve the memory in GPU. Data to passed.
    /// later dynamically.
    /// - Parameter size: Size of vertex buffer.
    OpenGLVertexBuffer(uint32_t size);
    
    /// This destructor destroyes the Open GL Vertex Buffer.
    ~OpenGLVertexBuffer();

    /// This function updates the date in buffer dynamically.
    /// - Parameters:
    ///   - data: Data pointer to be stored in GPU
    ///   - size: size of data
    ///   - offset: data offset
    void SetData(void* data, uint32_t size, uint32_t offset = 0) override;
    /// This function binds the Vertex Buffer before rendering.
    void Bind() const override;
    /// This function unbinds the Vertex Buffer after rendering.
    void Unbind() const override;
    
    /// This function returns the renderer ID of Vertex Buffer.
    RendererID GetRendererID() const override;
    /// This function returns the size of Vertex Buffer in GPU.
    uint32_t GetSize() const override;

    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLVertexBuffer);
    
  private:
    RendererID m_rendererID {0};
    uint32_t m_size {0};
    std::queue<Buffer> m_dataQueue;
  };
} // namespace KanViz
