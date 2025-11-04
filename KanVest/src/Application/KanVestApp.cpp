//
//  KanVestApp.cpp
//  KanVest
//
//  Created by Ashish . on 29/10/25.
//

#include "KanVestApp.hpp"

#include "RendererLayer.hpp"

#include "Portfolio/UserDatabase.hpp"

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
    
    std::filesystem::path userDataPath = "../../../KanVest/UserData";
    if (!exists(userDataPath))
    {
      std::filesystem::create_directory(userDataPath);
    }
    UserDatabase::SetDatabaseFilePath(userDataPath / "UserDatabase.yaml");

    // Creating a renderer layer and pushing it onto the application stack.
    m_layer = KanViz::CreateScope<RendererLayer>();
    PushLayer(std::move(m_layer));
  }
  
  void Application::OnShutdown()
  {
    IK_PROFILE();
    
    if (m_layer)
    {
      // Removing the renderer layer from the application stack and destroying its instance
      PopLayer(m_layer);
      m_layer.reset();
    }
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
