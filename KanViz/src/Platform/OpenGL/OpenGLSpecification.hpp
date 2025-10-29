//
//  OpenGLSpecification.hpp
//  KanViz
//
//  Created by Ashish . on 11/11/24.
//

#pragma once

#include <glad/glad.h>

#include "Renderer/Core/Renderer.hpp"
#include "Renderer/Graphics/Texture.hpp"
#include "Renderer/Graphics/Specifications.h"

namespace KanViz
{
  // Texture utils ---------------------------------------------------------------
  namespace TextureUtils
  {
    /// This function converts the KanViz texture type with GL texture type
    /// - Parameter type: KanViz texture type
    GLenum OpenGLTypeFromKanVizType(TextureType type);
    
    /// This function converts the KanViz texture format with GL texture format
    /// - Parameter format: KanViz texture format
    GLenum OpenGLFormatFromKanVizFormat(TextureFormat format);
    
    /// This function returns the data type of GL texture format type
    /// - Parameter formatType: GL texture type
    GLenum GetTextureDataType(GLenum formatType);
    
    /// This function converts the KanViz texture filter with GL texture filter
    /// - Parameter filter: KanViz filter
    GLint OpenGLFilterFromKanVizFilter(TextureFilter filter);
    
    /// This function converts the KanViz texture wrap with GL texture wrap
    /// - Parameter wrap: KanViz wrap
    GLint OpenGLWrapFromKanVizWrap(TextureWrap wrap);
    
    /// This function returns the texture format name as const char *
    /// - Parameter format: format type
    std::string_view KanVizFormatName(TextureFormat format);
    
    /// This function returns the texture wrap name as const char *
    /// - Parameter wrap: texture wrap
    std::string_view KanVizWrapName(TextureWrap wrap);
    
    /// This function returns the texture type name as const char *
    /// - Parameter type: texture type
    std::string_view KanVizTypeName(TextureType type);
    
    /// This function returns the texture filter name as const char *
    /// - Parameter filter: filter
    std::string_view KanVizFilterName(TextureFilter filter);
    
    /// This function converts the GL  Format to Ikan Format
    /// - Parameter format:GL Format
    TextureFormat OpenGLFormatToKanVizFormat(GLint format);
    
    /// This function returns the Attachment ID for Color or Depth
    /// - Parameters:
    ///   - attachment: Attachment type
    ///   - id: ID
    GLint KanVizAttachmentToOpenGL(TextureAttachment attachment, uint32_t id);
    
    /// This functionc creates the texture attachment for frame buffer format
    /// - Parameters:
    ///   - rendererID: texture renderer ID
    ///   - type: Texture type
    ///   - intenalFormat: texture internal format
    ///   - attachmentType: framebuffer attachment type
    ///   - colorID: Color ID
    ///   - depthID: Deptgh ID
    ///   - level: Level of framebuffer
    void AttachTexture(RendererID rendererID, TextureType type, TextureFormat intenalFormat, TextureAttachment attachmentType, uint32_t colorID, uint32_t depthID, uint32_t level);
  } // namespace TextureUtils
  
  // Frame buffer utils ---------------------------------------------------------------
  namespace FramebufferUtils
  {
    /// This function checks is the specification format is depth or not
    /// - Parameter format: Format type
    bool IsDepthFormat(FrameBufferAttachments::TextureFormat format);
    
    /// This functionc creates the texture attachment for frame buffer format
    /// - Parameters:
    ///   - format: framebuffer format
    ///   - spec: texture specification from framebuffer
    Ref<Texture> CreateAttachment(FrameBufferAttachments::TextureFormat format, TextureSpecification& spec, bool debugLog);
  } // namespace FramebufferUtils

} // namespace KanViz
