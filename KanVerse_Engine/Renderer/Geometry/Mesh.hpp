/*!
 * @file Mesh.hpp
 * @brief High-level mesh container managing submeshes, vertex streams, and index buffers.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Renderer/Buffers/VertexBuffer.hpp"
#include "Renderer/Buffers/IndexBuffer.hpp"
#include "Renderer/Geometry/SubMesh.hpp"
#include "Renderer/Procedural/ChamferBox.hpp"

#include <memory>
#include <vector>

namespace KanVerse {
  
  /*!
   * @class Mesh
   * @brief Manages GPU buffer lifecycles and partitioned submesh topologies.
   */
  class Mesh {
  public:
    /*!
     * @brief Constructs an empty mesh instance.
     */
    Mesh() = default;
    
    /*!
     * @brief Constructs a mesh populated with vertices, indices, and a root submesh.
     * @param vertices Raw vertex data array.
     * @param vertexCount Total number of vertices.
     * @param indices Raw 16-bit indices array.
     * @param indexCount Total number of indices.
     * @param layout Vertex attribute layout descriptor.
     */
    Mesh(const Vertex* vertices,
         size_t vertexCount,
         const uint16_t* indices,
         uint32_t indexCount,
         const BufferLayout& layout);
    
    /*!
     * @brief Accesses the bound GPU vertex buffer.
     */
    [[nodiscard]] const std::shared_ptr<VertexBuffer>& GetVertexBuffer() const noexcept { return m_VertexBuffer; }
    
    /*!
     * @brief Accesses the bound GPU index buffer.
     */
    [[nodiscard]] const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const noexcept { return m_IndexBuffer; }
    
    /*!
     * @brief Accesses the submesh partition collection.
     */
    [[nodiscard]] const std::vector<Submesh>& GetSubmeshes() const noexcept { return m_Submeshes; }
    
    /*!
     * @brief Accesses mutable submesh partition collection.
     */
    [[nodiscard]] std::vector<Submesh>& GetSubmeshes() noexcept { return m_Submeshes; }
    
    /*!
     * @brief Factory creating a procedural chamfered cube mesh.
     * @param size Edge length.
     * @param chamfer Chamfer bevel depth.
     * @param layout Vertex attribute layout descriptor.
     * @return Shared pointer to newly constructed Mesh.
     */
    [[nodiscard]] static std::shared_ptr<Mesh> CreateChamferBox(float size,
                                                                float chamfer,
                                                                const BufferLayout& layout);
    
  private:
    std::shared_ptr<VertexBuffer> m_VertexBuffer;
    std::shared_ptr<IndexBuffer>  m_IndexBuffer;
    std::vector<Submesh>          m_Submeshes;
  };
  
} // namespace KanVerse
