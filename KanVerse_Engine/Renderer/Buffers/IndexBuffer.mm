/*!
 * @file IndexBuffer.mm
 * @brief Apple Metal implementation of IndexBuffer using metal-cpp.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/Buffers/IndexBuffer.hpp"
#include "Core/Diagnostics/Assert.hpp"
#include "Core/Application/Application.hpp"
#include "Core/Memory/MemoryTracking.hpp"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

namespace KanVerse {
  
  class MetalIndexBuffer : public IndexBuffer {
  public:
    /*!
     * @brief Allocates unified memory for indices and binds RAII allocation tracker.
     * @param indices Raw pointer to index buffer array.
     * @param count Number of elements.
     * @param type Index element type (UInt16 or UInt32).
     * @param elementSize Byte width per element.
     */
    MetalIndexBuffer(const void* indices, uint32_t count, IndexType type, size_t elementSize)
    : m_Count(count), m_Type(type), m_MemoryGuard(count * elementSize) {
      KV_CORE_ASSERT(indices != nullptr, "[KanVerse] Index source data pointer is null!");
      KV_CORE_ASSERT(count > 0, "[KanVerse] Index count must be greater than zero!");
      
      MTL::Device* device = Application::Get().GetRenderContext()->GetDevice();
      KV_CORE_ASSERT(device != nullptr, "[KanVerse] Active Metal device is null!");
      
      const size_t totalSize = count * elementSize;
      m_Buffer = device->newBuffer(indices, totalSize, MTL::ResourceStorageModeShared);
      KV_CORE_ASSERT(m_Buffer != nullptr, "[KanVerse] Failed to allocate MTL::Buffer for IndexBuffer!");
    }
    
    /*!
     * @brief Releases underlying native buffer; m_MemoryGuard automatically unwinds tracked bytes.
     */
    ~MetalIndexBuffer() override {
      if (m_Buffer) {
        m_Buffer->release();
        m_Buffer = nullptr;
      }
    }
    
    [[nodiscard]] uint32_t GetCount() const noexcept override {
      return m_Count;
    }
    
    [[nodiscard]] IndexType GetIndexType() const noexcept override {
      return m_Type;
    }
    
    [[nodiscard]] MTL::Buffer* GetNativeBuffer() const noexcept override {
      return m_Buffer;
    }
    
  private:
    MTL::Buffer*           m_Buffer{nullptr};                  /*!< Native Metal buffer pointer. */
    uint32_t               m_Count{0};                         /*!< Total number of indices. */
    IndexType              m_Type{IndexType::UInt16};          /*!< Element width identifier. */
    ScopedMemoryAllocation m_MemoryGuard{};                    /*!< Automated RAII memory tracking guard. */
  };
  
  std::unique_ptr<IndexBuffer> IndexBuffer::Create(const uint16_t* indices, uint32_t count) {
    return std::make_unique<MetalIndexBuffer>(indices, count, IndexType::UInt16, sizeof(uint16_t));
  }
  
  std::unique_ptr<IndexBuffer> IndexBuffer::Create(const uint32_t* indices, uint32_t count) {
    return std::make_unique<MetalIndexBuffer>(indices, count, IndexType::UInt32, sizeof(uint32_t));
  }
  
} // namespace KanVerse

