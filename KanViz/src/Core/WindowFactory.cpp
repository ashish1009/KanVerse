//
//  WindowFactory.cpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#include "WindowFactory.hpp"

#include "Platform/MacOS/MacWindiow.hpp"

namespace KanViz
{
  Scope<Window> WindowFactory::Create(const WindowSpecification &specification)
  {
#ifdef __APPLE__
    return CreateScope<MacWindow>(specification);
#else
    IK_FORCE_ASSERT(false, "Unsupported operating system.");
#endif
  }
} // namespace KanViz
