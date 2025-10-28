//
//  KanVizCore.hpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#pragma once

namespace KanViz
{
  /// This class stores APIs for core engine
  class CoreEngine
  {
  public:
    /// This function initializes the KanViz core engine
    static bool Initialize();
    /// This function shuts down the KanViz core engine
    static bool Shutdown();
  };
} // namespace KanViz
