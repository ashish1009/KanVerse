//
//  BufferLayout.cpp
//  KanViz
//
//  Created by Ashish . on 04/11/24.
//

#include "BufferLayout.hpp"

namespace KanViz
{
  namespace BufferUtils
  {
    /// This function returns the number of bytes in Shader data types. (Size)
    /// - Parameter type: Shader data type
    [[maybe_unused]] static uint32_t ShaderDataTypeSize(ShaderDataType type)
    {
      switch (type)
      {
        case ShaderDataType::Invalid : return 0;
        case ShaderDataType::Float   : return 4;
        case ShaderDataType::Float2  : return 4 * 2;
        case ShaderDataType::Float3  : return 4 * 3;
        case ShaderDataType::Float4  : return 4 * 4;
        case ShaderDataType::Mat3    : return 4 * 3 * 3;
        case ShaderDataType::Mat4    : return 4 * 4 * 5;
        case ShaderDataType::Int     : return 4;
        case ShaderDataType::Int2    : return 4 * 2;
        case ShaderDataType::Int3    : return 4 * 3;
        case ShaderDataType::Int4    : return 4 * 4;
        case ShaderDataType::Bool    : return 1;
      }
      IK_ASSERT(false, "Invalid Type");
      return 0;
    }
    
    /// This function returns the number of same data type elements. (Count)
    /// - Parameter type: Shader data type
    [[maybe_unused]] static uint32_t GetElementCount(ShaderDataType type)
    {
      switch (type)
      {
        case ShaderDataType::Invalid : return 0;
        case ShaderDataType::Float   : return 1;
        case ShaderDataType::Float2  : return 2;
        case ShaderDataType::Float3  : return 3;
        case ShaderDataType::Float4  : return 4;
        case ShaderDataType::Mat3    : return 3;
        case ShaderDataType::Mat4    : return 4;
        case ShaderDataType::Int     : return 1;
        case ShaderDataType::Int2    : return 2;
        case ShaderDataType::Int3    : return 3;
        case ShaderDataType::Int4    : return 4;
        case ShaderDataType::Bool    : return 1;
      }
      IK_ASSERT(false, "Unkown Shader datatype!! ");
    }
    
    /// This function returns the Open GL Data type from user defined Shader data type
    /// - Parameter type: Shader data type
    [[maybe_unused]] static const char* ShaderDataTypeToString(ShaderDataType type)
    {
      switch (type)
      {
        case ShaderDataType::Invalid:  return "NoType ";
        case ShaderDataType::Float:    return "Float  ";
        case ShaderDataType::Float2:   return "Float2 ";
        case ShaderDataType::Float3:   return "Float3 ";
        case ShaderDataType::Float4:   return "Float4 ";
        case ShaderDataType::Mat3:     return "Mat3   ";
        case ShaderDataType::Mat4:     return "Mat4   ";
        case ShaderDataType::Int:      return "Int    ";
        case ShaderDataType::Int2:     return "Int2   ";
        case ShaderDataType::Int3:     return "Int3   ";
        case ShaderDataType::Int4:     return "Int4   ";
        case ShaderDataType::Bool:     return "Bool   ";
      }
      
      IK_ASSERT(false, "Unknown ShaderDataType!");
      return 0;
    }
  } // namespace BufferUtils
  // Buffer Element --------------------------------------------------------------------------------------------------
  BufferElement::BufferElement(const std::string& name, ShaderDataType type, bool normalized)
  : name(name), type(type), size(BufferUtils::ShaderDataTypeSize(type)), count(BufferUtils::GetElementCount(type)),
  offset(0), normalized(normalized)
  {
    
  }
  
  // Buffer Layout ---------------------------------------------------------------------------------------------------
  BufferLayout::BufferLayout()
  : VectorIterator<BufferElement>(m_elements)
  {
    
  }
  
  BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements)
  : VectorIterator<BufferElement>(m_elements), m_elements(elements)
  {
    CalculateOffsetAndStride();
  }
  
  void BufferLayout::CalculateOffsetAndStride()
  {
    size_t offset = 0;
    m_stride = 0;
    for (BufferElement& element : m_elements)
    {
      element.offset = offset;
      offset += element.size;
      m_stride += element.size;
    }
  }
  
  const std::vector<BufferElement>& BufferLayout::GetElements() const
  {
    return m_elements;
  }
  
  uint32_t BufferLayout::GetStride() const
  {
    return m_stride;
  }
  
  BufferLayout::BufferLayout(const BufferLayout& other)
  : VectorIterator<BufferElement>(m_elements), m_stride(other.m_stride)
  {
    for (const BufferElement& elem : other.m_elements)
    {
      m_elements.emplace_back(elem);
    }
  }
  BufferLayout& BufferLayout::operator=(const BufferLayout& other)
  {
    m_stride = other.m_stride;
    m_elements.clear();
    for (const BufferElement& elem : other.m_elements)
    {
      m_elements.emplace_back(elem);
    }
    return *this;
  }
  BufferLayout& BufferLayout::operator=(BufferLayout&& other)
  {
    m_stride = other.m_stride;
    m_elements.clear();
    for (const BufferElement& elem : other.m_elements)
    {
      m_elements.emplace_back(elem);
    }
    return *this;
  }
  
#ifdef IK_ENABLE_LOG
  void BufferLayout::DebugLog()
  {
    IK_LOG_DEBUG(LogModule::BufferLayout, "  Vertex Attributes (Stride : {0}).", std::to_string(GetStride()));
    for (const BufferElement& element : GetElements())
    {
      IK_LOG_DEBUG(LogModule::BufferLayout, "    {0}  {1}", BufferUtils::ShaderDataTypeToString(element.type), element.name);
    }
  }
#endif
} // namespace KanViz
