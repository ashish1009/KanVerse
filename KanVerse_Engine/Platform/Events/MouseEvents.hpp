/*!
 * @file MouseEvents.hpp
 * @brief Strongly typed mouse button, motion, and scroll event declarations.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Platform/Events/Event.hpp"
#include <sstream>
#include <string>

namespace KanVerse {
  
  /*!
   * @class MouseMovedEvent
   * @brief Dispatched when the mouse cursor updates coordinates inside the active window.
   */
  class MouseMovedEvent : public Event {
  public:
    /*!
     * @brief Constructs a mouse movement event.
     * @param x Horizontal window coordinate in logical points.
     * @param y Vertical window coordinate in logical points.
     */
    MouseMovedEvent(float x, float y)
    : m_MouseX(x), m_MouseY(y) {}
    
    /*!
     * @brief Retrieves the horizontal coordinate of the mouse.
     * @return Logical point horizontal position.
     */
    [[nodiscard]] float GetX() const noexcept { return m_MouseX; }
    
    /*!
     * @brief Retrieves the vertical coordinate of the mouse.
     * @return Logical point vertical position.
     */
    [[nodiscard]] float GetY() const noexcept { return m_MouseY; }
    
    /*!
     * @brief Serializes event details for diagnostic logging.
     * @return Formatted string representation.
     */
    [[nodiscard]] std::string ToString() const {
      std::stringstream ss;
      ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
      return ss.str();
    }
    
    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    
  private:
    float m_MouseX; ///< Horizontal cursor coordinate.
    float m_MouseY; ///< Vertical cursor coordinate.
  };
  
  /*!
   * @class MouseScrolledEvent
   * @brief Dispatched upon mouse scroll wheel or trackpad scroll gesture deltas.
   */
  class MouseScrolledEvent : public Event {
  public:
    /*!
     * @brief Constructs a mouse scroll event.
     * @param xOffset Horizontal delta offset.
     * @param yOffset Vertical delta offset.
     */
    MouseScrolledEvent(float xOffset, float yOffset)
    : m_XOffset(xOffset), m_YOffset(yOffset) {}
    
    /*!
     * @brief Retrieves the horizontal scroll offset.
     * @return Float offset delta.
     */
    [[nodiscard]] float GetXOffset() const noexcept { return m_XOffset; }
    
    /*!
     * @brief Retrieves the vertical scroll offset.
     * @return Float offset delta.
     */
    [[nodiscard]] float GetYOffset() const noexcept { return m_YOffset; }
    
    /*!
     * @brief Serializes event details for diagnostic logging.
     * @return Formatted string representation.
     */
    [[nodiscard]] std::string ToString() const {
      std::stringstream ss;
      ss << "MouseScrolledEvent: " << m_XOffset << ", " << m_YOffset;
      return ss.str();
    }
    
    EVENT_CLASS_TYPE(MouseScrolled)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    
  private:
    float m_XOffset; ///< Horizontal scroll delta.
    float m_YOffset; ///< Vertical scroll delta.
  };
  
  /*!
   * @class MouseButtonEvent
   * @brief Abstract base class for discrete mouse button press and release actions.
   */
  class MouseButtonEvent : public Event {
  public:
    /*!
     * @brief Retrieves the target mouse button code (0 = Left, 1 = Right, 2 = Middle).
     * @return Integer button identifier.
     */
    [[nodiscard]] int GetMouseButton() const noexcept { return m_Button; }
    
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
    
  protected:
    /*!
     * @brief Protected constructor initialized by derived concrete events.
     * @param button Target mouse button code.
     */
    explicit MouseButtonEvent(int button)
    : m_Button(button) {}
    
    int m_Button; ///< Tracked mouse button index.
  };
  
  /*!
   * @class MouseButtonPressedEvent
   * @brief Dispatched upon physical depressing of a mouse button.
   */
  class MouseButtonPressedEvent : public MouseButtonEvent {
  public:
    /*!
     * @brief Constructs a mouse button down event.
     * @param button Pressed button code.
     */
    explicit MouseButtonPressedEvent(int button)
    : MouseButtonEvent(button) {}
    
    /*!
     * @brief Serializes event details for diagnostic logging.
     * @return Formatted string representation.
     */
    [[nodiscard]] std::string ToString() const {
      std::stringstream ss;
      ss << "MouseButtonPressedEvent: " << m_Button;
      return ss.str();
    }
    
    EVENT_CLASS_TYPE(MouseButtonPressed)
  };
  
  /*!
   * @class MouseButtonReleasedEvent
   * @brief Dispatched upon releasing of a mouse button.
   */
  class MouseButtonReleasedEvent : public MouseButtonEvent {
  public:
    /*!
     * @brief Constructs a mouse button up event.
     * @param button Released button code.
     */
    explicit MouseButtonReleasedEvent(int button)
    : MouseButtonEvent(button) {}
    
    /*!
     * @brief Serializes event details for diagnostic logging.
     * @return Formatted string representation.
     */
    [[nodiscard]] std::string ToString() const {
      std::stringstream ss;
      ss << "MouseButtonReleasedEvent: " << m_Button;
      return ss.str();
    }
    
    EVENT_CLASS_TYPE(MouseButtonReleased)
  };
  
} // namespace KanVerse
