//
//  Factories.cpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#include "Factories.hpp"

#include "Renderer/Core/Renderer.hpp"

#include "Platform/OpenGL/OpenGLRendererContext.hpp"
#include "Platform/OpenGL/OpenGLRendererAPI.hpp"
#include "Platform/OpenGL/OpenGLTexture.hpp"
#include "Platform/OpenGL/OpenGLVertexBuffer.hpp"
#include "Platform/OpenGL/OpenGLIndexBuffer.hpp"
#include "Platform/OpenGL/OpenGLShader.hpp"
#include "Platform/OpenGL/OpenGLPipeline.hpp"
#include "Platform/OpenGL/OpenGLFrameBuffer.hpp"

namespace KanViz
{
  /// Validates the current renderer API type.
  /// - Note: Asserts if the renderer API is invalid and logs critical errors.
  static void ValidateRendererAPI()
  {
    IK_LOG_CRITICAL(LogModule::Renderer, "Renderer API Type is not set or is set as invalid");
    IK_LOG_CRITICAL(LogModule::Renderer, "Call Renderer::SetCurrentRendererAPI(RendererType) before any renderer initialization to set Renderer API type");
    IK_LOG_CRITICAL(LogModule::Renderer, "'RendererType should not be RendererType::Invalid");
    
    IK_ASSERT(false, "Renderer API type is invalid; it must be set before initialization.");
  }
  
  Scope<RendererContext> RendererContextFactory::Create(GLFWwindow* windowPtr)
  {
    switch(Renderer::GetCurrentRendererAPI())
    {
      case RendererType::OpenGL:
        return CreateScope<OpenGLRendererContext>(windowPtr);
      case RendererType::Invalid:
      default:
        ValidateRendererAPI();
        return nullptr; // Adding for Release mode code where assert MACRO is not defined
    }
  }
  
  Scope<RendererAPI> RendererAPIFactory::Create()
  {
    switch (Renderer::GetCurrentRendererAPI())
    {
      case RendererType::OpenGL:
        return CreateScope<OpenGLRendererAPI>();
      case RendererType::Invalid:
      default:
        ValidateRendererAPI();
        return nullptr; // Adding for Release mode code where assert MACRO is not defined
    }
  }
  
  Ref<Texture> TextureFactory::Create(const TextureSpecification &spec)
  {
    switch(Renderer::GetCurrentRendererAPI())
    {
      case RendererType::OpenGL:
        return CreateScope<OpenGLTexture>(spec);
      case RendererType::Invalid:
      default:
        ValidateRendererAPI();
        return nullptr; // Adding for Release mode code where assert MACRO is not defined
    }
  }
  
  Ref<Texture> TextureFactory::Create(uint32_t data)
  {
    // TODO: KanViz: Copy inside Open GL Texture class or use shared pointer.
    static uint32_t whiteTextureData = data;
    
    // Texture specification
    TextureSpecification textureSpec;
    textureSpec.title = "White Texture";
    textureSpec.width = 1;
    textureSpec.height = 1;
    textureSpec.data = &whiteTextureData;
    textureSpec.size = sizeof(uint32_t);
    
    return Create(textureSpec);
  }
  
  Ref<Image> TextureFactory::Create(const Imagespecification &spec)
  {
    switch(Renderer::GetCurrentRendererAPI())
    {
      case RendererType::OpenGL:
        return CreateScope<OpenGLImage>(spec);
      case RendererType::Invalid:
      default:
        ValidateRendererAPI();
        return nullptr; // Adding for Release mode code where assert MACRO is not defined
    }
  }
  
  Ref<Image> TextureFactory::Create(const std::filesystem::path& filePath)
  {
    Imagespecification spec;
    spec.filePath = filePath;
    return Create(spec);
  }
  
  Ref<VertexBuffer> VertexBufferFactory::Create(void *data, uint32_t size)
  {
    switch (Renderer::GetCurrentRendererAPI())
    {
      case RendererType::OpenGL:
        return CreateScope<OpenGLVertexBuffer>(data, size);
      case RendererType::Invalid:
      default:
        ValidateRendererAPI();
        return nullptr; // Adding for Release mode code where assert MACRO is not defined
    }
  }
  
  Ref<Texture> TextureFactory::Create(const CharTextureSpecification& spec)
  {
    switch (Renderer::GetCurrentRendererAPI())
    {
      case RendererType::OpenGL:
        return CreateScope<OpenGLCharTexture>(spec);
      case RendererType::Invalid:
      default:
        ValidateRendererAPI();
        return nullptr; // Adding for Release mode code where assert MACRO is not defined
    }
  }
  
  Ref<VertexBuffer> VertexBufferFactory::Create(uint32_t size)
  {
    switch (Renderer::GetCurrentRendererAPI())
    {
      case RendererType::OpenGL:
        return CreateScope<OpenGLVertexBuffer>(size);
      case RendererType::Invalid:
      default:
        ValidateRendererAPI();
        return nullptr; // Adding for Release mode code where assert MACRO is not defined
    }
  }
  
  Ref<IndexBuffer> IndexBufferFactory::CreateWithSize(void *data, uint32_t size)
  {
    switch (Renderer::GetCurrentRendererAPI())
    {
      case RendererType::OpenGL:
        return CreateScope<OpenGLIndexBuffer>(data, size);
      case RendererType::Invalid:
      default:
        ValidateRendererAPI();
        return nullptr; // Adding for Release mode code where assert MACRO is not defined
    }
  }
  
  Ref<IndexBuffer> IndexBufferFactory::CreateWithCount(void *data, uint32_t count)
  {
    switch (Renderer::GetCurrentRendererAPI())
    {
      case RendererType::OpenGL:
        return CreateScope<OpenGLIndexBuffer>(data, count * SizeOfSingleIndices);
      case RendererType::Invalid:
      default:
        ValidateRendererAPI();
        return nullptr; // Adding for Release mode code where assert MACRO is not defined
    }
  }
  
  Ref<Shader> ShaderFactory::Create(const std::filesystem::path &shaderFilePath)
  {
    switch (Renderer::GetCurrentRendererAPI())
    {
      case RendererType::OpenGL:
        return CreateScope<OpenGLShader>(shaderFilePath);
      case RendererType::Invalid:
      default:
        ValidateRendererAPI();
        return nullptr; // Adding for Release mode code where assert MACRO is not defined
    }
  }
  
  Ref<Pipeline> PipelineFactory::Create(const PipelineSpecification& spec)
  {
    switch (Renderer::GetCurrentRendererAPI())
    {
      case RendererType::OpenGL:
        return CreateScope<OpenGLPipeline>(spec);
      case RendererType::Invalid:
      default:
        ValidateRendererAPI();
        return nullptr; // Adding for Release mode code where assert MACRO is not defined
    }
  }
  
  Ref<FrameBuffer> FrameBufferFactory::Create(const FrameBufferSpecification& spec)
  {
    switch (Renderer::GetCurrentRendererAPI())
    {
      case RendererType::OpenGL:
        return CreateRef<OpenGLFrameBuffer>(spec);
      case RendererType::Invalid:
      default:
        ValidateRendererAPI();
        return nullptr;
    }
  }

} // namespace KanViz
