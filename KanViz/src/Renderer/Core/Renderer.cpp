//
//  Renderer.cpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#include "Renderer.hpp"

namespace KanViz
{
  /// This structure stores the renderer data
  struct RendererData
  {
    RendererType rendererType {RendererType::Invalid};
    
    void Reset()
    {
    }
  };
  static RendererData s_rendererData;
  
  void Renderer::Initialize(RendererType rendererType, void* windowPointer)
  {
    IK_ASSERT(rendererType != RendererType::Invalid, "Invalid Renderer Type");
    IK_LOG_INFO(LogModule::Renderer, "Initializing the Renderer with backedn API type '{0}'", RendererUtils::GetRendererTypeString(rendererType));
    
    s_rendererData.rendererType = rendererType;
  }
  void Renderer::Shutdown()
  {
    IK_LOG_INFO(LogModule::Renderer, "Shutting down the Renderer");
    s_rendererData.Reset();
  }
  
  void Renderer::OnUpdate()
  {

  }
  
  void Renderer::ResizeFramebuffer(int32_t framebufferWidth, int32_t framebufferHeight)
  {

  }
  
  // Fundamentals -------------------------------------------------------------------------------
  RendererType Renderer::GetCurrentRendererAPI()
  {
    return s_rendererData.rendererType;
  }
  
  // Getters ------------------------------------------------------------------------------------

  // Renderer Controllers ------------------------------------------------------------------------------------------------------------------------
  void Renderer::CheckError(std::string_view context)
  {

  }
} // namespace KanViz
