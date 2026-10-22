/*!
 * @file Layer.hpp
 * @brief Base polymorphic layer contract for KanVerse application lifecycle.
 * @details Establishes the foundational interface for modular application components.
 *          Layers can be attached to the engine's LayerStack to receive deterministic
 *          per-frame simulation ticks, render callbacks, and routed platform events.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Core/Time/TimeStep.hpp"
#include "Platform/Events/Event.hpp"
#include <string>
#include <string_view>

namespace KanVerse {
  
  /*!
   * @class Layer
   * @brief Abstract base class representing an isolated update, render, and event slice.
   * @details Subclasses override lifecycle methods to implement gameplay logic, scene
   *          graphs, debug diagnostics, or user interfaces without coupling directly
   *          to the platform application runner.
   */
  class Layer {
  public:
    /*!
     * @brief Constructs a Layer with an identifying debug name.
     * @param debugName Descriptive tag used for engine profiling, diagnostics, and logging.
     */
    explicit Layer(std::string_view debugName = "Layer")
    : m_DebugName(debugName) {}
    
    /*!
     * @brief Virtual destructor ensuring clean polymorphic destruction of derived layers.
     */
    virtual ~Layer() = default;
    
    /*!
     * @brief Invoked immediately after the layer is added to the active LayerStack.
     * @details Ideal initialization point for allocating GPU resources, registering event
     *          listeners, or bootstrapping layer-specific data structures.
     */
    virtual void OnAttach() {}
    
    /*!
     * @brief Invoked immediately before the layer is removed from the active LayerStack
     *        or during engine teardown.
     * @details Guarantees an orderly release point for hardware buffers, shaders,
     *          and dynamically allocated memory.
     */
    virtual void OnDetach() {}
    
    /*!
     * @brief Per-frame simulation, physics, and gameplay logic update hook.
     * @param ts Discrete high-resolution delta time elapsed since the preceding frame tick.
     */
    virtual void OnUpdate(TimeStep ts) {}
    
    /*!
     * @brief Per-frame hardware rendering dispatch hook.
     * @details Invoked during the active swapchain render pass after simulation ticks complete.
     */
    virtual void OnRender() {}
    
    /*!
     * @brief Platform, window, and user input event dispatch handler.
     * @param event Reference to the dispatched event instance. Can be flagged as handled
     *              via event.SetHandled() to halt propagation to underlying layers.
     */
    virtual void OnEvent(Event& event) {}
    
    /*!
     * @brief Queries the layer's descriptive debug name.
     * @return Const reference to the internal identifier string.
     */
    [[nodiscard]] const std::string& GetName() const noexcept { return m_DebugName; }
    
  protected:
    std::string m_DebugName; ///< Descriptive label for engine tracing and diagnostics.
  };
  
} // namespace KanVerse
