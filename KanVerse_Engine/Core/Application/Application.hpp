/*!
 * @file Application.hpp
 * @brief Master application singleton orchestrating engine subsystems.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Core/Layers/Layer.hpp"
#include "Core/Layers/LayerStack.hpp"
#include "Core/Time/Clock.hpp"
#include "Core/Time/TimeStep.hpp"

#include "Platform/Events/Event.hpp"
#include "Platform/Events/WindowEvents.hpp"
#include "Platform/Window/Window.hpp"

#include "Renderer/Context/RenderContext.hpp"

namespace KanVerse {
  /*!
   * @struct ApplicationProps
   * @brief High-level boot configuration options for the application instance.
   */
  struct ApplicationProps {
    std::string Name{"KanVerse Application"}; ///< Window and application display title.
    uint32_t    WindowWidth{1280};             ///< Initial window viewport width.
    uint32_t    WindowHeight{720};             ///< Initial window viewport height.
    bool        VSync{true};                   ///< Vertical synchronization toggle.
    bool        Maximized{true};               ///< Maimized window.
  };

  /*!
   * @class Application
   * @brief Singleton orchestrator controlling windowing, event routing, and layer stacks.
   */
  class Application {
  public:
    /*!
     * @brief Initializes engine core subsystems, platform window, and render context.
     * @param props Startup properties defining window size and title.
     */
    explicit Application(const ApplicationProps& props = ApplicationProps());
    
    /*!
     * @brief Orderly teardown of layers, renderer context, and native window.
     */
    virtual ~Application();
    
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) noexcept = delete;
    Application& operator=(Application&&) noexcept = delete;
    
    /*!
     * @brief Starts the application event loop.
     */
    void Run();
    
    /*!
     * @brief Signals the application loop to terminate cleanly.
     */
    void Close();
    
    /*!
     * @brief Internal event handler routing incoming window and platform events to layers.
     * @param e Dispatched event reference.
     */
    void OnEvent(Event& e);
    
    /*!
     * @brief Appends a standard gameplay layer to the active layer stack.
     * @param layer Heap-allocated unique pointer owning the layer instance.
     */
    void PushLayer(std::unique_ptr<Layer> layer);
    
    /*!
     * @brief Appends an overlay layer to the top-most position of the stack.
     * @param overlay Heap-allocated unique pointer owning the overlay instance.
     */
    void PushOverlay(std::unique_ptr<Layer> overlay);
    
    /*!
     * @brief Accesses the native window abstraction.
     * @return Const reference to the Window interface.
     */
    [[nodiscard]] const Window& GetWindow() const noexcept { return *m_Window; }
    
    /*!
     * @brief Accesses the active render context.
     * @return Raw pointer to RenderContext.
     */
    [[nodiscard]] RenderContext* GetRenderContext() const noexcept { return m_RenderContext.get(); }
    
    /*!
     * @brief Queries the static global Application singleton instance.
     * @return Reference to the active Application.
     */
    [[nodiscard]] static Application& Get() noexcept { return *s_Instance; }
    
  private:
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);

    // Member Variables
    static Application* s_Instance;
    
    std::unique_ptr<RenderContext> m_RenderContext;
    std::unique_ptr<Window>        m_Window;
    
    LayerStack                     m_LayerStack;
    Clock                          m_Clock;
    
    bool                           m_Running{true};
    bool                           m_Minimized{false};
  };
  
  /*!
   * @brief Client entry point factory implemented by the game application.
   * @return Unique pointer owning the client-derived Application instance.
   */
  std::unique_ptr<Application> CreateApplication();
} // namespace KanVerse
