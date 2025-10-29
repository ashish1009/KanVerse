//
//  RendererStats.cpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#include "RendererStats.hpp"

namespace KanViz
{
  void RendererStatistics::ResetEachFrame()
  {
    // Reset per-frame statistics
    drawCalls = 0;
    indexCount = 0;
    vertexCount = 0;
    
    _2d.quads = 0;
    _2d.circles = 0;
    _2d.lines = 0;
    _2d.chars = 0;
  }
  
  void RendererStatistics::ResetAll()
  {
    // Reset all statistics including buffer sizes and max values
    ResetEachFrame();
    
    vertexBufferSize = 0;
    indexBufferSize = 0;
    textureBufferSize = 0;
    
    _2d.maxQuads = 0;
    _2d.maxCircles = 0;
    _2d.maxLines = 0;
  }
  
  RendererStatistics& RendererStatistics::Get()
  {
    // Return the singleton instance of RendererStatistics
    static RendererStatistics instance;
    return instance;
  }
} // namespace KanViz
