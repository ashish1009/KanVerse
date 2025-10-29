//
//  Renderer.cpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#include "Renderer.hpp"

#include "Renderer/Graphics/RendererAPI.hpp"

namespace KanViz
{
  /// This structure stores the renderer data
  struct RendererData
  {
    RendererType rendererType {RendererType::Invalid};
    Ref<RendererContext> rendererContext;
    Scope<RendererAPI> rendererAPI;

    void Reset()
    {
      rendererContext.reset();
      rendererAPI.reset();
    }
  };
  static RendererData s_rendererData;
  
  void Renderer::Initialize(RendererType rendererType, void* windowPointer)
  {
    IK_ASSERT(rendererType != RendererType::Invalid, "Invalid Renderer Type");
    IK_LOG_INFO(LogModule::Renderer, "Initializing the Renderer with backedn API type '{0}'", RendererUtils::GetRendererTypeString(rendererType));
    
    s_rendererData.rendererType = rendererType;
    
    s_rendererData.rendererContext = RendererContextFactory::Create(static_cast<GLFWwindow*>(windowPointer));
    s_rendererData.rendererAPI = RendererAPIFactory::Create();
  }
  void Renderer::Shutdown()
  {
    IK_LOG_INFO(LogModule::Renderer, "Shutting down the Renderer");
    s_rendererData.Reset();
  }
  
  void Renderer::OnUpdate()
  {
    s_rendererData.rendererContext->SwapBuffers();
  }
  
  // Fundamentals -------------------------------------------------------------------------------
  RendererType Renderer::GetCurrentRendererAPI()
  {
    return s_rendererData.rendererType;
  }
  
  // Getters ------------------------------------------------------------------------------------
  RendererContext* Renderer::GetRendererContext()
  {
    return s_rendererData.rendererContext.get();
  }

  // Renderer Controllers ------------------------------------------------------------------------------------------------------------------------
  void Renderer::CheckError(std::string_view context)
  {
    s_rendererData.rendererAPI->CheckError(context);
  }
} // namespace KanViz
