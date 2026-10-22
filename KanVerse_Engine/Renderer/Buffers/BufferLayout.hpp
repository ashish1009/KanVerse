/*!
 * @file BufferLayout.hpp
 * @brief Memory layout reflection and descriptor types for GPU vertex attributes.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <cassert>

namespace KanVerse {
  
  /*!
   * @enum ShaderDataType
   * @brief Underlying data formats for vertex shader input attributes.
   */
  enum class ShaderDataType : uint8_t {
    None = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Mat3,
    Mat4,
    Int,
    Int2,
    Int3,
    Int4,
    Bool
  };
  
  /*!
   * @brief Calculates size in bytes for a given shader attribute type adhering to Apple Silicon / SIMD 16-byte alignment.
   * @param type The shader attribute data format.
   * @return Size in bytes.
   */
  [[nodiscard]] constexpr uint32_t ShaderDataTypeSize(ShaderDataType type) noexcept {
    switch (type) {
      case ShaderDataType::Float:    return 4;
      case ShaderDataType::Float2:   return 8;
      case ShaderDataType::Float3:   return 16; // 16-byte aligned in SIMD/MSL float3
      case ShaderDataType::Float4:   return 16;
      case ShaderDataType::Mat3:     return 4 * 3 * 4;
      case ShaderDataType::Mat4:     return 4 * 4 * 4;
      case ShaderDataType::Int:      return 4;
      case ShaderDataType::Int2:     return 8;
      case ShaderDataType::Int3:     return 16;
      case ShaderDataType::Int4:     return 16;
      case ShaderDataType::Bool:     return 1;
      default: break;
    }
    return 0;
  }
  
  /*!
   * @struct BufferElement
   * @brief Descriptive metadata for a discrete element within an interleaved vertex buffer.
   */
  struct BufferElement {
    std::string    Name;
    ShaderDataType Type;
    uint32_t       Size;
    size_t         Offset{0};
    bool           Normalized{false};
    
    BufferElement() = default;
    
    BufferElement(ShaderDataType type, std::string name, bool normalized = false)
    : Name(std::move(name)),
    Type(type),
    Size(ShaderDataTypeSize(type)),
    Offset(0),
    Normalized(normalized) {}
    
    [[nodiscard]] constexpr uint32_t GetComponentCount() const noexcept {
      switch (Type) {
        case ShaderDataType::Float:   return 1;
        case ShaderDataType::Float2:  return 2;
        case ShaderDataType::Float3:  return 3;
        case ShaderDataType::Float4:  return 4;
        case ShaderDataType::Mat3:    return 9;
        case ShaderDataType::Mat4:    return 16;
        case ShaderDataType::Int:     return 1;
        case ShaderDataType::Int2:    return 2;
        case ShaderDataType::Int3:    return 3;
        case ShaderDataType::Int4:    return 4;
        case ShaderDataType::Bool:    return 1;
        default: break;
      }
      return 0;
    }
  };
  
  /*!
   * @class BufferLayout
   * @brief Encapsulates an ordered collection of BufferElements with automatic stride calculation.
   */
  class BufferLayout {
  public:
    BufferLayout() = default;
    
    BufferLayout(std::initializer_list<BufferElement> elements)
    : m_Elements(elements) {
      CalculateOffsetsAndStride();
    }
    
    [[nodiscard]] uint32_t GetStride() const noexcept { return m_Stride; }
    [[nodiscard]] const std::vector<BufferElement>& GetElements() const noexcept { return m_Elements; }
    
    [[nodiscard]] auto begin() noexcept { return m_Elements.begin(); }
    [[nodiscard]] auto end() noexcept { return m_Elements.end(); }
    [[nodiscard]] auto begin() const noexcept { return m_Elements.begin(); }
    [[nodiscard]] auto end() const noexcept { return m_Elements.end(); }
    
  private:
    void CalculateOffsetsAndStride() {
      size_t offset = 0;
      m_Stride = 0;
      for (auto& element : m_Elements) {
        element.Offset = offset;
        offset += element.Size;
        m_Stride += element.Size;
      }
    }
    
    std::vector<BufferElement> m_Elements;
    uint32_t                   m_Stride{0};
  };
  
} // namespace KanVerse
