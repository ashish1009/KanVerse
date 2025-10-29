//
//  Renderer.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Renderer/Core/RendererType.hpp"
#include "Renderer/Core/RendererCommandQueue.hpp"

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

    // Render Command Queue ------------------------------------------------------------------------------------------------------------------------
    /// This function executes all the renderer commands in the queue
    /// - Parameter func: function pointer lambda to be submitted
    template <typename FuncT> static void Submit(FuncT&& func)
    {
#ifdef RENDER_COMMAND_QUEUE_ENABLED
      using RenderCommandFn = RenderCommandQueue::RenderCommandFn;
      
      // Create the render command lambda
      static RenderCommandFn renderCommand = [](void *ptr)
      {
        // Cast the void* function pointer to type of function in which it is submitted
        FuncT* pFunc = static_cast<FuncT*>(ptr);
        
        (*pFunc)();       // Execute the function pointer
        pFunc->~FuncT();  // Call the destructor
      };
      
      // Get the buffer pointer from queue
      RenderCommandQueue* renderCommandQueue = GetRenderCommandQueue();
      if (renderCommandQueue)
      {
        void* storageBuffer = renderCommandQueue->Allocate(renderCommand, sizeof(func));
        if (!storageBuffer)
        {
          throw std::runtime_error("Failed to allocate memory for render command");
        }
        
        // Store the function in pre-allocated memory
        new (storageBuffer) FuncT(std::forward<FuncT>(func));  // Placement new
      }
      else
      {
        func();  // Directly execute the function if not using command queue
      }
      
#else
      func();  // Directly execute the function if not using command queue
#endif
    }
    
    /// This function executes all renderer commands in the queue and waits until rendering is complete
    static void WaitAndRender();

    // Error Handling ------------------------------------------------------------------------------------------------------------------------------
    /// This function checks the renderer API error
    /// - Parameter context: Error message to be printed
    static void CheckError(std::string_view context);
    
  private:
    /// This function returns the render command queue buffer
    static RenderCommandQueue* GetRenderCommandQueue();
  };
  
  // Error Message define
#define IK_CHECK_RENDERER_ERROR() KanViz::Renderer::CheckError(__PRETTY_FUNCTION__);
} // namespace KanViz
