//
//  Application.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Core/ApplicationSpecification.h"
#include "Core/WindowFactory.hpp"
#include "Core/LayerStack.hpp"

#include "Core/Event/ApplicationEvents.h"

namespace KanViz
{
  /// This class is the base Application for the KanViz Engine.
  ///   - Initializes the window and renderers
  ///   - Manages all core and client layers
  ///   - Handles events from the window
  ///   - Runs the main loop
  /// - Note: This base application class can be extended; clients can derive from it and override methods as needed.
  /// - Important: This is a singleton class.
  class Application
  {
  public:
    /// This constructor initializes the application instance with the provided specification data.
    /// - Parameter specification: application specification data
    Application(const ApplicationSpecification& specification);
    /// This destructor destroys the application instance
    virtual ~Application();
    
    /// This function runs the Game loop of the application. Updates the application, layers, and window, and renders the GUI layer.
    void Run();
    /// This function closes the current application (KanViz::Application) and ends the game loop
    void Close();
    
    /// This function Pushes the layer of type IKan::Layer in Core Application layer stack.
    /// - Note: Attaches the layer as well.
    /// - Parameter layer: Reference to the layer to be add
    void PushLayer(const Ref<Layer>& layer);
    /// This function Pops the layer of type IKan::Layer from Core Application layer stack.
    /// - Note: Detaches the layer as well.
    /// - Parameter layer: Reference to the layer to be removed
    void PopLayer(const Ref<Layer>& layer);
    /// This function checks if layer is present in the stack
    /// - Parameter layer: Reference to layer to be checked
    bool Contain(const Ref<Layer>& layer);

    // Virtual APIs --------------------------------------------------------------------------------------------------------------------------------
    /// This function is called before game loop
    virtual void OnInit() {}
    /// This function is called after game loop
    virtual void OnShutdown() {}
    /// This function is called inside Game Loop Running
    /// - Parameter ts: Time step for each frame
    virtual void OnUpdate(const TimeStep&) {}
    /// This function is called when an event triggers
    /// - Parameter event: Triggered Events
    virtual void OnEvent(Event&) {}
    /// This function is called within the game loop specifically during ImGui layer rendering.
    virtual void OnImGuiRender() {}

  private:
    /// This function handles all window events. Dispatches the events and executes the appropriate actions.
    /// - Parameter event: event abstract type
    void HandleEvents(Event& event);
    /// This function is dispatched by the event dispatcher when the window closes.
    /// - Parameter windowCloseEvent: Window close event instance
    bool WindowClose(WindowCloseEvent& windowCloseEvent);

    /// This function begins the ImGui renderer and renders ImGui
    void RenderImGuiLayers();
    
    /// This function flushes pending tasks before the game loop starts.
    void FlushBeforeGameLoop();
    /// This function flushes pending tasks after the game loop ends.
    void FlushAfterGameLoop();

    bool m_isRunning {false};
    
    ApplicationSpecification m_specification;
    TimeStep m_timeStep;
    Scope<Window> m_window;
    LayerStack m_layers;

    // Static data members
    inline static Application* s_instance {nullptr}; //< Singleton instance of the application
  };
} // namespace KanViz
