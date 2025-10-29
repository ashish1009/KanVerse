//
//  RendererContext.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Renderer/Graphics/Factories.hpp"

namespace KanViz
{
  /// This is the interface class to create renderer context instance
  class RendererContext
  {
  public:
    /// Default virtual destructor for proper cleanup of derived classes.
    virtual ~RendererContext() = default;
    /// This function swaps the front and back buffers to display the rendered frame.
    /// - Note: This should be called at the end of each frame to update the display.
    virtual void SwapBuffers() = 0;
  };
} // namespace KanViz
