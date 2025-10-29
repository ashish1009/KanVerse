//
//  ShaderUniforms.cpp
//  KanViz
//
//  Created by Ashish . on 04/11/24.
//

#include "ShaderUniforms.hpp"

namespace KanViz
{
  ShaderStruct::ShaderStruct(std::string_view name)
  : m_name(name.data())
  {

  }
  
  ShaderStruct::~ShaderStruct()
  {
    for (ShaderUniformDeclaration* field : m_memberFields)
    {
      delete field;
    }
  }
  
  void ShaderStruct::AddMember(ShaderUniformDeclaration* memebreField)
  {
    IK_PROFILE();
    
    m_size += memebreField->GetSize();
    uint32_t offset = 0;
    if (m_memberFields.size())
    {
      ShaderUniformDeclaration* previous = m_memberFields.back();
      offset = previous->GetOffset() + previous->GetSize();
    }

    memebreField->SetOffset(offset);
    m_memberFields.emplace_back(memebreField);
  }
  
  void ShaderStruct::SetOffset(uint32_t offset)
  {
    m_offset = offset;
  }
  
  uint32_t ShaderStruct::GetSize() const
  {
    return m_size;
  }
  
  const std::string& ShaderStruct::GetName() const
  {
    return m_name;
  }
  
  const std::vector<ShaderUniformDeclaration*>& ShaderStruct::GetMemberFields() const
  {
    return m_memberFields;
  }
  
  uint32_t ShaderStruct::GetOffset() const
  {
    return m_offset;
  }
} // namespace KanViz
