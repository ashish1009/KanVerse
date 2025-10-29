//
//  LayerStack.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Core/Layer.hpp"

namespace KanViz
{
  /// @brief This class stores all the layer pointers in stack to execute them in application.
  class LayerStack : public VectorIterator<Ref<Layer>>
  {
  public:
    // Constructor & Destructor -------------------------------------------------------------------------------------------------------------------
    /// @brief Creates an empty LayerStack instance.
    LayerStack();
    
    /// @brief Destroys the LayerStack instance and detaches all layers.
    virtual ~LayerStack();
    
    // Core API -------------------------------------------------------------------------------------------------------------------
    /// @brief Inserts a layer at the next available index (before overlays).
    /// @details Calls Layer::OnAttach() when the layer is pushed.
    void PushLayer(const Ref<Layer>& layer);
    
    /// @brief Removes a layer from the main stack (not overlays).
    /// @details Calls Layer::OnDetach() when the layer is popped.
    void PopLayer(const Ref<Layer>& layer);
    
    /// @brief Inserts an overlay at the end of the stack.
    /// @details Calls Layer::OnAttach() when the overlay is pushed.
    void PushOverlay(const Ref<Layer>& layer);
    
    /// @brief Removes an overlay from the end of the stack.
    /// @details Calls Layer::OnDetach() when the overlay is popped.
    void PopOverlay(const Ref<Layer>& layer);
    
    /// @brief Checks if the given layer exists in the stack.
    [[nodiscard]] bool Contain(const Ref<Layer>& layer) const noexcept;
    
  private:
    uint32_t m_layerInsertIndex {0}; ///< Tracks the index for the next layer insertion
    std::vector<Ref<Layer>> m_layers; ///< Storage for layers and overlays
  };
} // namespace KanViz
