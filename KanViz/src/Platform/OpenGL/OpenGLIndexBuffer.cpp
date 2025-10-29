//
//  OpenGLIndexBuffer.cpp
//  KanViz
//
//  Created by Ashish . on 25/11/24.
//

#include "OpenGLIndexBuffer.hpp"

#include <glad/glad.h>

#include "Renderer/Core/RendererStats.hpp"

namespace KanViz
{
  OpenGLIndexBuffer::OpenGLIndexBuffer(void *data, uint32_t size)
  : m_size(size)
  {
    Buffer localData = Buffer::Copy(data, size);
    Renderer::Submit([this, localData](){
      IK_PROFILE();
      if (!localData.data or m_size == 0)
      {
        return;
      }
      
      glGenBuffers(1, &m_rendererID);
      RendererStatistics::Get().indexBufferSize += m_size;
      
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_size, localData.data, GL_STATIC_DRAW);
      
      IK_LOG_DEBUG(LogModule::IndexBuffer, "Creating Open GL Index Buffer.");
      IK_LOG_DEBUG(LogModule::IndexBuffer, "  Renderer ID            {0}", m_rendererID);
      IK_LOG_DEBUG(LogModule::IndexBuffer, "  Size of single Indices {0} B", SizeOfSingleIndices);
      IK_LOG_DEBUG(LogModule::IndexBuffer, "  Size                   {0} B", m_size);
      IK_LOG_DEBUG(LogModule::IndexBuffer, "  Total Size Used        {0} B", RendererStatistics::Get().indexBufferSize);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  OpenGLIndexBuffer::~OpenGLIndexBuffer()
  {
    Renderer::Submit([this](){
      IK_PROFILE();
      RendererStatistics::Get().indexBufferSize -= m_size;
      glDeleteBuffers(1, &m_rendererID);
      
      IK_LOG_DEBUG(LogModule::IndexBuffer, "Destroying Open GL Index Buffer.");
      IK_LOG_DEBUG(LogModule::IndexBuffer, "  Renderer ID            {0}", m_rendererID);
      IK_LOG_DEBUG(LogModule::IndexBuffer, "  Size of single Indices {0} B", SizeOfSingleIndices);
      IK_LOG_DEBUG(LogModule::IndexBuffer, "  Size                   {0} B", m_size);
      IK_LOG_DEBUG(LogModule::IndexBuffer, "  Total Size Left        {0} B", RendererStatistics::Get().indexBufferSize);
    });
  }
  
  void OpenGLIndexBuffer::Bind() const
  {
    Renderer::Submit([this](){
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  void OpenGLIndexBuffer::Unbind() const
  {
    Renderer::Submit([](){
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  RendererID OpenGLIndexBuffer::GetRendererID() const
  {
    return m_rendererID;
  }
  
  uint32_t OpenGLIndexBuffer::GetSize() const
  {
    return m_size;
  }
  
  uint32_t OpenGLIndexBuffer::GetCount() const
  {
    return m_size / SizeOfSingleIndices;
  }
} // namespace KanViz
