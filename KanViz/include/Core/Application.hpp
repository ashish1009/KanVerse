//
//  Application.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Core/ApplicationSpecification.h"

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
    
  private:
    bool m_isRunning {false};
    
    ApplicationSpecification m_specification;
    
    // Static data members
    inline static Application* s_instance {nullptr}; //< Singleton instance of the application
  };
} // namespace KanViz
