//
//  BufferLayout.hpp
//  KanViz
//
//  Created by Ashish . on 04/11/24.
//

#pragma once

namespace KanViz
{
  /// This enum stores the data type to be stored in shader
  enum class ShaderDataType : uint8_t
  {
    Invalid = 0,
    Float, Float2, Float3, Float4,
    Mat3, Mat4,
    Int, Int2, Int3, Int4,
    Bool
  };
  
  // Buffer Element --------------------------------------------------------------------------------------------------------------------------------
  /// This structure stores all the propertes of a vertex element in a buffer.
  struct BufferElement
  {
    std::string name {""};
    ShaderDataType type {ShaderDataType::Invalid};
    uint32_t size {0};
    uint32_t count {0};
    size_t offset {0};
    bool normalized {false};

    /// This constructor creates the vertex buffer element with name, type and normalized flag
    /// - Parameters:
    ///   - name: name of element
    ///   - type: type of element
    ///   - normalized: flag is element normalized
    BufferElement(const std::string& name, ShaderDataType type, bool normalized = false);
  };

  // Buffer Layout ---------------------------------------------------------------------------------------------------------------------------------
  /// This class stores the layout of vertex Buffer in a vector (stores each elements)
  class BufferLayout : public VectorIterator<BufferElement>
  {
  public:
    /// This is the default constructor
    BufferLayout();
    /// This Costructor initialize the vector of layout elements with initializer list
    /// - Parameter elements: Element initializer list
    BufferLayout(const std::initializer_list<BufferElement>& elements);
    
    /// Copy and Move constructors and operator
    BufferLayout(const BufferLayout& other);
    BufferLayout(BufferLayout&& other);
    BufferLayout& operator=(const BufferLayout& other);
    BufferLayout& operator=(BufferLayout&& other);
    
    /// This function returns the elements vector
    const std::vector<BufferElement>& GetElements() const;
    /// This function returns the stride value
    uint32_t GetStride() const;
    
#ifdef IK_DEBUG
    /// This funcion print the debug log
    void DebugLog();
#endif
    
  private:
    /// This function calculates the offset of element and update the stride value
    void CalculateOffsetAndStride();
    
    std::vector<BufferElement> m_elements;
    uint32_t m_stride {0};
  };

} // namespace KanViz
