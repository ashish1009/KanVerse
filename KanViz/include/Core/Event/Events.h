//
//  Events.h
//  IKan
//
//  Created by Ashish . on 22/08/25.
//

#pragma once

namespace IKan
{
  // Macros for defining Event classes
#define EVENT_CLASS_TYPE(type) \
static constexpr EventType GetStaticType() noexcept { return EventType::type; } \
EventType GetType() const noexcept override { return GetStaticType(); } \
const char* GetName() const noexcept override { return #type; }
  
#define EVENT_CLASS_CATEGORY(category) \
int32_t GetCategoryFlags() const noexcept override { return category; }
  
  // API macro to bind class member function to std::function
#define IK_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
  
  /// This enum defines all supported event types in IKan engine
  enum class EventType : uint8_t
  {
    NoEventType = 0,
    WindowClose, WindowResize, WindowMinimize, WindowMaximize,
    KeyPressed, KeyReleased, KeyTyped,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
  };
  
  /// This enum defines the category bit-flags for events
  enum EventCategory : uint8_t
  {
    NoEventCategory       = 0,
    EventCategoryApplication = BIT(0),
    EventCategoryInput       = BIT(1),
    EventCategoryKeyboard    = BIT(2),
    EventCategoryMouse       = BIT(3),
    EventCategoryMouseButton = BIT(4)
  };
  
  /// Base class for all events in the IKan engine.
  /// Stores common APIs to identify and categorize events.
  class Event
  {
  public:
    /// Virtual destructor for base polymorphic class
    virtual ~Event() = default;
    
    // Virtual Interfaces ------------------------------------------------------------------------------------------------
    /// Returns the type of the triggered Event. (e.g., EventType::WindowClose)
    virtual EventType GetType() const noexcept = 0;
    
    /// Returns the name of the event as a string literal.
    virtual const char* GetName() const noexcept = 0;
    
    /// Returns the category flags of this event as a bitmask of EventCategory values.
    virtual int32_t GetCategoryFlags() const noexcept = 0;
    
    // Fundamental API ---------------------------------------------------------------------------------------------------
    /// Checks whether the event belongs to the given category
    /// - Parameter category: EventCategory type
    /// - Returns: true if event belongs to category, false otherwise
    [[nodiscard]] bool IsInCategory(EventCategory category) const noexcept
    {
      return (GetCategoryFlags() & category) != 0;
    }
    
  public:
    /// Flag indicating whether this event has been handled
    bool isHandled {false};
  };
  
  /// Dispatcher class to route events to the correct handler function.
  /// Provides a type-safe way to call event-specific handlers.
  class EventDispatcher
  {
  public:
    /// Constructs an EventDispatcher for the given event reference
    explicit EventDispatcher(Event& event) noexcept
    : m_event(event) {}
    
    /// Dispatches an event if the types match
    /// - Template Parameter T: The specific event type to check
    /// - Parameter func: Callable taking `T&` and returning bool (true = handled, false = unhandled)
    /// - Returns: true if dispatched and handler was invoked, false otherwise
    template<typename T, typename F>
    requires std::is_invocable_r_v<bool, F, T&> // Enforce correct handler signature
    bool Dispatch(const F& func)
    {
      if (m_event.GetType() == T::GetStaticType())
      {
        m_event.isHandled |= func(static_cast<T&>(m_event));
        return true;
      }
      return false;
    }
    
    DELETE_COPY_MOVE_CONSTRUCTORS(EventDispatcher);
    
  private:
    Event& m_event;
  };
  
  // Common typedef for event callback function
  using EventCallbackFn = std::function<void(Event&)>;
  
} // namespace IKan
