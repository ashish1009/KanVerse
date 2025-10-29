//
//  FrameBuffer.hpp
//  KanViz
//
//  Created by Ashish . on 04/11/24.
//

#pragma once

#include "Renderer/Core/Renderer.hpp"
#include "Renderer/Graphics/Factories.hpp"

namespace KanViz
{
  /// This class is the interface for storing Framebuffer
  class FrameBuffer
  {
  public:
    /// Default virtual destructor
    virtual ~FrameBuffer() = default;
    
    /// This function binds the current FrameBuffer to the renderer
    virtual void Bind() const = 0;
    /// This function unbinds the current FrameBuffer from the renderer
    virtual void Unbind() const = 0;
    /// This function resizes the Framebuffer
    /// - Parameters:
    ///   - width: New width of FB
    ///   - height: New height of FB
    virtual void Resize(const uint32_t width, const uint32_t height) = 0;
    
    /// This function returns the Frame buffer specification
    virtual const FrameBufferSpecification& GetSpecification() const = 0;
    /// This function returns the Renderer ID
    virtual RendererID GetRendererId() const = 0;
    /// This function returns the Depth Attachment ID
    virtual Ref<Texture> GetDepthAttachment() const = 0;
    /// This function returns the Color Attachment iDs
    virtual const std::vector<Ref<Texture>>& GetColorAttachments() const = 0;
  };
} // namespace KanViz
