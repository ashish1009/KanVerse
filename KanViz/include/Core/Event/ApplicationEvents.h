//
//  ApplicationEvents.h
//  KanViz
//
//  Created by Ashish . on 22/08/25.
//

#pragma once

#include "Core/Event/Events.h"

namespace KanViz
{
  // -------------------------------------------------------------------------------------------------------------------
  /// Event triggered when the window is resized
  // -------------------------------------------------------------------------------------------------------------------
  class WindowResizeEvent : public Event
  {
  public:
    /// Virtual destructor for polymorphic base compatibility
    ~WindowResizeEvent() override = default;
    
    /// Constructs a resize event with the new width and height
    /// - Parameters:
    ///   - width : new window width
    ///   - height: new window height
    WindowResizeEvent(uint32_t width, uint32_t height) noexcept
    : m_prevWidth(m_width), m_prevHeight(m_height),
    m_width(width), m_height(height) {}
    
    /// Returns the previous window width
    [[nodiscard]] uint32_t GetPrevWidth() const noexcept { return m_prevWidth; }
    
    /// Returns the previous window height
    [[nodiscard]] uint32_t GetPrevHeight() const noexcept { return m_prevHeight; }
    
    /// Returns the new window width
    [[nodiscard]] uint32_t GetWidth() const noexcept { return m_width; }
    
    /// Returns the new window height
    [[nodiscard]] uint32_t GetHeight() const noexcept { return m_height; }
    
    EVENT_CLASS_TYPE(WindowResize);
    EVENT_CLASS_CATEGORY(EventCategoryApplication);
    
  private:
    uint32_t m_width {0}, m_height {0};
    uint32_t m_prevWidth {0}, m_prevHeight {0};
  };
  
  // -------------------------------------------------------------------------------------------------------------------
  /// Event triggered when the window is requested to close
  // -------------------------------------------------------------------------------------------------------------------
  class WindowCloseEvent : public Event
  {
  public:
    ~WindowCloseEvent() override = default;
    
    EVENT_CLASS_TYPE(WindowClose);
    EVENT_CLASS_CATEGORY(EventCategoryApplication);
  };
  
  // -------------------------------------------------------------------------------------------------------------------
  /// Event triggered when the window is minimized
  // -------------------------------------------------------------------------------------------------------------------
  class WindowMinimizeEvent : public Event
  {
  public:
    explicit WindowMinimizeEvent(bool minimized) noexcept
    : m_minimized(minimized) {}
    
    /// Returns whether the window is minimized
    [[nodiscard]] bool IsMinimized() const noexcept { return m_minimized; }
    
    EVENT_CLASS_TYPE(WindowMinimize);
    EVENT_CLASS_CATEGORY(EventCategoryApplication);
    
  private:
    bool m_minimized {false};
  };
  
  // -------------------------------------------------------------------------------------------------------------------
  /// Event triggered when the window is maximized
  // -------------------------------------------------------------------------------------------------------------------
  class WindowMaximizeEvent : public Event
  {
  public:
    explicit WindowMaximizeEvent(bool maximized) noexcept
    : m_maximized(maximized) {}
    
    /// Returns whether the window is maximized
    [[nodiscard]] bool IsMaximized() const noexcept { return m_maximized; }
    
    EVENT_CLASS_TYPE(WindowMaximize);
    EVENT_CLASS_CATEGORY(EventCategoryApplication);
    
  private:
    bool m_maximized {false};
  };
} // namespace KanViz
