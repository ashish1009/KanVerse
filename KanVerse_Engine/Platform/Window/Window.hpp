/*!
 * @file Window.hpp
 * @brief Abstract platform-independent window interface.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Platform/Events/Event.hpp"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace KanVerse {
  
  /*!
   * @struct WindowProps
   * @brief Configuration properties used to construct a native window.
   */
  struct WindowProps {
    std::string Title{"KanVerse Engine"};
    uint32_t    Width{1280};
    uint32_t    Height{720};
    bool        VSync{true};
    bool        Maximized{false};
  };
  
  /*!
   * @class Window
   * @brief Abstract base class defining the standard platform window contract.
   */
  class Window {
  public:
    using EventCallbackFn = std::function<void(Event&)>;
    
    virtual ~Window() = default;
    
    /*!
     * @brief Dispatches OS window events and processes message queues.
     */
    virtual void OnUpdate() = 0;
    
    /*!
     * @brief Queries current client width in logical points.
     * @return Horizontal extent.
     */
    [[nodiscard]] virtual uint32_t GetWidth() const noexcept = 0;
    
    /*!
     * @brief Queries current client height in logical points.
     * @return Vertical extent.
     */
    [[nodiscard]] virtual uint32_t GetHeight() const noexcept = 0;
    
    /*!
     * @brief Registers the event dispatch callback receiving routed OS events.
     * @param callback Function handler matching void(Event&).
     */
    virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
    
    /*!
     * @brief Enables or disables vertical synchronization.
     * @param enabled True to limit swap rate to display refresh.
     */
    virtual void SetVSync(bool enabled) = 0;
    
    /*!
     * @brief Queries whether vertical synchronization is currently active.
     * @return True if VSync is enabled.
     */
    [[nodiscard]] virtual bool IsVSync() const noexcept = 0;
    
    /*!
     * @brief Retrieves the opaque native platform handle (e.g. NSWindow* on macOS).
     * @return Raw pointer to platform handle.
     */
    [[nodiscard]] virtual void* GetNativeWindow() const noexcept = 0;
    
    /*!
     * @brief Toggles or sets the window maximized/zoomed state.
     */
    virtual void Maximize() = 0;
    
    /*!
     * @brief Checks if the window is currently zoomed/maximized.
     * @return True if maximized, false otherwise.
     */
    [[nodiscard]] virtual bool IsMaximized() const noexcept = 0;
    
    /*!
     * @brief Factory method constructing the concrete platform window implementation.
     * @param props Construction properties (title, dimensions, VSync).
     * @return Unique pointer owning the initialized Window instance.
     */
    [[nodiscard]] static std::unique_ptr<Window> Create(const WindowProps& props = WindowProps());
  };
  
} // namespace KanVerse
