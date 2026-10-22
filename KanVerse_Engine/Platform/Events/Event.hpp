/*!
 * @file Event.hpp
 * @brief Base event hierarchy and dispatch mechanisms for the KanVerse platform.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <functional>

namespace KanVerse {
  
  /*!
   * @enum EventType
   * @brief Concrete type identifiers for engine events.
   */
  enum class EventType : uint8_t {
    None = 0,
    WindowClose,
    WindowResize,
    WindowFocus,
    WindowLostFocus,
    KeyPressed,
    KeyReleased,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled,
    AppTick,
    AppUpdate,
    AppRender
  };
  
  /*!
   * @enum EventCategory
   * @brief Bitflags grouping events for batch filtering and query masks.
   */
  enum EventCategory : uint8_t {
    None                     = 0,
    EventCategoryApplication = 1 << 0,
    EventCategoryInput       = 1 << 1,
    EventCategoryKeyboard    = 1 << 2,
    EventCategoryMouse       = 1 << 3,
    EventCategoryMouseButton = 1 << 4
  };
  
#define EVENT_CLASS_TYPE(type)                                                    \
[[nodiscard]] static EventType GetStaticType() noexcept { return EventType::type; } \
[[nodiscard]] EventType GetEventType() const noexcept override { return GetStaticType(); } \
[[nodiscard]] const char* GetName() const noexcept override { return #type; }
  
#define EVENT_CLASS_CATEGORY(category)                                            \
[[nodiscard]] uint8_t GetCategoryFlags() const noexcept override { return category; }
  
  /*!
   * @class Event
   * @brief Abstract base class representing discrete user, platform, or hardware events.
   */
  class Event {
  public:
    virtual ~Event() = default;
    
    [[nodiscard]] virtual EventType GetEventType() const noexcept = 0;
    [[nodiscard]] virtual const char* GetName() const noexcept = 0;
    [[nodiscard]] virtual uint8_t GetCategoryFlags() const noexcept = 0;
    
    /*!
     * @brief Checks if this event belongs to a specified category bitmask.
     * @param category Bitmask to test against.
     * @return True if category bit is set.
     */
    [[nodiscard]] bool IsInCategory(EventCategory category) const noexcept {
      return (GetCategoryFlags() & static_cast<uint8_t>(category)) != 0;
    }
    
    /*!
     * @brief Queries whether this event has been consumed by a layer or handler.
     */
    [[nodiscard]] bool IsHandled() const noexcept { return m_Handled; }
    
    /*!
     * @brief Flags the event as handled to suppress further propagation.
     */
    void SetHandled(bool handled = true) noexcept { m_Handled = handled; }
    
  protected:
    bool m_Handled{false};
  };
  
  /*!
   * @class EventDispatcher
   * @brief Type-safe event dispatch helper routing concrete events to callbacks.
   */
  class EventDispatcher {
  public:
    /*!
     * @brief Constructs an event dispatcher bound to an active event instance.
     * @param event Reference to the event being processed.
     */
    explicit EventDispatcher(Event& event) noexcept
    : m_Event(event) {}
    
    /*!
     * @brief Dispatches the event to a matching member or lambda function.
     * @tparam T Event type derived from Event.
     * @tparam F Invocable callable matching bool(T&).
     * @param func Handler function returning true if handled.
     * @return True if dispatch succeeded and function was called.
     */
    template <typename T, typename F>
    bool Dispatch(const F& func) {
      if (m_Event.GetEventType() == T::GetStaticType()) {
        m_Event.SetHandled(m_Event.IsHandled() | func(static_cast<T&>(m_Event)));
        return true;
      }
      return false;
    }
    
  private:
    Event& m_Event;
  };
  
} // namespace KanVerse
