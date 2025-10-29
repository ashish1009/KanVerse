//
//  MacWindiow.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include <GLFW/glfw3.h>

#include "Core/Window.hpp"

namespace KanViz
{
  /// This structure stores the macOS window data.
  /// - Note: This data is passed to GLFW callback.
  struct MacWindowData
  {
    WindowSpecification specification;      //< Window specifications.
    EventCallbackFn eventCallbackFunction; //< Callback for window events.
  };
  
  /// This is the implementation class for creating a MAC window.
  class MacWindow : public Window
  {
  public:
    /// Constructor that creates an instance of the MAC OS window with specification data.
    /// - Parameter specification: Window specification data.
    explicit MacWindow(const WindowSpecification& specification);
    
    /// Destructor that destroys the MAC OS window instance.
    ~MacWindow() override;
    
    // Fundamentals --------------------------------------------------------------------------------------------------------------------------------
    /// This function updates the window for each frame. This function should be called by the application inside the game loop.
    /// It swaps the context buffers and updates the time step of each frame.
    /// - Note: To be called in the main thread only.
    void Update() override;
    /// This function updates the event callback functions and registers them to the GLFW window instance.
    /// - Parameter eventCallbackFn: The function object to be registered for window events.
    /// - Note: eventCallbackFn is called every time an event is triggered for the window.
    void SetEventFunction(EventCallbackFn eventCallbackFn) noexcept override;
    
    // Controllers ---------------------------------------------------------------------------------------------------------------------------------
    /// This function maximizes the window to its full length based on the current monitor.
    void Maximize() const noexcept override;
    
    /// This function restores the window to its original size based on the current monitor and last size of the window.
    void Restore() const noexcept override;
    
    /// This function minimizes the window.
    void Iconify() const noexcept override;
    
    /// This function places the window at the center of the screen.
    void SetAtCenter() const noexcept override;
    
    /// This function sets the position of the window.
    /// - Parameter pos: position of the window.
    void SetPosition(const glm::vec2& pos) const noexcept override;
    
    /// This function makes the window resizable if 'resizable' is true; otherwise, the window cannot be resized after calling this API.
    /// - Parameter resizable: new flag for resizability.
    void SetResizable(bool resizable) const noexcept override;
    
    /// This function resizes the window.
    /// - Parameter size: new size of the window.
    void SetSize(const glm::vec2& size) noexcept override;
    
    /// This function changes the title of the window to 'newTitle'.
    /// - Parameter newTitle: new title of the window.
    void SetTitle(std::string_view newTitle) noexcept override;
    
    // Getters -------------------------------------------------------------------------------------------------------------------------------------
    /// This function checks if the window is maximized.
    [[nodiscard]] bool IsMaximized() const noexcept override;
    
    /// This function checks if the window is active.
    [[nodiscard]] bool IsActive() const noexcept override;
    
    /// This function checks if the window title bar is hidden.
    [[nodiscard]] bool IsTitleBarHidden() const noexcept override;
    
    /// This function checks if the window is in full screen mode.
    [[nodiscard]] bool IsFullscreen() const noexcept override;
    
    /// This function returns the width of the window.
    [[nodiscard]] uint32_t GetWidth() const noexcept override;
    
    /// This function returns the height of the window.
    [[nodiscard]] uint32_t GetHeight() const noexcept override;
    
    /// This function returns the current time step of this frame.
    [[nodiscard]] TimeStep GetTimestep() const noexcept override;
    
    /// This function returns the native window pointer (void*).
    /// - Returns: A pointer to the native GLFW window.
    void* GetNativeWindow() const noexcept override;
    
    /// This function returns the title of the window.
    [[nodiscard]] const std::string& GetTitle() const noexcept override;
    
    /// This function returns the window specification data.
    [[nodiscard]] const WindowSpecification& GetSpecification() const noexcept override;
    
  private:
    // Member Functions ----------------------------------------------------------------------------------------------------------------------------
    /// This function registers the event callbacks for the window.
    void SetEventCallbacks();
    
    // Member Variables ----------------------------------------------------------------------------------------------------------------------------
    float m_lastFrameTime {0.0f};       //< Last frame time for timestep calculation.
    MacWindowData m_data;               //< MAC window data.
    GLFWwindow* m_window;               //< Pointer to the GLFW window.
    TimeStep m_timeStep;                //< Current frame time step.
  };
} // namespace KanViz
