//
//  RendererType.cpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#include "RendererType.hpp"

namespace KanViz::RendererUtils
{
#ifdef IK_DEBUG
  std::string_view GetRendererTypeString(RendererType rendererType)
  {
    switch (rendererType)
    {
      case RendererType::OpenGL: return "OpenGL";
      case RendererType::Metal : return "Metal";
      case RendererType::Invalid:
      default:
        IK_ASSERT(false, "Invalid Renderer Type");
    }
  }
#endif
} // namespace KanViz::RendererUtils
