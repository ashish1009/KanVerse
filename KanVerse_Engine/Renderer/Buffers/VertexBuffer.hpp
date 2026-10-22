/*!
 * @file VertexBuffer.hpp
 * @brief RHI vertex buffer interface wrapping Metal GPU memory.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Core/Memory/MemoryTracking.hpp"
#include "Renderer/Buffers/BufferLayout.hpp"
#include <cstdint>
#include <memory>

namespace MTL {
  class Buffer;
}

namespace KanVerse {
  
  /*!
   * @class VertexBuffer
   * @brief RAII abstraction for GPU vertex buffers allocated in unified memory.
   */
  class VertexBuffer {
  public:
    virtual ~VertexBuffer() = default;
    
    /*!
     * @brief Binds buffer layout defining vertex attribute bindings.
     * @param layout Description of vertex memory strides and offsets.
     */
    virtual void SetLayout(const BufferLayout& layout) = 0;
    
    /*!
     * @brief Queries the bound attribute layout.
     */
    [[nodiscard]] virtual const BufferLayout& GetLayout() const noexcept = 0;
    
    /*!
     * @brief Accesses the native underlying MTL::Buffer pointer.
     */
    [[nodiscard]] virtual MTL::Buffer* GetNativeBuffer() const noexcept = 0;
    
    /*!
     * @brief Size of the allocated buffer in bytes.
     */
    [[nodiscard]] virtual size_t GetSize() const noexcept = 0;
    
    /*!
     * @brief Factory method creating a VertexBuffer initialized with raw vertex memory.
     * @param vertices Pointer to source vertex data.
     * @param size Size in bytes to allocate.
     * @return Unique pointer owning the VertexBuffer.
     */
    [[nodiscard]] static std::unique_ptr<VertexBuffer> Create(const void* vertices, size_t size);
  };
  
} // namespace KanVerse
