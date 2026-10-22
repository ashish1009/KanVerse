/*!
 * @file KeyEvents.hpp
 * @brief Strongly typed keyboard press and release event declarations.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Platform/Events/Event.hpp"
#include <sstream>
#include <string>

namespace KanVerse {
  
  /*!
   * @class KeyEvent
   * @brief Abstract base class for keyboard-related events.
   */
  class KeyEvent : public Event {
  public:
    /*!
     * @brief Retrieves the target virtual key code.
     * @return Integer key code.
     */
    [[nodiscard]] int GetKeyCode() const noexcept { return m_KeyCode; }
    
    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    
  protected:
    /*!
     * @brief Protected constructor initialized by derived concrete events.
     * @param keyCode Target virtual key code.
     */
    explicit KeyEvent(int keyCode)
    : m_KeyCode(keyCode) {}
    
    int m_KeyCode; ///< Tracked virtual key code.
  };
  
  /*!
   * @class KeyPressedEvent
   * @brief Dispatched when a physical keyboard key is depressed.
   */
  class KeyPressedEvent : public KeyEvent {
  public:
    /*!
     * @brief Constructs a key press event.
     * @param keyCode Pressed key code.
     * @param repeatCount Number of key repeat cycles.
     */
    KeyPressedEvent(int keyCode, int repeatCount = 0)
    : KeyEvent(keyCode), m_RepeatCount(repeatCount) {}
    
    /*!
     * @brief Retrieves key repeat count.
     * @return Repeat count integer.
     */
    [[nodiscard]] int GetRepeatCount() const noexcept { return m_RepeatCount; }
    
    /*!
     * @brief Serializes event details for diagnostic logging.
     * @return Formatted string representation.
     */
    [[nodiscard]] std::string ToString() const {
      std::stringstream ss;
      ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
      return ss.str();
    }
    
    EVENT_CLASS_TYPE(KeyPressed)
    
  private:
    int m_RepeatCount; ///< Repeat count metric.
  };
  
  /*!
   * @class KeyReleasedEvent
   * @brief Dispatched when a physical keyboard key is released.
   */
  class KeyReleasedEvent : public KeyEvent {
  public:
    /*!
     * @brief Constructs a key release event.
     * @param keyCode Released key code.
     */
    explicit KeyReleasedEvent(int keyCode)
    : KeyEvent(keyCode) {}
    
    /*!
     * @brief Serializes event details for diagnostic logging.
     * @return Formatted string representation.
     */
    [[nodiscard]] std::string ToString() const {
      std::stringstream ss;
      ss << "KeyReleasedEvent: " << m_KeyCode;
      return ss.str();
    }
    
    EVENT_CLASS_TYPE(KeyReleased)
  };
  
} // namespace KanVerse
