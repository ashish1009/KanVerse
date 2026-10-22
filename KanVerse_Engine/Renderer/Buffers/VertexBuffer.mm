/*!
 * @file VertexBuffer.mm
 * @brief Apple Metal implementation of VertexBuffer using metal-cpp.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/Buffers/VertexBuffer.hpp"
#include "Core/Diagnostics/Assert.hpp"
#include "Core/Application/Application.hpp"
#include "Core/Memory/MemoryTracking.hpp"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

namespace KanVerse {
  
  class MetalVertexBuffer : public VertexBuffer {
  public:
    /*!
     * @brief Allocates unified memory buffer and registers byte tracking via RAII guard.
     * @param vertices Pointer to source vertex data.
     * @param size Total buffer capacity in bytes.
     */
    MetalVertexBuffer(const void* vertices, size_t size)
    : m_Size(size), m_MemoryGuard(size) {
      KV_CORE_ASSERT(size > 0, "[KanVerse] Cannot allocate a zero-sized VertexBuffer!");
      MTL::Device* device = Application::Get().GetRenderContext()->GetDevice();
      KV_CORE_ASSERT(device != nullptr, "[KanVerse] Active Metal device is null!");
      
      m_Buffer = device->newBuffer(vertices, size, MTL::ResourceStorageModeShared);
      KV_CORE_ASSERT(m_Buffer != nullptr, "[KanVerse] Failed to allocate MTL::Buffer for VertexBuffer!");
    }
    
    /*!
     * @brief Releases underlying native buffer; m_MemoryGuard automatically unwinds tracked bytes.
     */
    ~MetalVertexBuffer() override {
      if (m_Buffer) {
        m_Buffer->release();
        m_Buffer = nullptr;
      }
    }
    
    void SetLayout(const BufferLayout& layout) override {
      m_Layout = layout;
    }
    
    [[nodiscard]] const BufferLayout& GetLayout() const noexcept override {
      return m_Layout;
    }
    
    [[nodiscard]] MTL::Buffer* GetNativeBuffer() const noexcept override {
      return m_Buffer;
    }
    
    [[nodiscard]] size_t GetSize() const noexcept override {
      return m_Size;
    }
    
  private:
    MTL::Buffer*           m_Buffer{nullptr};     /*!< Native Apple Metal buffer pointer. */
    size_t                 m_Size{0};             /*!< Total allocated size in bytes. */
    BufferLayout           m_Layout;              /*!< Stride and attribute reflection schema. */
    ScopedMemoryAllocation m_MemoryGuard{};       /*!< Automated RAII memory tracking guard. */
  };
  
  std::unique_ptr<VertexBuffer> VertexBuffer::Create(const void* vertices, size_t size) {
    return std::make_unique<MetalVertexBuffer>(vertices, size);
  }
  
} // namespace KanVerse
