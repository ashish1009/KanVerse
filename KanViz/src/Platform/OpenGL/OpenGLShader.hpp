//
//  OpenGLShader.hpp
//  KanViz
//
//  Created by Ashish . on 18/11/24.
//

#pragma once

#include <glad/glad.h>

#include "Renderer/Graphics/Shader.hpp"
#include "Platform/OpenGL/OpenGLShaderUniforms.hpp"

namespace KanViz
{
  // Max Supported Shaders
  static constexpr uint32_t MaxShaderSupported = 3;
  
  /// This class implements the APIs for for compiling and storing Open GL Shader.
  class OpenGLShader : public Shader
  {
  public:
    /// This consturctor creates the shader. Rarse and compile the shader code
    /// - Parameter shaderFilePath: Shader Code file path
    OpenGLShader(const std::filesystem::path& shaderFilePath);
    /// This destructor deletes the Open GL shader
    ~OpenGLShader();
    
    /// This function binds the current Shader to the renderers
    void Bind() const override;
    /// This function unbinds the current Shader from the renderers
    void Unbind() const override;
    
    /// This function set the vertex shader buffer data.
    /// - Parameter buffer: buffer data.
    void SetVSMaterialUniformBuffer(const Buffer& buffer) override;
    /// This function set the fragment shader buffer data.
    /// - Parameter buffer: buffer data.
    void SetFSMaterialUniformBuffer(const Buffer& buffer) override;
    /// This function set the geomatry shader buffer data.
    /// - Parameter buffer: buffer data.
    void SetGSMaterialUniformBuffer(const Buffer& buffer) override;

    // Getters -----------------------------------------------------------------------------------------------------
    /// This function returns the Renderer ID of Shader.
    RendererID GetRendererID() const override;
    
    /// This function returns the Name of Shader.
    const std::string& GetName() const override;
    /// This function returns the File Path of Shader.
    const std::filesystem::path& GetFilePath() const override;
        
    /// This function returns true if have the vertex shader buffer data.
    bool HasVSMaterialUniformBuffer() const override;
    /// This function returns true if have the fragment shader buffer data.
    bool HasFSMaterialUniformBuffer() const override;
    /// This function returns true if have the geomatry shader buffer data.
    bool HasGSMaterialUniformBuffer() const override;
    
    /// This function resturns the vertex Shader buffer data.
    const ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer() const override;
    /// This function resturns the fragment Shader buffer data.
    const ShaderUniformBufferDeclaration& GetFSMaterialUniformBuffer() const override;
    /// This function resturns the geomatry Shader buffer data.
    const ShaderUniformBufferDeclaration& GetGSMaterialUniformBuffer() const override;
    
    /// This function returns all the resources.
    const std::vector<ShaderResourceDeclaration*>& GetResources() const override;

    /// This functions uploads the Matrix 4x4 array value to shader.
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - values: Value of Uniform
    ///   - count: Size of Mat4 Array
    void SetUniformMat4Array(std::string_view name, const glm::mat4& values, uint32_t count) override;
    
    /// This functions uploads the Matrix 4x4 value to shader.
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    void SetUniformMat4(std::string_view name, const glm::mat4& value) override;
    /// This functions uploads the Matrix 3x3 value to shader.
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    void SetUniformMat3(std::string_view name, const glm::mat3& value) override;
    
    /// This functions uploads the Flaot value to shader.
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    void SetUniformFloat1(std::string_view name, float value) override;
    /// This functions uploads the Vec2 value to shader.
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    void SetUniformFloat2(std::string_view name, const glm::vec2& value) override;
    /// This functions uploads the Vec3 value to shader.
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    void SetUniformFloat3(std::string_view name, const glm::vec3& value) override;
    /// This functions uploads the Vec4 value to shader.
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    void SetUniformFloat4(std::string_view name, const glm::vec4& value) override;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLShader);
    
  
  private:
    // Member Functions ---------------------------------------------------------------------------------------------------------------------------
    /// This function reads the shader code in string and store all the shader code in a map to be used by compiler.
    /// - Parameter sourceString: shader code as string
    void PreprocessShader(const std::string& sourceString);
    /// This functions compiles all the shader codes and store their ID in Program ID (renderer_id).
    void Compile();
    /// This function returns the location of attribute in the shader.
    /// - Parameter name: attribute name.
    int32_t GetUniformLocation(std::string_view name);
    
    /// This function parses the shader uniforms to store the uniform data in buffers
    void Parse();
    /// This function resolves all the uniform present in the shader
    void ResolveUniforms();

    /// This function parses the Uniforms that are structure in shader. It will just store the structures only.
    /// - Parameters:
    ///   - block: block code of shader
    ///   - domain: type of shader
    void ParseStructure(const std::string& block, ShaderDomain domain);
    /// This function parses the Uniforms
    /// - Parameters:
    ///   - statement: block fo code of shader.
    ///   - domain domain of shader.
    void ParseUniforms(const std::string& statement, ShaderDomain domain);

    /// This function finds the structure stored in shader.
    /// - Parameter name: Name of structure.
    ShaderStruct* FindStruct(std::string_view name);

    /// This function resolves and set the uniforms using declaration.
    /// - Parameters:
    ///   - decl: declaration.
    ///   - buffer: buffer with data.
    void ResolveAndSetUniforms(const Ref<OpenGLShaderUniformBufferDeclaration>& decl, const Buffer& buffer);
    /// This function resolves and set the uniforms using uniform instance.
    /// - Parameters:
    ///   - uniform: uniform.
    ///   - buffer: buffer with data.
    void ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, const Buffer& buffer);
    /// This function resolves and set the uniforms using uniform instance array.
    /// - Parameters:
    ///   - uniform: uniform.
    ///   - buffer: buffer with data.
    void ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, const Buffer& buffer);
    /// This function resolves and set the uniforms using field instance.
    /// - Parameters:
    ///   - field: field isntacnce.
    ///   -  data: data buffer.
    ///   - offset: offset of data.
    ///   - idx: index.
    void ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, std::byte* data, int32_t offset, uint8_t idx);
    
    // Attributes ---------------------------------------------------------------------------------------------------
    /// This functions uploads the Int value to shader.
    /// - Parameters:
    ///   - name: Name of Uniform.
    ///   - value: Value of Uniform.
    void SetUniformInt1(std::string_view name, int32_t value);
    /// This functions uploads the Int Array value to shader.
    /// - Parameters:
    ///   - name: Name of Uniform.
    ///   - values: Values of Uniform.
    ///   - count: Size of array.
    void SetIntArray(std::string_view name, int32_t* values, uint32_t count);
    
    /// This function upload the uniform of type structure.
    /// - Parameters:
    ///   - location: location of field.
    ///   - value: value to be uploaded.
    void UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform, std::byte* buffer, uint32_t offset);

    // Member Variables ---------------------------------------------------------------------------------------------------------------------------
    RendererID m_rendererID {0};
    std::filesystem::path m_filePath {};
    std::string m_name {};
    
    std::unordered_map<GLenum /* GL Shader type */, std::string /* Shader code */> m_shaderSourceCodeMap;
    std::unordered_map<std::string_view /* Attribute name */, int32_t /* Attribute location */> m_locationMap;

    std::vector<ShaderStruct*> m_structs; // Stores the structure in the shader
    std::vector<ShaderResourceDeclaration*> m_resources; // Stores the resources of shader like sampler 2D
    
    Ref<OpenGLShaderUniformBufferDeclaration> m_vsMaterialUniformBuffer; // Uniform data buffer of vertex shader
    Ref<OpenGLShaderUniformBufferDeclaration> m_fsMaterialUniformBuffer; // Uniform data buffer of pixel shader
    Ref<OpenGLShaderUniformBufferDeclaration> m_gsMaterialUniformBuffer; // Uniform data buffer of geometry shader
  };
} // namespace KanViz
