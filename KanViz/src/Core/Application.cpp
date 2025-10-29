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
  {
    IK_PROFILE();
    IK_LOG_INFO(LogModule::Application, "Creating core application instance");
  }
  
  Application::~Application()
  {
    IK_PROFILE();
    IK_LOG_WARN(LogModule::Application, "Destroying core application instance");
  }
  
  void Application::Run()
  {
    
  }
  
  void Application::Close()
  {
    
  }
} // namespace KanViz
