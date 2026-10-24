/*!
 * @file ImGuiLayer.hpp
 * @brief Dear ImGui overlay layer managing docking, multi-viewports, and event capture.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Core/Layers/Layer.hpp"

namespace MTL {
  class CommandBuffer;
  class RenderCommandEncoder;
  class RenderPassDescriptor;
}

namespace KanVerse {
  
  /*!
   * @class ImGuiLayer
   * @brief Engine overlay layer managing the Dear ImGui context and Metal/OSX backends.
   *
   * @note Thread-safety: Must be initialized and invoked exclusively on the main thread.
   */
  class ImGuiLayer : public Layer {
  public:
    /*!
     * @brief Constructs the ImGui overlay layer.
     */
    ImGuiLayer();
    
    /*!
     * @brief Destructor ensuring clean shutdown of ImGui resources.
     */
    ~ImGuiLayer() override = default;
    
    ImGuiLayer(const ImGuiLayer&) = delete;
    ImGuiLayer& operator=(const ImGuiLayer&) = delete;
    ImGuiLayer(ImGuiLayer&&) noexcept = delete;
    ImGuiLayer& operator=(ImGuiLayer&&) noexcept = delete;
    
    /*!
     * @brief Initializes the ImGui context, dark theme, docking flags, and Metal/OSX backends.
     */
    void OnAttach() override;
    
    /*!
     * @brief Shuts down the Metal/OSX backends and destroys the ImGui context.
     */
    void OnDetach() override;
    
    /*!
     * @brief Filters mouse and keyboard events when `m_BlockEvents` is enabled.
     * @param event Dispatched platform event.
     */
    void OnEvent(Event& event) override;
    
    /*!
     * @brief Starts a new Dear ImGui frame for the given Metal render pass descriptor.
     * @param passDescriptor Active swapchain render pass descriptor.
     */
    void Begin(MTL::RenderPassDescriptor* passDescriptor);
    
    /*!
     * @brief Finalizes ImGui draw data and encodes draw commands into the active Metal pass.
     * @param commandBuffer Active Metal command buffer.
     * @param renderEncoder Active swapchain render command encoder.
     */
    void End(MTL::CommandBuffer* commandBuffer, MTL::RenderCommandEncoder* renderEncoder);
    
    /*!
     * @brief Controls whether ImGui blocks mouse/keyboard events from reaching underlying layers.
     * @param block True to block events when hovering UI outside the 3D viewport, false to pass through.
     */
    void BlockEvents(bool block) noexcept { m_BlockEvents = block; }
    
  private:
    bool m_BlockEvents{true}; ///< True when UI panels should consume mouse and keyboard events.
  };
  
} // namespace KanVerse
