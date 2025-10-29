//
//  Buffer.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

namespace KanViz
{
  /// This structure stores the data in a buffer as bytes. Allocate the memory in heap on cosntruction and de-allocate the memory on destruction
  struct Buffer
  {
  public:
    // Member variables ----------------------------------------------------------------------------------------------------------------------------
    std::byte* data {nullptr};
    uint32_t size {0};
    
    // Member Functions ----------------------------------------------------------------------------------------------------------------------------
    constexpr Buffer() noexcept = default;
    /// This constructor assign the pointer comming in the argument as 'data'
    /// - Parameters:
    ///   - data: data pointer as std bytes:
    ///   - size: size of data:
    Buffer(std::byte* data, uint32_t size) noexcept;
    /// This is the copy constructor thet deep copies the data from 'other'
    /// - Parameter other: other Buffer:
    Buffer(const Buffer& other);
    /// This is the move constructor that moves the buffer from other to this
    /// - Parameter other: other Buffer:
    Buffer(Buffer&& other) noexcept;
    
    /// This destructor destroy the memory of buffer and clear it
    ~Buffer();
    
    /// This functions deletes the Buffer and make size 0
    void Clear();
    
    /// This function sets the memory of buffer to 0
    void ZeroInitialize();
    /// This funciton writes the data in the buffer
    /// - Parameters:
    ///   - data: Data point to be written in the buffer as void*:
    ///   - size: Size of data to be copied:
    ///   - offset:  Offset from where data to be copied. Offset from 'data':
    void Write(void* data, uint32_t size, uint32_t offset = 0);
    /// This function allocates the memory to buffer of size 'size'
    /// - Parameter size: size of new buffer:
    void Allocate(uint32_t size);
    
    /// This function returns the size of buffer
    constexpr uint32_t GetSize() const;
    
    /// This function returns true if Data is not nullptr
    operator bool() const;
    /// This function returns the byte reference at position index
    /// - Parameter index: Index of position:
    std::byte& operator[](uint32_t index);
    /// This function returns the byte value at position index
    /// - Parameter index: Index of position:
    std::byte operator[](uint32_t index) const;
    
    /// This function assign the 'other' buffer to this
    /// - Parameter other: Assign Buffer other to this Buffer:
    /// - Important: New memory allocation is done (Deep Copy)
    Buffer& operator =(const Buffer& other);
    /// This function moves the 'other' buffer to this
    /// - Parameter other: Assign Buffer other to this Buffer:
    /// - Important: New memory allocation is done (Deep Copy)
    Buffer& operator =(Buffer&& other);
    
    /// This function creates new instance of Buffer. Copy the 'data' and return the newly created buffer
    /// - Parameters:
    ///   - data: Data pointer (void*) to be copied:
    ///   - size: size of data:
    [[nodiscard]] static Buffer Copy(void* data, uint32_t size);
    /// This function returns the total allocated size in entire program
    static uint32_t GetTotalAllocations() noexcept;
    
    /// This function reads the buffer at specific offset as T
    /// - Parameter - offset from where buffer to be read:
    template<typename T> T& Read(uint32_t offset) { return *(T*)(data + offset); }
    /// This function reads thr buffer from begining as T
    template<typename T> constexpr T* As() noexcept { return (T*)data; }
    
  private:
    /// This function dealocate the memory of buffer
    void Dealocate();
    
    // Stores the Total allocation made in program using Buffer class
    inline static uint32_t s_allocatedBytes {0};
  };
} // namespace KanViz
