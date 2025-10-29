//
//  OpenGLShaderUniforms.cpp
//  KanViz
//
//  Created by Ashish . on 25/11/24.
//

#include "OpenGLShaderUniforms.hpp"

namespace KanViz 
{
#define SHADER_LOG(...) IK_LOG_DEBUG(LogModule::Shader, __VA_ARGS__);
  
  namespace ShaderUtils
  {
    /// This function convert the shader uniform data type to string
    /// - Parameter type: shader uniform data type
    std::string_view TypeToString(ShaderUniformDataType type)
    {
      switch (type)
      {
        case ShaderUniformDataType::None:       return "None  ";
        case ShaderUniformDataType::Bool:       return "bool  ";
        case ShaderUniformDataType::Int32:      return "int32 ";
        case ShaderUniformDataType::Float32:    return "float ";
        case ShaderUniformDataType::Vec2:       return "vec2  ";
        case ShaderUniformDataType::Vec3:       return "vec3  ";
        case ShaderUniformDataType::Vec4:       return "vec4  ";
        case ShaderUniformDataType::Mat3:       return "mat3  ";
        case ShaderUniformDataType::Mat4:       return "mat4  ";
        case ShaderUniformDataType::Struct:     return "struct";
      }
      return "Invalid Type";
    }
    
    /// This function returns the size of data type
    /// - Parameter type: shader uniform data type
    uint32_t SizeOfUniformType(ShaderUniformDataType type)
    {
      switch (type)
      {
        case ShaderUniformDataType::None:    return 0;
        case ShaderUniformDataType::Bool:
        case ShaderUniformDataType::Int32:
        case ShaderUniformDataType::Float32: return 4;
        case ShaderUniformDataType::Vec2:    return 4 * 2;
        case ShaderUniformDataType::Vec3:    return 4 * 3;
        case ShaderUniformDataType::Vec4:    return 4 * 4;
        case ShaderUniformDataType::Mat3:    return 4 * 3 * 3;
        case ShaderUniformDataType::Mat4:    return 4 * 4 * 4;
        case ShaderUniformDataType::Struct:
        default:
          IK_ASSERT(false, "No need to return the size in case of struct!")
          break;
      }
      return 0;
    }

    std::string_view TypeToString(ShaderResourceType type)
    {
      switch (type)
      {
        case ShaderResourceType::None:           return "None       ";
        case ShaderResourceType::Texture2D:      return "sampler2D  ";
        case ShaderResourceType::TextureCubeMap: return "samplerCube";
      }
      return "Invalid Type";
    }

    ShaderUniformDataType StringToUniformType(const std::string& type)
    {
      if (type == "bool")     return ShaderUniformDataType::Bool;
      if (type == "int")      return ShaderUniformDataType::Int32;
      if (type == "float")    return ShaderUniformDataType::Float32;
      if (type == "vec2")     return ShaderUniformDataType::Vec2;
      if (type == "vec3")     return ShaderUniformDataType::Vec3;
      if (type == "vec4")     return ShaderUniformDataType::Vec4;
      if (type == "mat3")     return ShaderUniformDataType::Mat3;
      if (type == "mat4")     return ShaderUniformDataType::Mat4;
      
      return ShaderUniformDataType::None;
    }
    
    ShaderResourceType StringToResourceType(const std::string& type)
    {
      if (type == "sampler2D")    return ShaderResourceType::Texture2D;
      if (type == "sampler2DMS")  return ShaderResourceType::Texture2D;
      if (type == "samplerCube")  return ShaderResourceType::TextureCubeMap;
      
      return ShaderResourceType::None;
    }

  } // namespace ShaderUtils
  
  // OpenGLShaderUniformDeclaration ----------------------------------------------------------------------------------
  OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderUniformDataType type, std::string_view name, uint32_t count)
  : m_domain(domain), m_dataType(type), m_name(name), m_count(count)
  {
    m_size = ShaderUtils::SizeOfUniformType(m_dataType) * m_count;
    SHADER_LOG("      {0} {1}[{2}] (Size : {3}) ", ShaderUtils::TypeToString(m_dataType), m_name, m_count, m_size);
  }
  OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderStruct* uniformStruct, std::string_view name, uint32_t count)
  : m_domain(domain), m_struct(uniformStruct), m_dataType(ShaderUniformDataType::Struct), m_name(name), m_count(count)
  {
    IK_ASSERT(uniformStruct, "Structure is nullptr!");
    m_size = m_struct->GetSize() * m_count;
    SHADER_LOG("      {0} {1}[{2}] (Size : {3}) ", ShaderUtils::TypeToString(m_dataType), m_name, m_count, m_size);
  }
  OpenGLShaderUniformDeclaration::~OpenGLShaderUniformDeclaration()
  {
    SHADER_LOG("      Destroying : {0} {1}[{2}] (Size : {3}) ", ShaderUtils::TypeToString(m_dataType), m_name, m_count, m_size);
  }
  
  void OpenGLShaderUniformDeclaration::SetOffset(uint32_t offset)
  {
    if (m_dataType == ShaderUniformDataType::Struct)
    {
      m_struct->SetOffset(offset);
    }
    m_offset = offset;
  }
  
  const std::string& OpenGLShaderUniformDeclaration::GetName() const
  {
    return m_name;
  }
  uint32_t OpenGLShaderUniformDeclaration::GetSize() const
  {
    return m_size;
  }
  uint32_t OpenGLShaderUniformDeclaration::GetCount() const
  {
    return m_count;
  }
  uint32_t OpenGLShaderUniformDeclaration::GetOffset() const
  {
    return m_offset;
  }
  ShaderDomain OpenGLShaderUniformDeclaration::GetDomain() const
  {
    return m_domain;
  }

  ShaderUniformDataType OpenGLShaderUniformDeclaration::GetType() const
  {
    return m_dataType;
  }
  bool OpenGLShaderUniformDeclaration::IsArray() const
  {
    return m_count > 1;
  }

  const ShaderStruct& OpenGLShaderUniformDeclaration::GetShaderUniformStruct() const
  {
    IK_ASSERT(m_struct, "Struct is NULL!");
    return *m_struct;
  }
  
  int32_t OpenGLShaderUniformDeclaration::GetLocation(uint32_t index) const
  {
    return m_locations[index];
  }
  
  // OpenGLShaderResourceDeclaration --------------------------------------------------------------------------------
  OpenGLShaderResourceDeclaration::OpenGLShaderResourceDeclaration(ShaderResourceType type, const std::string& name, uint32_t count)
  : m_type(type), m_name(name), m_count(count)
  {
    SHADER_LOG("      {0} {1}[{2}]", ShaderUtils::TypeToString(m_type), m_name, m_count);
  }
  
  OpenGLShaderResourceDeclaration::~OpenGLShaderResourceDeclaration() {
    SHADER_LOG("      Destroying : {0} {1}[{2}]", ShaderUtils::TypeToString(m_type), m_name, m_count);
  }
  
  const std::string& OpenGLShaderResourceDeclaration::GetName() const
  {
    return m_name;
  }
  uint32_t OpenGLShaderResourceDeclaration::GetRegister() const
  {
    return m_register;
  }
  uint32_t OpenGLShaderResourceDeclaration::GetCount() const
  {
    return m_count;
  }
  ShaderResourceType OpenGLShaderResourceDeclaration::GetType() const
  {
    return m_type;
  }
  
  // OpenGLShaderUniformBufferDeclaration ----------------------------------------------------------------------------
  OpenGLShaderUniformBufferDeclaration::OpenGLShaderUniformBufferDeclaration(const std::string& name, ShaderDomain domain)
  : m_name(name), m_domain(domain), m_size(0), m_register(0)
  {
    
  }
  
  OpenGLShaderUniformBufferDeclaration::~OpenGLShaderUniformBufferDeclaration()
  {
    for (ShaderUniformDeclaration* uniform : m_uniforms)
    {
      delete uniform;
      uniform = nullptr;
    }
    
    m_uniforms.clear();
  }
  
  void OpenGLShaderUniformBufferDeclaration::PushUniform(OpenGLShaderUniformDeclaration* uniform)
  {
    IK_ASSERT(uniform, "Uniform is NULL!");
    uint32_t offset = 0;
    if (m_uniforms.size())
    {
      OpenGLShaderUniformDeclaration* previous = (OpenGLShaderUniformDeclaration*)m_uniforms.back();
      offset = previous->m_offset + previous->m_size;
    }
    uniform->SetOffset(offset);
    m_size += uniform->GetSize();
    m_uniforms.push_back(uniform);
  }
  
  ShaderUniformDeclaration* OpenGLShaderUniformBufferDeclaration::FindUniform(const std::string& name)
  {
    for (ShaderUniformDeclaration* uniform : m_uniforms)
    {
      if (uniform->GetName() == name)
      {
        return uniform;
      }
    }
    return nullptr;
  }
  
  const std::string& OpenGLShaderUniformBufferDeclaration::GetName() const
  {
    return m_name;
  }
  uint32_t OpenGLShaderUniformBufferDeclaration::GetRegister() const
  {
    return m_register;
  }
  uint32_t OpenGLShaderUniformBufferDeclaration::GetSize() const
  {
    return m_size;
  }
  const std::vector<ShaderUniformDeclaration*>& OpenGLShaderUniformBufferDeclaration::GetUniformDeclarations() const
  {
    return m_uniforms;
  }
  ShaderDomain OpenGLShaderUniformBufferDeclaration::GetDomain() const
  {
    return m_domain;
  }

} // namespace KanViz
