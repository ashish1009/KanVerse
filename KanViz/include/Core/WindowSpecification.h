//
//  WindowSpecification.h
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Renderer/Core/RendererType.hpp"

namespace KanViz
{
  /// Stores window creation parameters for KanVerse
  struct WindowSpecification
  {
    std::string title {"KanVerse"};         ///< Window title
    RendererType rendererType;              ///< Renderer type for context

    std::uint32_t width {1600};             ///< Width in pixels
    std::uint32_t height {900};             ///< Height in pixels
    std::uint32_t samples {8};              ///< MSAA samples
    
    bool isFullScreen {false};              ///< Start in fullscreen
    bool resizable {true};                  ///< Allow resizing
    bool maximized {false};                 ///< Start maximized
    bool decorated {true};                  ///< Enable decorations (borders, title bar)
    bool hideTitleBar {false};              ///< Hide only title bar
    
#ifndef __APPLE__
    bool isVsync {false};                   ///< Enable V-Sync (unsupported on macOS)
#endif
  };
} // namespace KanViz
