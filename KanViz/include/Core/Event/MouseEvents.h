//
//  MouseEvents.h
//  IKan
//
//  Created by Ashish . on 22/08/25.
//

#pragma once

#include "Core/Event/Events.h"
#include "Core/MouseButtonCodes.h"

namespace IKan
{
  // -------------------------------------------------------------------------------------------------------------------
  /// Event triggered when the mouse is moved
  // -------------------------------------------------------------------------------------------------------------------
  class MouseMovedEvent : public Event
  {
  public:
    /// Constructs a mouse moved event
    /// - Parameters:
    ///   - xOffset: x position of mouse
    ///   - yOffset: y position of mouse
    MouseMovedEvent(float xOffset, float yOffset) noexcept
    : m_mouseX(xOffset), m_mouseY(yOffset) {}
    
    /// Returns the current X position of the mouse
    [[nodiscard]] float GetX() const noexcept { return m_mouseX; }
    
    /// Returns the current Y position of the mouse
    [[nodiscard]] float GetY() const noexcept { return m_mouseY; }
    
    EVENT_CLASS_TYPE(MouseMoved);
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);
    
  private:
    float m_mouseX {0.0f}, m_mouseY {0.0f};
  };
  
  // -------------------------------------------------------------------------------------------------------------------
  /// Event triggered when the mouse wheel is scrolled
  // -------------------------------------------------------------------------------------------------------------------
  class MouseScrolledEvent : public Event
  {
  public:
    /// Constructs a mouse scrolled event
    /// - Parameters:
    ///   - xOffset: horizontal scroll offset
    ///   - yOffset: vertical scroll offset
    MouseScrolledEvent(float xOffset, float yOffset) noexcept
    : m_xOffset(xOffset), m_yOffset(yOffset) {}
    
    /// Returns horizontal scroll offset
    [[nodiscard]] float GetXOffset() const noexcept { return m_xOffset; }
    
    /// Returns vertical scroll offset
    [[nodiscard]] float GetYOffset() const noexcept { return m_yOffset; }
    
    EVENT_CLASS_TYPE(MouseScrolled);
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);
    
  private:
    float m_xOffset {0.0f}, m_yOffset {0.0f};
  };
  
  // -------------------------------------------------------------------------------------------------------------------
  /// Base class for mouse button related events
  // -------------------------------------------------------------------------------------------------------------------
  class MouseButtonEvent : public Event
  {
  public:
    /// Returns the mouse button that triggered the event
    [[nodiscard]] MouseButton GetMouseButton() const noexcept { return m_button; }
    
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton);
    
  protected:
    /// Constructs a mouse button event
    /// - Parameter button: mouse button code
    explicit MouseButtonEvent(MouseButton button) noexcept
    : m_button(button) {}
    
    MouseButton m_button {MouseButton::Button0};
  };
  
  // -------------------------------------------------------------------------------------------------------------------
  /// Event triggered when a mouse button is pressed
  // -------------------------------------------------------------------------------------------------------------------
  class MouseButtonPressedEvent : public MouseButtonEvent
  {
  public:
    /// Constructs a mouse button pressed event
    /// - Parameter button: Mouse button pressed
    explicit MouseButtonPressedEvent(MouseButton button) noexcept
    : MouseButtonEvent(button) {}
    
    EVENT_CLASS_TYPE(MouseButtonPressed);
  };
  
  // -------------------------------------------------------------------------------------------------------------------
  /// Event triggered when a mouse button is released
  // -------------------------------------------------------------------------------------------------------------------
  class MouseButtonReleasedEvent : public MouseButtonEvent
  {
  public:
    /// Constructs a mouse button released event
    /// - Parameter button: Mouse button released
    explicit MouseButtonReleasedEvent(MouseButton button) noexcept
    : MouseButtonEvent(button) {}
    
    EVENT_CLASS_TYPE(MouseButtonReleased);
  };
} // namespace IKan
