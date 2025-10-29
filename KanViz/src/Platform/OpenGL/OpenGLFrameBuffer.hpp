//
//  OpenGLFrameBuffer.hpp
//  KanViz
//
//  Created by Ashish . on 24/12/24.
//

#pragma once

#include "Renderer/Graphics/FrameBuffer.hpp"

namespace KanViz
{
  /// This class implements the APIs to create and store Framebuffers for Open GL.
  class OpenGLFrameBuffer : public FrameBuffer
  {
  public:
    /// This consturctor creates the frame buffer with the specification.
    /// - Parameter spec: Framebuffer specification.
    OpenGLFrameBuffer(const FrameBufferSpecification& spec);
    /// This destructor destroyes and deletes the framebuffer.
    ~OpenGLFrameBuffer();
    
    /// This function binds the current FrameBuffer to the renderer.
    void Bind() const override;
    /// This function unbinds the current FrameBuffer from the renderer.
    void Unbind() const override;
    
    /// This function resizes the Framebuffer.
    /// - parameters:
    ///   - width: New width of Framebuffer.
    ///   - height: New height of Framebuffer.
    void Resize(const uint32_t width, const uint32_t height) override;
    
    /// This function returns the Frame buffer specification.
    const FrameBufferSpecification& GetSpecification() const override;
    /// This function returns the Renderer ID.
    RendererID GetRendererId() const override;
    /// This function returns the Depth Attachment.
    Ref<Texture> GetDepthAttachment() const override;
    /// This function returns the Color Attachments.
    const std::vector<Ref<Texture>>& GetColorAttachments() const override;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLFrameBuffer);
    
  private:
    // Member Function -----------------------------------------------------------------------------------------------
    /// This function invalidates the frame buffer on creation and on resize
    /// - Parameter debugLogs : flag to shows the resize texture logs
    void Invalidate(bool debugLogs = true);
    /// This function destroys the framebuffer and its attachments.
    void Destroy();
    
    // Member Variables ----------------------------------------------------------------------------------------------
    RendererID m_rendererID{0};
    FrameBufferSpecification m_specification;
    
    // Stores the attachment format of Framebuffer
    std::vector<FrameBufferAttachments::TextureFormat> m_colorSpecifications;
    FrameBufferAttachments::TextureFormat m_depthSpecification = FrameBufferAttachments::TextureFormat::None;
    
    // Stores the attachment texture for both colors and depth
    std::vector<Ref<Texture>> m_colorAttachments;
    Ref<Texture> m_depthAttachment{0};
  };
} // namespace KanViz
