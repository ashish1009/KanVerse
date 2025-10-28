//
//  KeyboardEvents.h
//  IKan
//
//  Created by Ashish . on 22/08/25.
//

#pragma once

#include "Core/Event/Events.h"
#include "Core/KeyCodes.h"

namespace IKan
{
  // -------------------------------------------------------------------------------------------------------------------
  /// Base class for all keyboard-related events
  // -------------------------------------------------------------------------------------------------------------------
  class KeyEvent : public Event
  {
  public:
    /// Returns the key code that triggered this event
    [[nodiscard]] Key GetKeyCode() const noexcept { return m_keyCode; }
    
    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput);
    
  protected:
    /// Constructs a keyboard event with the given key code
    explicit KeyEvent(Key keyCode) noexcept
    : m_keyCode(keyCode) {}
    
    Key m_keyCode {Key::Unknown};
  };
  
  // -------------------------------------------------------------------------------------------------------------------
  /// Event triggered when a key is pressed
  // -------------------------------------------------------------------------------------------------------------------
  class KeyPressedEvent : public KeyEvent
  {
  public:
    /// Constructs a key pressed event
    /// - Parameters:
    ///   - keyCode      : Key code of the pressed key
    ///   - repeatedCount: Number of times the key was repeated
    KeyPressedEvent(Key keyCode, int32_t repeatedCount) noexcept
    : KeyEvent(keyCode), m_repeatedCount(repeatedCount) {}
    
    /// Returns the number of times the key was repeated
    [[nodiscard]] int32_t GetRepeatCount() const noexcept { return m_repeatedCount; }
    
    EVENT_CLASS_TYPE(KeyPressed);
    
  private:
    int32_t m_repeatedCount {0};
  };
  
  // -------------------------------------------------------------------------------------------------------------------
  /// Event triggered when a key is released
  // -------------------------------------------------------------------------------------------------------------------
  class KeyReleasedEvent : public KeyEvent
  {
  public:
    /// Constructs a key released event
    /// - Parameter keyCode: Key code of the released key
    explicit KeyReleasedEvent(Key keyCode) noexcept
    : KeyEvent(keyCode) {}
    
    EVENT_CLASS_TYPE(KeyReleased);
  };
  
  // -------------------------------------------------------------------------------------------------------------------
  /// Event triggered when a key is typed (character input)
  // -------------------------------------------------------------------------------------------------------------------
  class KeyTypedEvent : public KeyEvent
  {
  public:
    /// Constructs a key typed event
    /// - Parameter keyCode: Key code of the typed key
    explicit KeyTypedEvent(Key keyCode) noexcept
    : KeyEvent(keyCode) {}
    
    EVENT_CLASS_TYPE(KeyTyped);
  };
} // namespace IKan
