//
//  Buffer.cpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#include "Buffer.hpp"

namespace KanViz
{
  Buffer::Buffer(std::byte* data, uint32_t size) noexcept : data(data), size(size)
  {
    
  }
  Buffer::Buffer(const Buffer& other)
  : size(other.size)
  {
    data = new std::byte[size];
    memcpy(data, other.data, size);
    s_allocatedBytes += size;
  }
  Buffer::Buffer(Buffer&& other) noexcept
  : data(other.data),  size(other.size)
  {
    other.size = 0;
    other.data = nullptr;
  }
  
  Buffer::~Buffer()
  {
    Clear();
  }
  
  void Buffer::Clear()
  {
    if (data)
    {
      Dealocate();
    }
    
    size = 0;
    data = nullptr;
  }
  
  void Buffer::ZeroInitialize()
  {
    if (data)
    {
      memset(data, 0, size);
    }
  }
  
  void Buffer::Write(void* newData, uint32_t allocatedSize, uint32_t offset)
  {
    if (allocatedSize == 0)
    {
      return;
    }
    IK_ASSERT(offset + allocatedSize <= size or offset > size, "Buffer overflow!");
    memcpy(data + offset, newData, allocatedSize);
  }
  void Buffer::Allocate(uint32_t allocated_size)
  {
    if (allocated_size == 0)
    {
      return;
    }
    
    Clear();
    size = allocated_size;
    data = new std::byte[size];
    
    s_allocatedBytes += size;
  }
  
  constexpr uint32_t Buffer::GetSize() const
  {
    return size;
  }
  
  Buffer::operator bool () const
  {
    return data;
  }
  
  std::byte& Buffer::operator[](uint32_t index)
  {
    return data[index];
  }
  
  std::byte Buffer::operator[](uint32_t index) const
  {
    return data[index];
  }
  
  Buffer& Buffer::operator =(const Buffer& other)
  {
    Clear();
    Allocate(other.size);
    Write((void*)other.data, other.size);
    return *this;
  }
  
  Buffer& Buffer::operator =(Buffer&& other)
  {
    Clear();
    
    size = other.size;
    data = other.data;
    
    other.size = 0;
    other.data = nullptr;
    
    return *this;
  }
  
  Buffer Buffer::Copy(void* data, uint32_t size)
  {
    Buffer buffer;
    
    if (size == 0)
    {
      return buffer;
    }
    
    buffer.Allocate(size);
    memcpy(buffer.data, data, size);
    return buffer;
  }
  
  uint32_t Buffer::GetTotalAllocations() noexcept
  {
    return s_allocatedBytes;
  }
  
  void Buffer::Dealocate()
  {
    s_allocatedBytes -= size;
    delete[] data;
  }
} // namespace KanViz
