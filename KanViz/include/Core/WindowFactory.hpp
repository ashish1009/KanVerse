//
//  WindowFactory.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Core/Window.hpp"

namespace KanViz
{
  /// This structure provides an API to create windows for supported platforms {macOS}.
  /// - Note Only supports macOS currently. Modify with additional platform cases if extending to other OSes.
  struct WindowFactory
  {
    [[nodiscard("The created window must be used or explicitly ignored.")]]
    static Scope<Window> Create(const WindowSpecification& specification);
  };
} // namespace KanViz
