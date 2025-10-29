//
//  Renderer.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Renderer/Core/RendererType.hpp"

#include "Renderer/Graphics/RendererContext.hpp"

namespace KanViz
{
  /// Data type of Renderer IDs
  using RendererID = uint32_t;
  
  /// This is the static wrapper class to manage the Renderer APIs that deals with Graphics APIs directly
  /// - Note: Set the renderer API before using any other API related to rendering or window creation (Context)
  class Renderer
  {
  public:
    // Fundamentals -------------------------------------------------------------------------------
    /// This function initializes the renderer
    /// - Parameter rendererType: current renderer type
    /// - Parameter windowPointer: window pointer
    /// - Note: Should be set before creating renderer context or using any renderer API
    static void Initialize(RendererType rendererType, void* windowPointer);
    /// This function shuts down the renderer
    static void Shutdown();
    
    /// This function updates the renderer each frame
    static void OnUpdate();
        
    // Renderer API functions ---------------------------------------------------------------------
    /// This function returns the current renderer API type
    static RendererType GetCurrentRendererAPI();
    
    // Getters ------------------------------------------------------------------------------------
    /// This function returns the rednerer context pointer
    static RendererContext* GetRendererContext();

    // Error Handling ------------------------------------------------------------------------------------------------------------------------------
    /// This function checks the renderer API error
    /// - Parameter context: Error message to be printed
    static void CheckError(std::string_view context);
  };
  
  // Error Message define
#define IK_CHECK_RENDERER_ERROR() KanViz::Renderer::CheckError(__PRETTY_FUNCTION__);
} // namespace KanViz
