//
//  OpenGLFrameBuffer.cpp
//  KanViz
//
//  Created by Ashish . on 24/12/24.
//

#include "OpenGLFrameBuffer.hpp"

#include <glad/glad.h>

#include "Platform/OpenGL/OpenGLTexture.hpp"
#include "Platform/OpenGL/OpenGLSpecification.hpp"

namespace KanViz
{
#define FB_LOG(...) IK_LOG_DEBUG(LogModule::FrameBuffer, __VA_ARGS__);
  
  static GLint s_rendererViewport[4];
  
  OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
  : m_specification(spec)
  {
    IK_PROFILE();
    FB_LOG("Creating Open GL Framebuffer.");
    
    // Store all the attachment in the member variables.
    for (const FrameBufferAttachments::TextureFormat& attchmentFormat : m_specification.attachments.textureFormats)
    {
      if (FramebufferUtils::IsDepthFormat(attchmentFormat))
      {
        m_depthSpecification = attchmentFormat;
      }
      else
      {
        m_colorSpecifications.emplace_back(attchmentFormat);
      }
    }
    
    // Invalide the framebuffer/
    Invalidate(false);
  }
  
  OpenGLFrameBuffer::~OpenGLFrameBuffer()
  {
    IK_PROFILE();
    FB_LOG("Destroying Open GL Framebuffer with Renderer ID {0}.", m_rendererID);
    
    // Destroy the framebuffer and its attachments.
    Destroy();
  }
  
  void OpenGLFrameBuffer::Invalidate(bool debugLogs)
  {
    IK_PROFILE();
    if (debugLogs)
    {
      FB_LOG("Invalidating Open GL Framebuffer : {0}", m_specification.debugName);
    }
    
    // Delete the framebuffers if already created.
    if (m_rendererID)
    {
      Destroy();
    }
    
    // Generate the frame buffer to renderer ID.
    Renderer::Submit([this](){
      glGenFramebuffers(1, &m_rendererID);
      glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
      IK_CHECK_RENDERER_ERROR()
    });
    
    // Create texture specification for attachments.
    TextureSpecification spec;
    spec.title  = m_specification.debugName;
    spec.width  = m_specification.width;
    spec.height = m_specification.height;
    
    // Debug logs for texture.
#ifdef IK_DEBUG
    spec.enableDebugLogs = debugLogs;
#endif
    
    // Color Attachments ----------
    if (m_colorSpecifications.size())
    {
      m_colorAttachments.resize(m_colorSpecifications.size());
      
      // For each color attachment
      for (size_t i = 0; i < m_colorSpecifications.size(); i++)
      {
        m_colorAttachments[i] = FramebufferUtils::CreateAttachment(m_colorSpecifications[i], spec, debugLogs);
        m_colorAttachments[i]->AttachToFramebuffer(TextureAttachment::Color, (uint32_t)i);
      }
    } // Color Attachments
    
    // Depth Attachment -----------
    if (FrameBufferAttachments::TextureFormat::None != m_depthSpecification)
    {
      m_depthAttachment = FramebufferUtils::CreateAttachment(m_depthSpecification, spec, debugLogs);
      m_depthAttachment->AttachToFramebuffer(TextureAttachment::Depth);
    } // Depth Attachment
    
    Renderer::Submit([this]() {
      // Error check
      if (m_colorAttachments.size() >= 1)
      {
        IK_ASSERT((m_colorAttachments.size() <= 4), "Inalid Attachment!");
        static GLenum buffers[4] =
        {
          GL_COLOR_ATTACHMENT0,
          GL_COLOR_ATTACHMENT1,
          GL_COLOR_ATTACHMENT2,
          GL_COLOR_ATTACHMENT3
        };
        
        glDrawBuffers((GLsizei)m_colorAttachments.size(), buffers);
        IK_ASSERT((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE), "FrameBuffer is Incomplete!");
      }
      else if (m_colorSpecifications.empty() and FrameBufferAttachments::TextureFormat::None != m_depthSpecification)
      {
        // Only depth-pass
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        
        static float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        
        IK_ASSERT((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE), "FrameBuffer is Incomplete!");
      }
      else
      {
        FB_LOG("Framebuffer created without attachment");
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  void OpenGLFrameBuffer::Bind() const
  {
    Renderer::Submit([this](){
      glGetIntegerv(GL_VIEWPORT, s_rendererViewport);
      glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
      IK_CHECK_RENDERER_ERROR()
    });
    Renderer::SetViewport(m_specification.width, m_specification.height);
  }
  
  void OpenGLFrameBuffer::Unbind() const
  {
    Renderer::Submit([](){
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glViewport(s_rendererViewport[0], s_rendererViewport[1], (GLsizei)s_rendererViewport[2], (GLsizei)s_rendererViewport[3]);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  void OpenGLFrameBuffer::Resize(const uint32_t width, const uint32_t height)
  {
    IK_PROFILE();
    if (0 == width or 0 == height or (width == m_specification.width and height == m_specification.height))
    {
      return;
    }
    
    Renderer::Submit([](){
      glGetIntegerv(GL_VIEWPORT, s_rendererViewport);
      IK_CHECK_RENDERER_ERROR()
    });
    
    m_specification.width  = width;
    m_specification.height = height;
    
#if 0
    FB_LOG("Resizing the Framebuffer  : {0}", m_specification.debugName);
    FB_LOG("  Size                    : {0} x {1}", m_specification.width, m_specification.height);
#endif
    Invalidate(false);
    
    Renderer::SetViewport(m_specification.width, m_specification.height);
    
    Renderer::Submit([](){
      glViewport(s_rendererViewport[0], s_rendererViewport[1], (GLsizei)s_rendererViewport[2], (GLsizei)s_rendererViewport[3]);
      IK_CHECK_RENDERER_ERROR()
    });
  }
  
  void OpenGLFrameBuffer::Destroy()
  {
    // Clear the depth and color attachments.
    m_depthAttachment.reset();
    for (Ref<Texture> ca : m_colorAttachments)
    {
      ca.reset();
    }
    m_colorAttachments.clear();
    
    // Delete the framebuffer.
    Renderer::Submit([this](){
      glDeleteFramebuffers(1, &m_rendererID);
    });
  }
  
  const FrameBufferSpecification& OpenGLFrameBuffer::GetSpecification() const
  {
    return m_specification;
  }
  
  RendererID OpenGLFrameBuffer::GetRendererId() const
  {
    return m_rendererID;
  }
  
  Ref<Texture> OpenGLFrameBuffer::GetDepthAttachment() const
  {
    return m_depthAttachment;
  }
  
  const std::vector<Ref<Texture>>& OpenGLFrameBuffer::GetColorAttachments() const
  {
    return m_colorAttachments;
  }
} // namespace KanViz
