//
//  OpenGLVertexBuffer.cpp
//  KanViz
//
//  Created by Ashish . on 18/11/24.
//

#include "OpenGLVertexBuffer.hpp"

#include <glad/glad.h>

#include "Renderer/Core/RendererStats.hpp"

namespace KanViz
{
  OpenGLVertexBuffer::OpenGLVertexBuffer(void *data, uint32_t size)
  : m_size(size)
  {
    // Storing data in queue to call the command queue on by one and data is not lost
    // For static data, only one data to be present before render command queue
    m_dataQueue.push(Buffer::Copy(data, size));
    Renderer::Submit([this]() {
      IK_PROFILE();
      
      Buffer vertexData = m_dataQueue.front();
      if (!vertexData.data or vertexData.size == 0)
      {
        return;
      }
      
      glGenBuffers(1, &m_rendererID);

      glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
      glBufferData(GL_ARRAY_BUFFER, vertexData.size, vertexData.data, GL_STATIC_DRAW);
      
      RendererStatistics::Get().vertexBufferSize += vertexData.size;
      
      IK_LOG_DEBUG(LogModule::VertexBuffer, "Creating Open GL Vertex Buffer with Data.");
      IK_LOG_DEBUG(LogModule::VertexBuffer, "  Renderer ID      {0}", m_rendererID);
      IK_LOG_DEBUG(LogModule::VertexBuffer, "  Size             {0} B", vertexData.size);
      IK_LOG_DEBUG(LogModule::VertexBuffer, "  Total Size Used  {0} B", RendererStatistics::Get().vertexBufferSize);
      
      // Delete the data from queue once stored in GPU
      m_dataQueue.pop();
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
  : m_size(size)
  {
    Renderer::Submit([this]() {
      IK_PROFILE();
      if(m_size == 0)
      {
        return;
      }
      
      glGenBuffers(1, &m_rendererID);
      RendererStatistics::Get().vertexBufferSize += m_size;
      
      glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
      glBufferData(GL_ARRAY_BUFFER, m_size, nullptr, GL_DYNAMIC_DRAW);
      
      IK_LOG_DEBUG(LogModule::VertexBuffer, "Creating Open GL Vertex Buffer without Data.");
      IK_LOG_DEBUG(LogModule::VertexBuffer, "  Renderer ID      {0}", m_rendererID);
      IK_LOG_DEBUG(LogModule::VertexBuffer, "  Size             {0} B", m_size);
      IK_LOG_DEBUG(LogModule::VertexBuffer, "  Total Size Used  {0} B", RendererStatistics::Get().vertexBufferSize);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  OpenGLVertexBuffer::~OpenGLVertexBuffer()
  {
    IK_PROFILE();
    RendererStatistics::Get().vertexBufferSize -= m_size;
    IK_LOG_DEBUG(LogModule::VertexBuffer, "Destroying Open GL Vertex Buffer.");
    IK_LOG_DEBUG(LogModule::VertexBuffer, "  Renderer ID      {0}", m_rendererID);
    IK_LOG_DEBUG(LogModule::VertexBuffer, "  Size             {0} B", m_size);
    IK_LOG_DEBUG(LogModule::VertexBuffer, "  Total Size Left  {0} B", RendererStatistics::Get().vertexBufferSize);
    
    Renderer::Submit([this]() {
      glDeleteBuffers(1, &m_rendererID);
    });
  }
  
  void OpenGLVertexBuffer::SetData(void* data, uint32_t size, uint32_t offset)
  {
    // Storing data in queue to call the command queue on by one and data is not lost
    // For static data, only one data to be present before render command queue
    m_dataQueue.push(Buffer::Copy(data, size));
    m_size = size;
    Renderer::Submit([this, offset]() {
      Buffer vertexData = m_dataQueue.front();
      if (!vertexData.data or (vertexData.size + offset) > vertexData.size)
      {
        return;
      }

      glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
      glBufferSubData(GL_ARRAY_BUFFER, offset, vertexData.size, vertexData.data);
      m_dataQueue.pop();
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  void OpenGLVertexBuffer::Bind() const
  {
    Renderer::Submit([this]() {
      glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  void OpenGLVertexBuffer::Unbind() const
  {
    Renderer::Submit([]() {
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  RendererID OpenGLVertexBuffer::GetRendererID() const
  {
    return m_rendererID;
  }
  
  uint32_t OpenGLVertexBuffer::GetSize() const
  {
    return m_size;
  }

} // namespace KanViz
