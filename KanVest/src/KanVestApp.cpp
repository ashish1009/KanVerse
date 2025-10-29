//
//  KanVestApp.cpp
//  KanVest
//
//  Created by Ashish . on 29/10/25.
//

#include "KanVestApp.hpp"

namespace KanVest
{
  Application::Application(const KanViz::ApplicationSpecification& appSpec)
  : KanViz::Application(appSpec)
  {
    IK_PROFILE();
    IK_LOG_INFO("Kreator Application", "Creating 'Studio' application instance with title: {}", appSpec.appName);
  }
  
  Application::~Application()
  {
    IK_PROFILE();
    IK_LOG_WARN("Kreator Application", "Destroying 'Studio' application instance");
  }
  
  void Application::OnInit()
  {
    IK_PROFILE();
  }
  
  void Application::OnShutdown()
  {
    IK_PROFILE();
  }
  
  void Application::OnUpdate([[maybe_unused]] const KanViz::TimeStep& ts)
  {
    // Intentionally left empty for now
  }
  
  void Application::OnImGuiRender()
  {
    // Intentionally left empty for now
  }
  
  void Application::OnEvent(KanViz::Event&)
  {
    // Intentionally left empty for now
  }
} // namespace KanVest
