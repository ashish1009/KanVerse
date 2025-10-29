//
//  RendererType.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

namespace KanViz
{
  /// This enum stores the Supported rendering backends for engine
  enum class RendererType
  {
    Invalid, OpenGL
  };
  
  namespace RendererUtils
  {
#ifdef IK_DEBUG
    std::string_view GetRendererTypeString(RendererType rendererType);
#endif
  } // namespace RendererUtils
} // namespace KanViz
