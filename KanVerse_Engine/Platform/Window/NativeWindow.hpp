/*!
 * @file NativeWindow.hpp
 * @brief macOS AppKit native window implementation header.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Platform/Window/Window.hpp"

namespace KanVerse {
  
  /*!
   * @class NativeWindow
   * @brief Apple Silicon / macOS AppKit implementation of the Window interface.
   */
  class NativeWindow : public Window {
  public:
    /*!
     * @struct WindowData
     * @brief Internal mutable state container shared with OS callbacks and delegates.
     */
    struct WindowData {
      std::string     Title;                 ///< Active window title text.
      uint32_t        Width{0};              ///< Framebuffer width in logical points.
      uint32_t        Height{0};             ///< Framebuffer height in logical points.
      bool            VSync{true};           ///< Active vertical synchronization toggle.
      bool            Maximized{true};       ///< Start Window Maximized.
      EventCallbackFn EventCallback;         ///< Callback dispatching events to the engine.
    };
    
    /*!
     * @brief Constructs and presents the native macOS Cocoa window.
     * @param props Window configuration specifications.
     */
    explicit NativeWindow(const WindowProps& props);
    
    /*!
     * @brief Teardown releasing AppKit window resources and delegates.
     */
    ~NativeWindow() override;
    
    NativeWindow(const NativeWindow&) = delete;
    NativeWindow& operator=(const NativeWindow&) = delete;
    NativeWindow(NativeWindow&&) noexcept = delete;
    NativeWindow& operator=(NativeWindow&&) noexcept = delete;
    
    /*!
     * @brief Drains the macOS Cocoa event loop.
     */
    void OnUpdate() override;
    
    /*!
     * @brief Retrieves the current window width in logical points.
     * @return Framebuffer width.
     */
    [[nodiscard]] uint32_t GetWidth() const noexcept override { return m_Data.Width; }
    
    /*!
     * @brief Retrieves the current window height in logical points.
     * @return Framebuffer height.
     */
    [[nodiscard]] uint32_t GetHeight() const noexcept override { return m_Data.Height; }
    
    /*!
     * @brief Registers the event dispatch callback receiving routed OS events.
     * @param callback Invocable receiving platform and window events.
     */
    void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
    
    /*!
     * @brief Enables or disables vertical synchronization.
     * @param enabled True to lock frame presentation to display refresh rate.
     */
    void SetVSync(bool enabled) override;
    
    /*!
     * @brief Queries whether vertical synchronization is currently active.
     * @return True if presentation is synchronized to display refresh.
     */
    [[nodiscard]] bool IsVSync() const noexcept override { return m_Data.VSync; }
    
    /*!
     * @brief Retrieves the opaque native platform handle (NSWindow* on macOS).
     * @return Opaque pointer to the NSWindow instance.
     */
    [[nodiscard]] void* GetNativeWindow() const noexcept override;
    
    /*!
     * @brief Toggles or sets the window maximized/zoomed state.
     */
    void Maximize() override;
    
    /*!
     * @brief Checks if the window is currently zoomed/maximized.
     * @return True if maximized, false otherwise.
     */
    [[nodiscard]] bool IsMaximized() const noexcept override;
    
    /*!
     * @brief Retrieves the underlying MTKView pointer for render pass attachment.
     * @return Opaque pointer to MTKView.
     */
    [[nodiscard]] void* GetNativeView() const noexcept;
    
  private:
    WindowData m_Data;                    ///< Configuration state and callback bindings.
    void*      m_WindowHandle{nullptr};   ///< Opaque NSWindow* handle.
    void*      m_ViewHandle{nullptr};     ///< Opaque MTKView* handle.
    void*      m_DelegateHandle{nullptr}; ///< Opaque NSWindowDelegate* handle.
    
    /*!
     * @brief Configures Cocoa application activation, menubar, and window attributes.
     * @param props Construction properties (title, dimensions, VSync).
     */
    void Init(const WindowProps& props);
    
    /*!
     * @brief Orderly teardown releasing Cocoa delegates, views, and windows.
     */
    void Shutdown();
  };
  
} // namespace KanVerse
