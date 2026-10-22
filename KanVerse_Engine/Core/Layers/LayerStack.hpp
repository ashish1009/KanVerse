/*!
 * @file LayerStack.hpp
 * @brief Container interface managing ordered ownership of application and overlay layers.
 * @details Implements a dual-partition contiguous container where standard gameplay layers
 *          occupy the lower partition (executing forward) and overlay layers occupy the upper
 *          partition (guaranteed top-most rendering and highest event priority).
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Core/Layers/Layer.hpp"
#include <vector>
#include <memory>

namespace KanVerse {
  
  /*!
   * @class LayerStack
   * @brief Dual-partitioned container orchestrating the lifecycle and dispatch order of layers.
   * @note Standard game layers are pushed at the middle insertion index, preserving insertion
   *       order while ensuring all overlays remain strictly after regular layers.
   */
  class LayerStack {
  public:
    /*!
     * @brief Default constructor initializing an empty layer stack with a zero insertion index.
     */
    LayerStack();
    
    /*!
     * @brief Destructor that detaches and deallocates all contained layers in reverse order.
     * @details Reverse traversal guarantees that dependent layers and overlays are destroyed
     *          before foundational systems they might depend on.
     */
    ~LayerStack();
    
    LayerStack(const LayerStack&) = delete;
    LayerStack& operator=(const LayerStack&) = delete;
    LayerStack(LayerStack&&) noexcept;
    LayerStack& operator=(LayerStack&&) noexcept;
    
    /*!
     * @brief Inserts a standard game layer at the current normal layer boundary.
     * @param layer Unique pointer taking ownership of the heap-allocated Layer instance.
     * @note Invokes layer->OnAttach() upon insertion.
     */
    void PushLayer(std::unique_ptr<Layer> layer);
    
    /*!
     * @brief Appends an overlay layer to the top-most end of the stack.
     * @param overlay Unique pointer taking ownership of the overlay Layer instance.
     * @note Invokes overlay->OnAttach() upon insertion. Overlays receive events first
     *       and render last.
     */
    void PushOverlay(std::unique_ptr<Layer> overlay);
    
    /*!
     * @brief Locates and removes a standard game layer by raw pointer reference.
     * @param layer Raw pointer matching the targeted layer instance.
     * @note Invokes layer->OnDetach() prior to erasure and decrements the insertion index.
     */
    void PopLayer(Layer* layer);
    
    /*!
     * @brief Locates and removes an overlay layer by raw pointer reference.
     * @param overlay Raw pointer matching the targeted overlay instance.
     * @note Invokes overlay->OnDetach() prior to erasure from the upper partition.
     */
    void PopOverlay(Layer* overlay);
    
    /*!
     * @brief Propagates an event backward through the stack (overlays first, then regular layers).
     * @param event Reference to the platform or window event being processed.
     * @details Traversal terminates early if any layer marks the event as handled.
     */
    void OnEvent(Event& event);
    
    /*!
     * @brief Returns a forward iterator to the first layer in the stack.
     */
    [[nodiscard]] auto begin() noexcept { return m_Layers.begin(); }
    
    /*!
     * @brief Returns a forward iterator to the end of the layer container.
     */
    [[nodiscard]] auto end() noexcept { return m_Layers.end(); }
    
    /*!
     * @brief Returns a const forward iterator to the first layer in the stack.
     */
    [[nodiscard]] auto begin() const noexcept { return m_Layers.begin(); }
    
    /*!
     * @brief Returns a const forward iterator to the end of the layer container.
     */
    [[nodiscard]] auto end() const noexcept { return m_Layers.end(); }
    
  private:
    std::vector<std::unique_ptr<Layer>> m_Layers;       ///< Contiguous storage of polymorphic layers.
    uint32_t                            m_LayerInsertIndex{0}; ///< Partition index dividing regular layers from overlays.
  };
  
} // namespace KanVerse
