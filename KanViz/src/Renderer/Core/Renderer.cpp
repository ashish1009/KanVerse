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
    Scope<RenderCommandQueue> commandQueue;

    void Reset()
    {
      rendererContext.reset();
      rendererAPI.reset();
      commandQueue.reset();
    }
  };
  static RendererData s_rendererData;
  
  void Renderer::Initialize(RendererType rendererType, void* windowPointer)
  {
    IK_ASSERT(rendererType != RendererType::Invalid, "Invalid Renderer Type");
    IK_LOG_INFO(LogModule::Renderer, "Initializing the Renderer with backedn API type '{0}'", RendererUtils::GetRendererTypeString(rendererType));
    
    s_rendererData.rendererType = rendererType;
    
    s_rendererData.commandQueue = CreateScope<RenderCommandQueue>(10);
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
  
  void Renderer::Clear(const glm::vec4& color)
  {
    s_rendererData.rendererAPI->SetClearColor(color);
    s_rendererData.rendererAPI->ClearBits();
  }
  void Renderer::ClearColor(const glm::vec4& color)
  {
    s_rendererData.rendererAPI->SetClearColor(color);
  }
  void Renderer::ClearBits()
  {
    s_rendererData.rendererAPI->ClearBits();
  }
  void Renderer::ClearColorBits()
  {
    s_rendererData.rendererAPI->ClearColorBits();
  }
  void Renderer::ClearDepthBits()
  {
    s_rendererData.rendererAPI->ClearDepthBits();
  }
  void Renderer::ClearStencilBits()
  {
    s_rendererData.rendererAPI->ClearStencilBits();
  }
  
  void Renderer::SetViewport(uint32_t width, uint32_t height)
  {
    s_rendererData.rendererAPI->SetViewport(width, height);
  }
  
  void Renderer::EnableStencilPass()
  {
    s_rendererData.rendererAPI->EnableStencilPass();
  }
  void Renderer::DisableStencilPass()
  {
    s_rendererData.rendererAPI->DisableStencilPass();
  }
  void Renderer::DepthFunc(GlDepthFunc func)
  {
    s_rendererData.rendererAPI->DepthFunc(func);
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
  
  // Render Command Queue --------------------------------------------------------------------------------------------------------------------------
  void Renderer::WaitAndRender()
  {
    IK_ASSERT(s_rendererData.commandQueue, "Render Command Queue is NULL");
    s_rendererData.commandQueue->Execute();
  }
  RenderCommandQueue* Renderer::GetRenderCommandQueue()
  {
    return s_rendererData.commandQueue.get();
  }

} // namespace KanViz
