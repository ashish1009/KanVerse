/*!
 * @file RenderCommand.mm
 * @brief Implementation of low-level GPU render commands using metal-cpp.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/Renderer/RenderCommand.hpp"
#include "Core/Diagnostics/Assert.hpp"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

namespace KanVerse {
  
  void RenderCommand::DrawIndexed(MTL::RenderCommandEncoder* encoder,
                                  const std::shared_ptr<Pipeline>& pipeline,
                                  const std::shared_ptr<VertexBuffer>& vertexBuffer,
                                  const std::shared_ptr<IndexBuffer>& indexBuffer) {
    KV_CORE_ASSERT(encoder != nullptr,      "[KanVerse RenderCommand] Active MTL::RenderCommandEncoder is null!");
    KV_CORE_ASSERT(pipeline != nullptr,     "[KanVerse RenderCommand] Pipeline is null!");
    KV_CORE_ASSERT(vertexBuffer != nullptr, "[KanVerse RenderCommand] VertexBuffer is null!");
    KV_CORE_ASSERT(indexBuffer != nullptr,  "[KanVerse RenderCommand] IndexBuffer is null!");
    
    // 1. Bind raster and depth pipeline state
    pipeline->Bind(encoder);
    
    // 2. Bind vertex stream at buffer index 0
    encoder->setVertexBuffer(vertexBuffer->GetNativeBuffer(), 0, 0);
    
    // 3. Resolve index precision format
    const MTL::IndexType indexType = (indexBuffer->GetIndexType() == IndexType::UInt16)
    ? MTL::IndexTypeUInt16
    : MTL::IndexTypeUInt32;
    
    // 4. Encode non-instanced indexed primitive draw call
    encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle,
                                   indexBuffer->GetCount(),
                                   indexType,
                                   indexBuffer->GetNativeBuffer(),
                                   0);
  }
  
  void RenderCommand::DrawIndexedInstanced(MTL::RenderCommandEncoder* encoder,
                                           const std::shared_ptr<Pipeline>& pipeline,
                                           const std::shared_ptr<VertexBuffer>& vertexBuffer,
                                           const std::shared_ptr<IndexBuffer>& indexBuffer,
                                           uint32_t instanceCount) {
    KV_CORE_ASSERT(encoder != nullptr,      "[KanVerse RenderCommand] Active MTL::RenderCommandEncoder is null!");
    KV_CORE_ASSERT(pipeline != nullptr,     "[KanVerse RenderCommand] Pipeline is null!");
    KV_CORE_ASSERT(vertexBuffer != nullptr, "[KanVerse RenderCommand] VertexBuffer is null!");
    KV_CORE_ASSERT(indexBuffer != nullptr,  "[KanVerse RenderCommand] IndexBuffer is null!");
    
    if (instanceCount == 0) return;
    
    // 1. Bind raster and depth pipeline state
    pipeline->Bind(encoder);
    
    // 2. Bind vertex stream at buffer index 0 (CRITICAL!)
    encoder->setVertexBuffer(vertexBuffer->GetNativeBuffer(), 0, 0);
    
    // 3. Resolve index precision format
    const MTL::IndexType indexType = (indexBuffer->GetIndexType() == IndexType::UInt16)
    ? MTL::IndexTypeUInt16
    : MTL::IndexTypeUInt32;
    
    // 4. Hardware-instanced indexed primitive draw call using the index buffer
    encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle,
                                   indexBuffer->GetCount(),
                                   indexType,
                                   indexBuffer->GetNativeBuffer(),
                                   0,
                                   instanceCount);
  }
} // namespace KanVerse
