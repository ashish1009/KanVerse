//
//  Factories.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Renderer/Graphics/Specification.h"

// Forward Declarations
class GLFWwindow;

namespace KanViz
{
  // KanViz Forward Declarations
  class RendererContext;
  class RendererAPI;
  class Texture;
  class Image;
  class VertexBuffer;
  class IndexBuffer;
  class Shader;
  class Pipeline;
  class FrameBuffer;

  /// This structure provides a method to create the renderer context instance based on the renderer API supported by the engine.
  struct RendererContextFactory
  {
    /// This function creates the renderer context instance based on the renderer API supported by the Engine
    /// - Parameter windowPtr: Pointer to the GLFW window.
    /// - Returns: A scope that manages the lifetime of the created RendererContext instance.
    /// - Note: Returns nullptr for release mode if the assertion macro is not defined.
    [[nodiscard]] static Scope<RendererContext> Create(GLFWwindow* windowPtr);
  };
  
  /// This structure provides a method to create the renderer API instance based on the supported APIs.
  struct RendererAPIFactory
  {
    /// This function creates the Renderer API instance based on the Supported APIs
    /// - Returns: A scope that manages the lifetime of the created RendererAPI instance.
    /// - Note: Returns nullptr for release mode if the assertion macro is not defined.
    [[nodiscard]] static Scope<RendererAPI> Create();
  };
  
  /// This structure provides a method to create the Texture instance based on the specifications
  struct TextureFactory
  {
    /// This function creates the texture instance with specification based on the current Supported API
    /// - Parameter spec: Texture specification
    [[nodiscard]] static Ref<Texture> Create(const TextureSpecification& spec);
    /// This function creates the Texture instance with data based on the current Supported API
    /// - Parameter data : White data
    [[nodiscard]] static Ref<Texture> Create(uint32_t data = 0xffffffff);
    
    /// This static functions creates the Texture from image file
    /// - Parameter spec: Texture specification
    [[nodiscard]] static Ref<Image> Create(const Imagespecification& spec);
    /// This static functions creates the Texture from image file
    /// - Parameter filePath: path of texture file
    [[nodiscard]] static Ref<Image> Create(const std::filesystem::path& filePath);
    
    /// This function creates Character texture to render text
    /// - Parameter spec: Character Texture specification
    [[nodiscard]] static Ref<Texture> Create(const CharTextureSpecification& charTextureSpec);
  };
  
  /// This structure provides a method to create the Vertex buffer instance based on the data
  struct VertexBufferFactory
  {
    /// This function creates the vertex buffer instance with data pointer and size of data. Buffer data is stored in GPU statically at initialization
    /// - Parameters:
    ///   - data: Data pointer
    ///   - size: size of data
    [[nodiscard]] static Ref<VertexBuffer> Create(void* data, uint32_t size);
    /// This function creates the vertex buffer instance with size of data. Buffer data is stored in GPU dynamically while rendering
    /// - Parameter size: size of data
    [[nodiscard]] static Ref<VertexBuffer> Create(uint32_t size);
  };
  
  /// This structure stores the API to create the index buffer instance based on the renderer API supported by the Engine
  struct IndexBufferFactory
  {
    /// This function create the Index Buffer with Indices data and its size based on the renderer API supported by the Engine
    /// - Parameters:
    ///   - data: data to be filled in index buffer
    ///   - size: size of data in index buffer
    [[nodiscard]] static Ref<IndexBuffer> CreateWithSize(void* data, uint32_t size);
    /// This function create the Index Buffer with Indices data and number of indices based on the renderer API supported by the Engine
    /// - Parameters:
    ///   - data: data to be filled in index buffer
    ///   - count: number of indices in index buffer
    /// - Note: Size of single indices is taken as size of uint32_t (4)
    [[nodiscard]] static Ref<IndexBuffer> CreateWithCount(void* data, uint32_t count);
  };
  
  /// This structure provides a method to create the Shader compiler instance based on file path
  struct ShaderFactory
  {
    /// This function creates the shader compiler instance based on the supported GL API
    /// - Parameter shaderFilePath: file path of shader
    [[nodiscard]] static Ref<Shader> Create(const std::filesystem::path& shaderFilePath);
  };
  
  /// This structure stores the API to create the pipeline instance based on the renderer API supported by the Engine
  struct PipelineFactory
  {
    /// This function creates the pipeline instance with specification data based on the renderer API supported by the Engine
    /// - Parameter spec: Pipeline specification data
    [[nodiscard]] static Ref<Pipeline> Create(const PipelineSpecification& spec);
  };
  
  /// This structure stores the API to create the framebuffer instance based on the renderer API supported by the Engine
  struct FrameBufferFactory
  {
    /// This function creates the framebuffer instance with specification data based on the renderer API supported by the Engine
    /// - Parameter spec: Frame buffer specification
    [[nodiscard]] static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
  };

} // namespace KanViz
