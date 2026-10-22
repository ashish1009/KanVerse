/*!
 * @file RenderCommand.hpp
 * @brief Low-level immediate rendering commands executing on active render encoders.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Renderer/Buffers/IndexBuffer.hpp"
#include "Renderer/Buffers/VertexBuffer.hpp"
#include "Renderer/Pipeline/Pipeline.hpp"

#include <cstdint>
#include <memory>

namespace MTL {
  class RenderCommandEncoder;
}

namespace KanVerse {
  
  /*!
   * @class RenderCommand
   * @brief Static gateway dispatching low-level GPU drawing and pipeline bind commands via metal-cpp.
   */
  class RenderCommand {
  public:
    /*!
     * @brief Binds pipeline, vertex buffer, and executes single-instance indexed primitive drawing.
     * @param encoder Active Metal render command encoder.
     * @param pipeline Pipeline state defining shaders, blend states, and depth tests.
     * @param vertexBuffer Bound vertex buffer holding interleaved vertex streams.
     * @param indexBuffer Bound index buffer defining primitive topology.
     */
    static void DrawIndexed(MTL::RenderCommandEncoder* encoder,
                            const std::shared_ptr<Pipeline>& pipeline,
                            const std::shared_ptr<VertexBuffer>& vertexBuffer,
                            const std::shared_ptr<IndexBuffer>& indexBuffer);
    
    /*!
     * @brief Encodes a hardware-instanced indexed draw call using GPU primitive replication.
     * @param encoder Active Metal render command encoder.
     * @param pipeline Pipeline state defining shaders, blend states, and depth tests.
     * @param vertexBuffer Bound vertex buffer holding interleaved vertex streams.
     * @param indexBuffer Bound index buffer defining primitive topology.
     * @param instanceCount Total number of instances to render in parallel on chip.
     */
    static void DrawIndexedInstanced(MTL::RenderCommandEncoder* encoder,
                                     const std::shared_ptr<Pipeline>& pipeline,
                                     const std::shared_ptr<VertexBuffer>& vertexBuffer,
                                     const std::shared_ptr<IndexBuffer>& indexBuffer,
                                     uint32_t instanceCount);
  };
  
} // namespace KanVerse
