//
//  Application.cpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#include "Application.hpp"

namespace KanViz
{
  Application::Application(const ApplicationSpecification& specification)
  : m_specification(specification)
  {
    IK_PROFILE();
    IK_ASSERT(!s_instance, "Application instance already created");
    if (!s_instance)
    { 
      s_instance = this;
      IK_LOG_INFO(LogModule::Application, "Creating core application instance");
    }
  }
  
  Application::~Application()
  {
    IK_PROFILE();
    IK_LOG_WARN(LogModule::Application, "Destroying core application instance");
    s_instance = nullptr;
  }
  
  void Application::Run()
  {
    
  }
  
  void Application::Close()
  {
    
  }
} // namespace KanViz
