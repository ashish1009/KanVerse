//
//  RendererAPI.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Renderer/Graphics/Factories.hpp"

namespace IKan
{
  /// This class is the interface for all the renderer APIs.
  /// It defines the core functions that any rendering backend must implement.
  class RendererAPI
  {
  public:
    /// Default virtual destructor for proper cleanup of derived classes.
    virtual ~RendererAPI() = default;
    
    /// This function checks the renderer API error
    /// - Parameter context: Error message to be printed
    virtual void CheckError(std::string_view context) const = 0;
  };
} // namespace IKan
