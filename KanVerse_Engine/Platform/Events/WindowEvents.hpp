/*!
 * @file WindowEvents.hpp
 * @brief Window lifecycle, resize, and close event definitions.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Platform/Events/Event.hpp"

namespace KanVerse {
  
  /*!
   * @class WindowResizeEvent
   * @brief Fired whenever the underlying native window dimensions mutate.
   */
  class WindowResizeEvent : public Event {
  public:
    /*!
     * @brief Constructs a resize event with new pixel dimensions.
     * @param width Framebuffer width in pixels.
     * @param height Framebuffer height in pixels.
     */
    WindowResizeEvent(uint32_t width, uint32_t height) noexcept
    : m_Width(width), m_Height(height) {}
    
    [[nodiscard]] uint32_t GetWidth() const noexcept { return m_Width; }
    [[nodiscard]] uint32_t GetHeight() const noexcept { return m_Height; }
    
    EVENT_CLASS_TYPE(WindowResize)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
    
  private:
    uint32_t m_Width{0};
    uint32_t m_Height{0};
  };
  
  /*!
   * @class WindowCloseEvent
   * @brief Fired when the native window receives an OS close request.
   */
  class WindowCloseEvent : public Event {
  public:
    WindowCloseEvent() noexcept = default;
    
    EVENT_CLASS_TYPE(WindowClose)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
  };
  
} // namespace KanVerse
