//
//  OpenGLShaderUniforms.hpp
//  KanViz
//
//  Created by Ashish . on 25/11/24.
//

#pragma once

#include "Renderer/Graphics/ShaderUniforms.hpp"

namespace KanViz
{
  /// This enum stores the type shader data type.
  enum class ShaderUniformDataType : uint8_t
  {
    None, Float32, Vec2, Vec3, Vec4, Mat3, Mat4, Int32, Bool, Struct
  };

  /// This enum stores the Shader unifor resource type
  enum class ShaderResourceType : uint8_t
  {
    None, Texture2D, TextureCubeMap
  };

  /// This class is the Open GL implementation to stores the uniform data
  /// --------------------------------------
  /// int uniformData1
  /// float uniformData2[3]
  /// Struct uniformData3
  /// --------------------------------------
  class OpenGLShaderUniformDeclaration : public ShaderUniformDeclaration
  {
  public:
    OpenGLShaderUniformDeclaration() = default;
    
    /// This constructor creates the open GL Shader uniform field with fundamental type.
    /// - Parameters:
    ///   - domain: Domain of shader
    ///   - type: type of fundamental uniform.
    ///   - name: name of uniform.
    ///   - count: count of shader data type (if array).
    OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderUniformDataType type, std::string_view name, uint32_t count = 1);
    /// This constructor creates the open GL Shader uniform field with structure type.
    /// - Parameters:
    ///   - domain: Domain of shader
    ///   - uniformStruct: uniform shader structure data.
    ///   - name: name of structure.
    ///   - count: count of shader data type.
    OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderStruct* uniformStruct, std::string_view name, uint32_t count = 1);
    /// This destructor destroyes the Shader uniform.
    virtual ~OpenGLShaderUniformDeclaration();
    
    /// This funciton updates the offset of field
    /// - Parameter offset: new offset
    void SetOffset(uint32_t offset) override;

    // Getters ------------------------------------------------
    /// This function returns the name of field.
    const std::string& GetName() const override;
    /// This function returns the size of field.
    uint32_t GetSize() const override;
    /// This function returns the count of field.
    uint32_t GetCount() const override;
    /// This function returns the offset of field.
    uint32_t GetOffset() const override;
    /// This function returns the domain of shader.
    ShaderDomain GetDomain() const override;

    /// This function returns the otype of field.
    ShaderUniformDataType GetType() const;
    /// This function returns true if field is array else false.
    bool IsArray() const;

    /// This function returns the structure pointer if field is structure.
    const ShaderStruct& GetShaderUniformStruct() const;

    /// This function returns the location of field in the shader.
    /// - Parameter index: index of shader uniform.
    int32_t GetLocation(uint32_t index = 0) const;

    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLShaderUniformDeclaration);

  private:
    ShaderDomain m_domain;                                            // < Domain of shader
    ShaderUniformDataType m_dataType {ShaderUniformDataType::None};   // < Data type of uniform
    ShaderStruct* m_struct;                                           // < Pointer to structure if type is struct
    std::string m_name {};                                            // < Name of variable uniform
    uint32_t m_size {};                                               // < Size of data
    uint32_t m_count {1};                                             // < Count of data (if array else always 1)
    uint32_t m_offset {0};                                            // < Offset of uniform in shader data
    std::vector<int32_t> m_locations;

    friend class OpenGLShader;
    friend class OpenGLShaderUniformBufferDeclaration;
  };
  
  /// This class implements the APIs to store the Open Gl shader fundamental uniforms resources.
  /// - Note: Non premitive data types like Sampler.
  class OpenGLShaderResourceDeclaration : public ShaderResourceDeclaration
  {
  public:
    /// This constructor creates the resource uniform of shader.
    /// - Parameters:
    ///   - type: type of uniform.
    ///   - name: name of uniform.
    ///   - count: count of uniform.
    OpenGLShaderResourceDeclaration(ShaderResourceType type, const std::string& name, uint32_t count);
    /// This destrcutror destroyes the uniform resource.
    virtual ~OpenGLShaderResourceDeclaration();

    /// This function returns the name of uniform.
    const std::string& GetName() const override;
    /// This function returns the register of uniform.
    uint32_t GetRegister() const override;
    /// This function returns the count of uniform.
    uint32_t GetCount() const override;
    /// This function returns the type of uniform.
    ShaderResourceType GetType() const;

    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLShaderResourceDeclaration);

  private:
    std::string m_name;
    uint32_t m_register, m_count;
    ShaderResourceType m_type;
    
    friend class Shader;
    friend class OpenGLShader;
  };
  
  // OpenGLShaderUniformBufferDeclaration ----------------------------------------------------------------------------
  /// This class implements the APIs to store the Open Gl shader fundamental uniforms in vector.
  class OpenGLShaderUniformBufferDeclaration : public ShaderUniformBufferDeclaration
  {
  public:
    /// This constructor creates the Unfirm buffer declaration with name and domain.
    /// - Parameters:
    ///   - name: name of uniform.
    ///   - domain: domain of uniform.
    OpenGLShaderUniformBufferDeclaration(const std::string& name, ShaderDomain domain);
    /// This destructor destroyes the Shader uniform.
    virtual ~OpenGLShaderUniformBufferDeclaration();
    
    /// This function finds the uniform declaretion by name.
    /// - Parameter name: name of uniform.
    ShaderUniformDeclaration* FindUniform(const std::string& name) override;
    /// This function pushes the unifom in vector.
    /// - Parameter uniform: uniform pointer to be pushed.
    void PushUniform(OpenGLShaderUniformDeclaration* uniform);
    
    /// This function returns the name of buffer.
    const std::string& GetName() const override;
    /// This function returns the register of buffer.
    uint32_t GetRegister() const override;
    /// This function returns the size of buffer.
    uint32_t GetSize() const override;
    /// This function returns all the declaration in buffer.
    const std::vector<ShaderUniformDeclaration*>& GetUniformDeclarations() const override;
    /// This function returns the domain of shader.
    ShaderDomain GetDomain() const;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLShaderUniformBufferDeclaration)
    
  private:
    std::string m_name;
    uint32_t m_register, m_size;
    std::vector<ShaderUniformDeclaration*> m_uniforms;
    ShaderDomain m_domain;
    
    friend class Shader;
    friend class OpenGLShader;
  };
  
  namespace ShaderUtils
  {
    /// This function returns the type with string type.
    /// - Parameter type: typ in strug.
    ShaderUniformDataType StringToUniformType(const std::string& type);
    /// This static function returns the type from string.
    /// - Parameter type: type in string.
    ShaderResourceType StringToResourceType(const std::string& type);
  }
} // namespace KanViz
