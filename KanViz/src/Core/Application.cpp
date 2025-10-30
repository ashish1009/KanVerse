//
//  Application.cpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#include "Application.hpp"

#include "Renderer/Core/Renderer.hpp"

namespace KanViz
{
  Application& Application::Get()
  {
    IK_ASSERT(s_instance, "Application instance not created !!!");
    return *s_instance;
  }
  
  Application::Application(const ApplicationSpecification& specification)
  : m_specification(specification)
  {
    IK_PROFILE();
    IK_ASSERT(!s_instance, "Application instance already created");
    if (!s_instance)
    { 
      s_instance = this;
      IK_LOG_INFO(LogModule::Application, "Creating core application instance");
      
      // Check Renderer API type
      IK_ASSERT(m_specification.rendererType == m_specification.windowSpec.rendererType);

      // Create the window
      m_window = WindowFactory::Create(m_specification.windowSpec);
      
      if (m_window)
      {
        // Set window even callbacks
        m_window->SetEventFunction(IK_BIND_EVENT_FN(Application::HandleEvents));
        
        // Control Window
        m_window->SetResizable(m_specification.windowSpec.resizable);
        if (m_specification.windowSpec.maximized)
        {
          m_window->Maximize();
        }

        // Set the renderer type before any renderer creation.
        Renderer::Initialize(m_specification.rendererType, m_window->GetNativeWindow());

        // Initialize the ImGui Layer
        m_imguiLayer = CreateRef<UI::ImGuiLayer>(m_window.get());
        if (m_imguiLayer)
        {
          m_layers.PushOverlay(m_imguiLayer);
          m_imguiLayer->SetIniFilePath(m_specification.iniFilePath);
        }

        // Execute all commands from queue
        Renderer::WaitAndRender();

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
    
    if (m_window)
    {
      m_window.reset();
      m_layers.PopOverlay(m_imguiLayer);
      
      Renderer::Shutdown();
    }

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
      PerformanceProfile::Get().BeginFrame();
      {
        IK_PERFORMANCE_FUNC("Application::Run");
        
        // Client virtual override update function.
        {
          IK_PERFORMANCE_FUNC("Application::ClientUpdate");
          OnUpdate(m_timeStep);
        }
        
        // Updating all the attached layer
        {
          IK_PERFORMANCE_FUNC("Application::LayersUpdate");
          for (const Ref<Layer>& layer : m_layers)
          {
            layer->OnUpdate(m_timeStep);
          }
        }
        
        {
          IK_PERFORMANCE_FUNC("Application::WindowUpdate");
          
          // Update the window swap buffers
          m_window->Update();
          
          // Store the frame time difference
          m_timeStep = m_window->GetTimestep();
        }
        
        {
          IK_PERFORMANCE_FUNC("Application::Renderer_Update");
          Renderer::OnUpdate();
        }
        
        // Render ImGui at last
        RenderImGuiLayers();
        
        {
          IK_PERFORMANCE_FUNC("Application::WaitAndRender");
          
          // Execute all commands from queue before game loop
          Renderer::WaitAndRender();
        }
      }
      PerformanceProfile::Get().EndFrame();
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
    IK_ASSERT(m_imguiLayer, "ImGui Layer is not created yet !!");
    
    m_imguiLayer->Begin();
    {
      // Render Imgui for all layers
      for (const Ref<Layer>& layer : m_layers)
      {
        layer->OnImGuiRender();
      }
      
      OnImGuiRender();
    }
    m_imguiLayer->End();
  }
  
  void Application::Close()
  {
    m_isRunning = false;
  }
  
  void Application::PushLayer(const Ref<Layer>& layer)
  {
    if (Contain(layer))
    {
      IK_LOG_ERROR(LogModule::Application, "Layer already added to stack.");
      return;
    }
    m_layers.PushLayer(layer);
  }
  
  void Application::PopLayer(const Ref<Layer>& layer)
  {
    if (!Contain(layer))
    {
      IK_LOG_ERROR(LogModule::Application, "Attempted to pop a layer that doesn't exist in the stack.");
      return;
    }
    m_layers.PopLayer(layer);
  }
  
  bool Application::Contain(const Ref<Layer> &layer)
  {
    return m_layers.Contain(layer);
  }

  const ApplicationSpecification& Application::GetSpecification() const
  {
    return m_specification;
  }
  void* Application::GetNativeWindow() const
  {
    return m_window->GetNativeWindow();
  }
  Window* Application::GetWindow() const
  {
    return m_window.get();
  }
  UI::ImGuiLayer& Application::GetImGuiLayer() const
  {
    if (!m_imguiLayer)
    {
      throw std::runtime_error("ImGui Layer is not created yet !!");
    }
    return *m_imguiLayer.get();
  }

  void Application::HandleEvents(Event &event)
  {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowCloseEvent>(IK_BIND_EVENT_FN(Application::WindowClose));

    // Event Handler for all layers
    for (const Ref<Layer>& layer : m_layers)
    {
      layer->OnEvent(event);
    }

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

    // Execute all commands from queue before game loop
    Renderer::WaitAndRender();
  }
  
  void Application::FlushAfterGameLoop()
  {
    IK_PROFILE();
    
    // Shutdown the client side application
    OnShutdown();

    // Execute all commands from queue after game loop
    Renderer::WaitAndRender();
  }
} // namespace KanViz
