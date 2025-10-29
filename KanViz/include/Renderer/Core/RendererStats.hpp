//
//  RendererStats.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

namespace KanViz
{
  /// This structure stores the statistics of renderer data
  struct RendererStatistics
  {
  public:
    // Member Variables ----------------------------------------------------------------------------------------------------------------------------
    uint64_t vertexBufferSize {0};  //< Size of the vertex buffer in bytes.
    uint64_t indexBufferSize {0};   //< Size of the index buffer in bytes.
    uint64_t textureBufferSize {0}; //< Size of the texture buffer in bytes.
    uint32_t vertexCount {0};       //< Total number of vertices processed.
    uint32_t indexCount {0};        //< Total number of indices processed.
    uint32_t drawCalls {0};         //< Total number of draw calls made.
    uint32_t textures {0};          //< Total number of textures loaded.
    
    struct TwoD
    {
      static const uint32_t MaxChars {16};
      
      uint32_t maxQuads {0};     //< Maximum quads that can be drawn in a frame.
      uint32_t maxCircles {0};   //< Maximum circles that can be drawn in a frame.
      uint32_t maxLines {0};     //< Maximum lines that can be drawn in a frame.
      uint32_t quads {0};        //< Current number of quads drawn.
      uint32_t circles {0};      //< Current number of circles drawn.
      uint32_t lines {0};        //< Current number of lines drawn.
      uint32_t chars {0};        //< Current number of characters drawn.
    };
    
    // Stores the 2D Specific data
    TwoD _2d;
    
    // Member Functions ----------------------------------------------------------------------------------------------------------------------------
    /// This function resets only those stats that need to be reset each frame
    void ResetEachFrame();
    /// This function resets all stats
    void ResetAll();
    
    /// This function returns the static singleton instance of RendererStatistics
    static RendererStatistics& Get();
  };
} // namespace KanViz
