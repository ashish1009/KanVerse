//
//  OpenGLSpecification.cpp
//  KanViz
//
//  Created by Ashish . on 11/11/24.
//

#include "OpenGLSpecification.hpp"

#include "Renderer/Graphics/Texture.hpp"

// Utils ---------------------------------------------------------------------------------------------
namespace KanViz
{
#define GET_FORMAT_NAME(name) TextureUtils::KanVizFormatName(name)
  
  namespace TextureUtils
  {
    /// This function converts the KanViz Texture type with GL Texture type
    /// - Parameter format: KanViz Texture type
    GLenum OpenGLTypeFromKanVizType(TextureType type)
    {
      switch (type)
      {
        case TextureType::Texture2D      : return GL_TEXTURE_2D;
        case TextureType::TextureCubemap : return GL_TEXTURE_CUBE_MAP;
        default:
          IK_ASSERT(false, "Invalid Format!");
      }
      return GL_INVALID_INDEX;
    }
    
    /// This function converts the KanViz Format with GL Texture format
    /// - Parameter format: KanViz Texture format
    GLenum OpenGLFormatFromKanVizFormat(TextureFormat format)
    {
      switch (format)
      {
        case TextureFormat::RGBA16F         : return GL_RGBA16F;
        case TextureFormat::RGBA8           : return GL_RGBA8;
        case TextureFormat::RGBA            : return GL_RGBA;
        case TextureFormat::RGB8            : return GL_RGB8;
        case TextureFormat::RGB             : return GL_RGB;
        case TextureFormat::R32I            : return GL_R32I;
        case TextureFormat::RED             : return GL_RED;
        case TextureFormat::RED_INTEGER     : return GL_RED_INTEGER;
        case TextureFormat::DEPTH_COMPONENT : return GL_DEPTH_COMPONENT;
        default:
          IK_ASSERT(false, "Invalid Format!");
      }
      return GL_INVALID_INDEX;
    }
    
    /// This function returns the data type of format from GL Texture type
    /// - Parameter formatType: GL Texture type
    GLenum GetTextureDataType(GLenum formatType)
    {
      switch (formatType)
      {
        case GL_RGBA8:
        case GL_RGB8:
        case GL_RGBA:
        case GL_RED:
        case GL_R32I:
          return GL_UNSIGNED_BYTE;
          
        case GL_DEPTH_COMPONENT:
        case GL_RGBA16F:
          return GL_FLOAT;
          
        default:
          IK_ASSERT(false, "Add other formats!");
      }
      return GL_INVALID_INDEX;
    }
    
    /// This function returns the Open GL Filter from KanViz Filter
    /// - Parameter filter: KanViz Filter
    GLint OpenGLFilterFromKanVizFilter(TextureFilter filter)
    {
      switch (filter)
      {
        case TextureFilter::Linear:                 return GL_LINEAR;
        case TextureFilter::Nearest:                return GL_NEAREST;
        case TextureFilter::LinearMipmapLinear:     return GL_LINEAR_MIPMAP_LINEAR;
        case TextureFilter::LinearMipmapNearest:    return GL_LINEAR_MIPMAP_NEAREST;
        case TextureFilter::NearestMipmapLinear:    return GL_NEAREST_MIPMAP_LINEAR;
        case TextureFilter::NearestMipmapNearest:   return GL_NEAREST_MIPMAP_NEAREST;
        default:
          IK_ASSERT(false, "Invalid Texture Filter!");
      }
      return (GLint)GL_INVALID_INDEX;
    }
    
    /// This function returns the Open GL Wrap from KanViz wrap
    /// - Parameter filter: KanViz wrap
    GLint OpenGLWrapFromKanVizWrap(TextureWrap wrap)
    {
      switch (wrap)
      {
        case TextureWrap::ClampEdge:    return GL_CLAMP_TO_EDGE;
        case TextureWrap::ClampBorder:  return GL_CLAMP_TO_BORDER;
        case TextureWrap::Repeat:       return GL_REPEAT;
        case TextureWrap::RepeatMirror: return GL_MIRRORED_REPEAT;
        default:
          IK_ASSERT(false, "Invalid Texture Wrap!");
      }
      return (GLint)GL_INVALID_INDEX;
    }
    
    std::string_view KanVizFormatName(TextureFormat format)
    {
      switch (format)
      {
        case TextureFormat::RGBA16F :         return "GL_RGBA16F";
        case TextureFormat::RGBA8 :           return "GL_RGBA8";
        case TextureFormat::RGBA :            return "GL_RGBA";
        case TextureFormat::RGB8 :            return "GL_RGB8";
        case TextureFormat::RGB :             return "GL_RGB";
        case TextureFormat::RED :             return "GL_RED";
        case TextureFormat::R32I :            return "GL_R32I";
        case TextureFormat::RED_INTEGER :     return "GL_RED_INTEGER";
        case TextureFormat::DEPTH_COMPONENT : return "GL_DEPTH_COMPONENT";
        default:
          IK_ASSERT(false, "Invalid Format!");
      }
      return "";
    }
    std::string_view KanVizWrapName(TextureWrap wrap)
    {
      switch (wrap) {
        case TextureWrap::ClampEdge:    return "GL_CLAMP_TO_EDGE";
        case TextureWrap::ClampBorder:  return "GL_CLAMP_TO_BORDER";
        case TextureWrap::Repeat:       return "GL_REPEAT";
        case TextureWrap::RepeatMirror: return "GL_MIRRORED_REPEAT";
        default:
          IK_ASSERT(false, "Invalid Texture Wrap!");
      }
      return "";
    }
    std::string_view KanVizTypeName(TextureType type)
    {
      switch (type)
      {
        case TextureType::Texture2D: return "GL_TEXTURE_2D";
        case TextureType::TextureCubemap: return "GL_TEXTURE_CUBE_MAP";
        default:
          IK_ASSERT(false, "Invalid Texture Type!");
      }
      return "";
    }
    std::string_view KanVizFilterName(TextureFilter filter)
    {
      switch (filter)
      {
        case TextureFilter::Linear:               return "GL_LINEAR";
        case TextureFilter::Nearest:              return "GL_NEAREST";
        case TextureFilter::LinearMipmapLinear:   return "GL_LINEAR_MIPMAP_LINEAR";
        default:
          IK_ASSERT(false, "Invalid Texture Filter!");
      }
      return "";
    }
    
    TextureFormat OpenGLFormatToKanVizFormat(const GLint format)
    {
      if (format == GL_RGBA8)     return TextureFormat::RGBA8;
      else if (format == GL_RGBA) return TextureFormat::RGBA;
      else if (format == GL_RGB8) return TextureFormat::RGB8;
      else if (format == GL_RGB)  return TextureFormat::RGB;
      else if (format == GL_RED)  return TextureFormat::RED;
      
      IK_ASSERT(false, "Invalid Format!");
      return TextureFormat::RGBA8;
    }
    
    GLint KanVizAttachmentToOpenGL(TextureAttachment attachment, uint32_t id)
    {
      switch (attachment)
      {
        case TextureAttachment::Color : return GL_COLOR_ATTACHMENT0 + (GLint)id;
        case TextureAttachment::Depth : return GL_DEPTH_ATTACHMENT;
        default:
          IK_ASSERT(false, "Invalid Attachment!");
      }
      return (GLint)GL_INVALID_INDEX;
    }
    
    void AttachTexture(RendererID rendererID, TextureType type, TextureFormat intenalFormat, TextureAttachment attachmentType, uint32_t colorID, uint32_t depthID, uint32_t level)
    {
      GLint openGLAttachmentType = KanVizAttachmentToOpenGL(attachmentType, colorID);
      // TODO: KanViz: No need to Submit caller need to take care
#if 0
      Renderer::Submit([rendererID, type, intenalFormat, attachmentType, colorID, depthID, level, openGLAttachmentType](){
#endif
        
        if (TextureType::TextureCubemap == type)
        {
          if (TextureFormat::DEPTH_COMPONENT == intenalFormat)
          {
            glFramebufferTexture(GL_FRAMEBUFFER, (GLenum)openGLAttachmentType, rendererID, (GLint)level);
          }
          else
          {
            glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)openGLAttachmentType, GL_TEXTURE_CUBE_MAP_POSITIVE_X + depthID, rendererID, (GLint)level);
          }
        }
        else
        {
          glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)openGLAttachmentType, GL_TEXTURE_2D, rendererID, (GLint)level);
        }
        IK_CHECK_RENDERER_ERROR()
#if 0
      });
#endif
    }
  } // namespace TextureUtils
  
  namespace FramebufferUtils
  {
    bool IsDepthFormat(FrameBufferAttachments::TextureFormat format)
    {
      switch (format)
      {
        case FrameBufferAttachments::TextureFormat::RGBA8: return false;
        case FrameBufferAttachments::TextureFormat::Depth24Stencil: return true;
        case FrameBufferAttachments::TextureFormat::None:
        default:
          IK_ASSERT(false, "invalid format!");
      }
      return false;
    }
    
    Ref<Texture> CreateAttachment(FrameBufferAttachments::TextureFormat format, TextureSpecification& spec, bool debugLog)
    {
      Ref<Texture> attachment = nullptr;
      switch (format)
      {
          // Color Attachments -------------------------------------------------------------
        case FrameBufferAttachments::TextureFormat::RGBA8:
        {
          // Create the Color Image from the specification
          spec.internalFormat   = TextureFormat::RGBA8;
          spec.dataFormat       = TextureFormat::RGBA;
          attachment = TextureFactory::Create(spec);
          break;
        }
          
          // Depth Attachments -------------------------------------------------------------
        case FrameBufferAttachments::TextureFormat::Depth24Stencil:
        {
          // Create the Depth Image from the specification
          spec.internalFormat = TextureFormat::DEPTH_COMPONENT;
          spec.dataFormat     = TextureFormat::DEPTH_COMPONENT;
          attachment = TextureFactory::Create(spec);
          break;
        }
          
          // Invalid Attachments ----------------------------------------------------------
        case FrameBufferAttachments::TextureFormat::None:
        default:
          IK_ASSERT(false, "Invalid Format!");
      } // swtch(color attachment)
      
      if (debugLog)
      {
        IK_LOG_DEBUG(LogModule::FrameBuffer, "  Attachment : {0} | {1}", GET_FORMAT_NAME(spec.internalFormat), GET_FORMAT_NAME(spec.dataFormat));
      }
      return attachment;
    }
  } // namespace FramebufferUtils

} // namespace KanViz
