//
//  OpenGLTexture.hpp
//  KanViz
//
//  Created by Ashish . on 04/11/24.
//

#pragma once

#include "Renderer/Graphics/Texture.hpp"

namespace KanViz
{
  /// This is the Open GL implementation class for storing graphics texture
  class OpenGLTexture : public Texture
  {
  public:
    /// This constructor creates the texture instance with specification data
    /// - Parameter specification: texcture specification data
    OpenGLTexture(const TextureSpecification& specification);
    /// This destroys the Open GL texture instance
    ~OpenGLTexture();

    /// This function binds the texture at slot inside shader.
    /// - Parameter slot: shader slot where this texture to be binded.
    void Bind(uint32_t slot = 0) const override;
    /// This function unbinds the current texture from the renderer.
    void Unbind() const override;
    
    /// This function attaches the current Image to Framebuffer.
    /// - Parameters:
    ///   - attachmentType: attachment type of texture.
    ///   - colorID : Color Attaachment ID to Framebuffer.
    ///   - depthID : Depth Attachment ID to Framebuffer.
    ///   - level : level of framebuffer attachment.
    void AttachToFramebuffer(TextureAttachment attachmentType, uint32_t colorID = 0, uint32_t depthID = 0, uint32_t level = 0) const override;
    
    /// This function returns width of texture.
    uint32_t GetWidth() const override;
    /// This function returns height of texture.
    uint32_t GetHeight() const override;
    /// This function returns renderer ID of texture.
    RendererID GetRendererID() const override;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLTexture);

  private:
    RendererID m_rendererID {0};
    TextureSpecification m_specification;
  };
  
  /// This class implements the APIs to create image for Open GL.
  class OpenGLImage : public Image
  {
  public:
    /// This constructor creates the Open GL image from specification.
    /// - Parameter spec: Open GL Image Specification
    OpenGLImage(const Imagespecification& spec);
    /// This destructor destroyes and deletes the Open GL image.
    virtual ~OpenGLImage();
    
    /// This function binds the image at slot inside shader.
    /// - Parameter slot: shader slot where this image to be binded.
    void Bind(uint32_t slot = 0) const override;
    /// This function unbinds the image.
    void Unbind() const override;
    
    /// This function attach the current Image to Framebuffer.
    /// - Parameters:
    ///   - attachmentType: attachment type of image.
    ///   - colorID : Color Attaachment ID to Framebuffer.
    ///   - depthID : Depth Attachment ID to Framebuffer.
    ///   - level : level of framebuffer attachment.
    void AttachToFramebuffer(TextureAttachment attachmentType, uint32_t colorID = 0, uint32_t depthID = 0, uint32_t level = 0) const override;
    
    /// This function returns width of image.
    uint32_t GetWidth() const override;
    /// This function returns height of image.
    uint32_t GetHeight() const override;
    /// This function returns renderer ID of image.
    RendererID GetRendererID() const override;
    
    /// This function returns file path of image.
    const std::filesystem::path& GetfilePath() const override;
    /// This function returns name of image.
    const std::string& GetName() const override;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLImage);
    
  private:
    RendererID m_rendererID {0};
    Imagespecification m_specification;
    int32_t m_width {1600};
    int32_t m_height {900};
    int32_t m_channel {0};
    uint32_t m_size {0};
    std::string m_name {};
    void* m_data {nullptr};
  };
  
  /// This class implements the APIs to create character texture for Open GL.
  class OpenGLCharTexture : public CharTexture
  {
  public:
    /// This constructor creates the Open GL character texture from specification.
    /// - Parameter charTextureSpec: Character texture specification
    OpenGLCharTexture(const CharTextureSpecification& charTextureSpec);
    /// This destructor destroyes and deletes the Open GL character texture.
    virtual ~OpenGLCharTexture();
    
    /// This function binds the character texture at slot inside shader.
    /// - Parameter slot: shader slot where this character texture to be binded.
    void Bind(uint32_t slot) const override;
    /// This function unbinds the character texture
    void Unbind() const  override;
    
    /// This function attach the current Image to Framebuffer
    /// - Parameters:
    ///   - attachmentType: attachment type of character texture.
    ///   - colorID : Color Attaachment ID to Framebuffer
    ///   - depthID : Depth Attachment ID to Framebuffer
    ///   - level : level of framebuffer attachment.
    void AttachToFramebuffer(TextureAttachment attachmentType, uint32_t colorID = 0, uint32_t depthID = 0, uint32_t level = 0) const override;
    
    /// This function returns the Renderer ID.
    RendererID GetRendererID() const override;
    /// This function returns width of texture.
    uint32_t GetWidth() const override;
    /// This function returns height of texture.
    uint32_t GetHeight() const override;
    /// This function returns the size.
    glm::ivec2 GetSize() const override;
    /// This function returns the bearing.
    glm::ivec2 GetBearing() const override;
    /// This function returns the advance.
    uint32_t GetAdvance() const override;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLCharTexture);
    
  private:
    RendererID m_rendererID {};
    CharTextureSpecification m_specification;
    uint32_t m_dataSize {0};
    uint32_t m_width {0}, m_height {0};
  };

} // namespace KanViz
