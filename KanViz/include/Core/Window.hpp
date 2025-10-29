//
//  Window.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Core/WindowSpecification.h"
#include "Core/TimeStep.hpp"

#include "Core/Event/Events.h"

namespace KanViz
{
  /// This is the abstract class for creating a GLFW window. Provides APIs to use the window.
  class Window
  {
  public:
    /// Default virtual destructor
    virtual ~Window() = default;
    
    // Fundamentals --------------------------------------------------------------------------------------------------------------------------------
    /// This function updates the window for each frame. This function should be called by the application inside the game loop.
    /// It swaps the context buffers and updates the time step of each frame.
    /// - Note: To be called in the main thread only.
    virtual void Update() = 0;
    /// This function updates the event callback functions and registers them to the window instance.
    /// - Parameter eventCallbackFn: function object to be registered in the window.
    /// - Note: The eventCallbackFn is called every time a user-defined event is triggered for the window.
    virtual void SetEventFunction(EventCallbackFn eventCallbackFn) noexcept = 0;
    
    // Controllers ---------------------------------------------------------------------------------------------------------------------------------
    /// This function maximizes the window to its full length based on the current monitor.
    virtual void Maximize() const noexcept = 0;
    
    /// This function restores the window to its original size based on the current monitor and last size of the window.
    virtual void Restore() const noexcept = 0;
    
    /// This function minimizes the window.
    virtual void Iconify() const noexcept = 0;
    
    /// This function places the window at the center of the screen.
    virtual void SetAtCenter() const noexcept = 0;
    
    /// This function sets the position of the window.
    /// - Parameter pos: position of the window.
    virtual void SetPosition(const glm::vec2& pos) const noexcept = 0;
    
    /// This function makes the window resizable if 'resizable' is true; otherwise, the window cannot be resized after calling this API.
    /// - Parameter resizable: new flag for resizability.
    virtual void SetResizable(bool resizable) const noexcept = 0;
    
    /// This function resizes the window.
    /// - Parameter size: new size of the window.
    virtual void SetSize(const glm::vec2& size) noexcept = 0;
    
    /// This function changes the title of the window to 'newTitle'.
    /// - Parameter newTitle: The new title of the window.
    virtual void SetTitle(std::string_view newTitle) noexcept = 0;
    
    // Getters -------------------------------------------------------------------------------------------------------------------------------------
    /// This function checks if the window is maximized.
    [[nodiscard]] virtual bool IsMaximized() const noexcept = 0;
    
    /// This function returns whether the window is active.
    [[nodiscard]] virtual bool IsActive() const noexcept = 0;
    
    /// This function returns whether the window title bar is hidden.
    [[nodiscard]] virtual bool IsTitleBarHidden() const noexcept = 0;
    
    /// This function returns whether the window is in full screen mode.
    [[nodiscard]] virtual bool IsFullscreen() const noexcept = 0;
    
    /// This function returns the width of the window.
    [[nodiscard]] virtual uint32_t GetWidth() const noexcept = 0;
    
    /// This function returns the height of the window.
    [[nodiscard]] virtual uint32_t GetHeight() const noexcept = 0;
    
    /// This function returns the current time step of this frame.
    [[nodiscard]] virtual TimeStep GetTimestep() const noexcept = 0;
    
    /// This function returns the native window pointer (void*).
    virtual void* GetNativeWindow() const noexcept = 0;
    
    /// This function returns the title of the window.
    [[nodiscard]] virtual const std::string& GetTitle() const noexcept = 0;
    
    /// This function returns the window specification data.
    [[nodiscard]] virtual const WindowSpecification& GetSpecification() const noexcept = 0;  };
} // namespace KanViz
