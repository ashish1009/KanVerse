//
//  RendererLayer.cpp
//  KanVest
//
//  Created by Ashish . on 29/10/25.
//

#include "RendererLayer.hpp"

namespace KanVest
{
  RendererLayer* RendererLayer::s_instance = nullptr;
  RendererLayer& RendererLayer::Get()
  {
    return *s_instance;
  }
  
  RendererLayer::RendererLayer()
  : KanViz::Layer("Kreator Renderer")
  {
    IK_PROFILE();
    IK_ASSERT(!s_instance, "RendererLayer instance already created");
    
    // Copy the single instance of application
    s_instance = this;
    
    IK_LOG_INFO("RendererLayer", "Creating '{0}' Layer instance", GetName());
  }
  
  RendererLayer::~RendererLayer()
  {
    IK_PROFILE();
    IK_LOG_WARN("RendererLayer", "Destroying '{0}' Layer instance", GetName());
  }
  
  void RendererLayer::OnAttach() noexcept
  {
    IK_PROFILE();
    IK_LOG_INFO("RendererLayer", "Attaching '{0}' Layer to application", GetName());
  }
  
  void RendererLayer::OnDetach() noexcept
  {
    IK_PROFILE();
    IK_LOG_WARN("RendererLayer", "Detaching '{0}' Layer from application", GetName());
  }
  
  void RendererLayer::OnUpdate(const KanViz::TimeStep& ts)
  {
    IK_PERFORMANCE_FUNC("RendererLayer::OnUpdate");
  }
  
  void RendererLayer::OnImGuiRender()
  {
  }
  
  void RendererLayer::OnEvent(KanViz::Event& event)
  {
  }
} // namespace KanVest
