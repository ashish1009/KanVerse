//
//  Factories.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

// Forward Declarations
class GLFWwindow;

namespace KanViz
{
  // KanViz Forward Declarations
  class RendererContext;
  class RendererAPI;
  
  /// This structure provides a method to create the renderer context instance based on the renderer API supported by the engine.
  struct RendererContextFactory
  {
    /// This function creates the renderer context instance based on the renderer API supported by the Engine
    /// - Parameter windowPtr: Pointer to the GLFW window.
    /// - Returns: A scope that manages the lifetime of the created RendererContext instance.
    /// - Note: Returns nullptr for release mode if the assertion macro is not defined.
    [[nodiscard]] static Scope<RendererContext> Create(GLFWwindow* windowPtr);
  };
  
  /// This structure provides a method to create the renderer API instance based on the supported APIs.
  struct RendererAPIFactory
  {
    /// This function creates the Renderer API instance based on the Supported APIs
    /// - Returns: A scope that manages the lifetime of the created RendererAPI instance.
    /// - Note: Returns nullptr for release mode if the assertion macro is not defined.
    [[nodiscard]] static Scope<RendererAPI> Create();
  };
} // namespace KanViz
