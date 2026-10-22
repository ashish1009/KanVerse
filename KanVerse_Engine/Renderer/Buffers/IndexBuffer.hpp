/*!
 * @file IndexBuffer.hpp
 * @brief RHI index buffer interface wrapping Metal GPU memory.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Core/Memory/MemoryTracking.hpp"
#include <cstdint>
#include <memory>

namespace MTL {
  class Buffer;
}

namespace KanVerse {
  
  /*!
   * @enum IndexType
   * @brief Index element width (16-bit or 32-bit unsigned integers).
   */
  enum class IndexType : uint8_t {
    UInt16 = 0,
    UInt32
  };
  
  /*!
   * @class IndexBuffer
   * @brief RAII abstraction for GPU index/element buffers.
   */
  class IndexBuffer {
  public:
    virtual ~IndexBuffer() = default;
    
    /*!
     * @brief Queries the element count in this index buffer.
     */
    [[nodiscard]] virtual uint32_t GetCount() const noexcept = 0;
    
    /*!
     * @brief Queries the index data type width (16-bit vs 32-bit).
     */
    [[nodiscard]] virtual IndexType GetIndexType() const noexcept = 0;
    
    /*!
     * @brief Accesses the native underlying MTL::Buffer pointer.
     */
    [[nodiscard]] virtual MTL::Buffer* GetNativeBuffer() const noexcept = 0;
    
    /*!
     * @brief Factory creating an IndexBuffer populated with 16-bit indices.
     * @param indices Pointer to uint16_t array.
     * @param count Number of indices.
     * @return Unique pointer owning the IndexBuffer.
     */
    [[nodiscard]] static std::unique_ptr<IndexBuffer> Create(const uint16_t* indices, uint32_t count);
    
    /*!
     * @brief Factory creating an IndexBuffer populated with 32-bit indices.
     * @param indices Pointer to uint32_t array.
     * @param count Number of indices.
     * @return Unique pointer owning the IndexBuffer.
     */
    [[nodiscard]] static std::unique_ptr<IndexBuffer> Create(const uint32_t* indices, uint32_t count);
  };
  
} // namespace KanVerse
