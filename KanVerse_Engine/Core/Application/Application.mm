/*!
 * @file Application.mm
 * @brief Implementation of the Application lifecycle runner with Renderer initialization.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Core/Diagnostics/Log.hpp"
#include "Core/Diagnostics/Assert.hpp"
#include "Core/Application/Application.hpp"

#include "Platform/Window/NativeWindow.hpp"

#include "Renderer/Renderer/Renderer.hpp"

#import <AppKit/AppKit.h>
#import <cassert>

namespace KanVerse {
  
  Application* Application::s_Instance = nullptr;
  
  Application::Application(const ApplicationProps& props) {
    KV_CORE_ASSERT(!s_Instance, "Application singleton already exists!");
    s_Instance = this;
    
    Log::Init();
    KV_CORE_INFO("KanVerse Engine initializing: '{}'", props.Name);
    
    m_RenderContext = RenderContext::Create();
    
    WindowProps winProps{
      .Title = props.Name,
      .Width = props.WindowWidth,
      .Height = props.WindowHeight,
      .VSync = props.VSync,
      .Maximized = props.Maximized
    };
    
    m_Window = Window::Create(winProps);
    m_Window->SetEventCallback([this](Event& e) {
      OnEvent(e);
    });
    
    m_Clock.Reset();
  }
  
  Application::~Application() {
    s_Instance = nullptr;
    KV_CORE_INFO("=== Application Destructor Finished ===");
  }
  
  void Application::Close() {
    m_Running = false;
  }
  
  void Application::PushLayer(std::unique_ptr<Layer> layer) {
    m_LayerStack.PushLayer(std::move(layer));
  }
  
  void Application::PushOverlay(std::unique_ptr<Layer> overlay) {
    m_LayerStack.PushOverlay(std::move(overlay));
  }
  
  void Application::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& event) {
      return OnWindowClose(event);
    });
    dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) {
      return OnWindowResize(event);
    });
    
    m_LayerStack.OnEvent(e);
  }
  
  bool Application::OnWindowClose(WindowCloseEvent& e) {
    Close();
    return true;
  }
  
  bool Application::OnWindowResize(WindowResizeEvent& e) {
    if (e.GetWidth() == 0 || e.GetHeight() == 0) {
      m_Minimized = true;
      return false;
    }
    
    m_Minimized = false;
    return false;
  }
  
  void Application::Run() {
    while (m_Running) {
      m_Window->OnUpdate();
      
      if (!m_Running) {
        KV_CORE_INFO("Run loop break triggered: exiting while(m_Running)...");
        break;
      }
      
      if (!m_Minimized) {
        TimeStep dt = m_Clock.Tick();
        
        for (auto& layer : m_LayerStack) {
          layer->OnUpdate(dt);
        }
        
        for (auto& layer : m_LayerStack) {
          layer->OnRender();
        }
      }
    }
    
    KV_CORE_INFO("Application::Run() completed normally.");
  }
} // namespace KanVerse

