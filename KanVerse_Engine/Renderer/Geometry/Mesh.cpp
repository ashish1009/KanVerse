/*!
 * @file Mesh.cpp
 * @brief Implementation of high-level Mesh subsystem.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/Geometry/Mesh.hpp"
#include <algorithm>

namespace KanVerse {
  
  Mesh::Mesh(const Vertex* vertices,
             size_t vertexCount,
             const uint16_t* indices,
             uint32_t indexCount,
             const BufferLayout& layout) {
    m_VertexBuffer = VertexBuffer::Create(vertices, vertexCount * sizeof(Vertex));
    m_VertexBuffer->SetLayout(layout);
    
    m_IndexBuffer = IndexBuffer::Create(indices, indexCount);
    
    Submesh rootSubmesh;
    rootSubmesh.NodeName = "RootSubmesh";
    rootSubmesh.BaseVertex = 0;
    rootSubmesh.BaseIndex = 0;
    rootSubmesh.IndexCount = indexCount;
    rootSubmesh.MaterialIndex = 0;
    
    for (size_t i = 0; i < vertexCount; ++i) {
      rootSubmesh.BoundingBox.Encapsulate(vertices[i].position);
    }
    
    m_Submeshes.push_back(rootSubmesh);
  }
  
  std::shared_ptr<Mesh> Mesh::CreateChamferBox(float size,
                                               float chamfer,
                                               const BufferLayout& layout) {
    MeshData meshData = ChamferBoxGenerator::Generate(size, chamfer);
    return std::make_shared<Mesh>(
                                  meshData.vertices.data(),
                                  meshData.vertices.size(),
                                  meshData.indices.data(),
                                  static_cast<uint32_t>(meshData.indices.size()),
                                  layout
                                  );
  }
  
} // namespace KanVerse
