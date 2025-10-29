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
      
      // Create the window
      m_window = WindowFactory::Create(m_specification.windowSpec);
      
      if (m_window)
      {
        // Set window even callbacks
        m_window->SetEventFunction(IK_BIND_EVENT_FN(Application::HandleEvents));
        
        m_isRunning = true;
      } // if window
      else
      {
        IK_ASSERT(false, "Failed to create window");
      }
    } // if not instance
  }
  
  Application::~Application()
  {
    IK_PROFILE();
    IK_LOG_WARN(LogModule::Application, "Destroying core application instance");
    s_instance = nullptr;
  }
  
  void Application::Run()
  {
    IK_ASSERT(m_window, "Window not created");
    if (!m_window)
    {
      std::cout << "ERROR : " << "Window not created. Exit Run() \n";
      return;
    }
    
    // Flush the pending task before game loop starts
    FlushBeforeGameLoop();
    
    // Separator in profiler logs before Game Loop
    {
      IK_NAMED_PROFILE("---------------------------------------------------------------------");
    }
    
    // Game Loop
    IK_LOG_INFO("", "--------------------------------------------------------------------------");
    IK_LOG_INFO("", "                          Starting Game Loop                              ");
    IK_LOG_INFO("", "--------------------------------------------------------------------------");
    
    while (m_isRunning)
    {
      IK_PERFORMANCE_FUNC("Application::Run");
      
      // Client virtual override update function.
      {
        IK_PERFORMANCE_FUNC("Application::ClientUpdate");
        OnUpdate(m_timeStep);
      }
      
      {
        IK_PERFORMANCE_FUNC("Application::WindowUpdate");
        
        // Update the window swap buffers
        m_window->Update();
        
        // Store the frame time difference
        m_timeStep = m_window->GetTimestep();
      }
      
      // Render ImGui at last
      RenderImGuiLayers();
    }

    IK_LOG_WARN("", "--------------------------------------------------------------------------");
    IK_LOG_WARN("", "                           Ending Game Loop                               ");
    IK_LOG_WARN("", "--------------------------------------------------------------------------");
    
    // Separator in profiler logs after Game Loop
    {
      IK_NAMED_PROFILE("---------------------------------------------------------------------");
    }
    
    // Flush the pending task after game loop starts
    FlushAfterGameLoop();
  }
  
  void Application::RenderImGuiLayers()
  {
    
  }
  
  void Application::Close()
  {
    m_isRunning = false;
  }
  
  void Application::HandleEvents(Event &event)
  {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowCloseEvent>(IK_BIND_EVENT_FN(Application::WindowClose));

    // Client events
    OnEvent(event);
  }
  
  bool Application::WindowClose([[maybe_unused]] WindowCloseEvent& windowCloseEvent)
  {
    IK_LOG_INFO(LogModule::Application, "Closing the Window from application");
    Close();
    return false;
  }
  
  void Application::FlushBeforeGameLoop()
  {
    IK_PROFILE();
    
    // Initialize the client side application
    OnInit();
  }
  
  void Application::FlushAfterGameLoop()
  {
    IK_PROFILE();
    
    // Shutdown the client side application
    OnShutdown();
  }
} // namespace KanViz
