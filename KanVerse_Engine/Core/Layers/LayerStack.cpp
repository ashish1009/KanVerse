/*!
 * @file LayerStack.cpp
 * @brief Implementation of the LayerStack container and lifecycle management.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Core/Layers/LayerStack.hpp"
#include <algorithm>

namespace KanVerse {
  
  LayerStack::LayerStack() = default;
  
  LayerStack::~LayerStack() {
    // Detach in reverse order: Overlays first, then gameplay layers
    for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it) {
      (*it)->OnDetach();
    }
  }
  
  LayerStack::LayerStack(LayerStack&&) noexcept = default;
  LayerStack& LayerStack::operator=(LayerStack&&) noexcept = default;
  
  void LayerStack::PushLayer(std::unique_ptr<Layer> layer) {
    if (!layer) return;
    
    layer->OnAttach();
    m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, std::move(layer));
    ++m_LayerInsertIndex;
  }
  
  void LayerStack::PushOverlay(std::unique_ptr<Layer> overlay) {
    if (!overlay) return;
    
    overlay->OnAttach();
    m_Layers.emplace_back(std::move(overlay));
  }
  
  void LayerStack::PopLayer(Layer* layer) {
    const auto boundary = m_Layers.begin() + m_LayerInsertIndex;
    const auto it = std::find_if(m_Layers.begin(), boundary,
                                 [layer](const std::unique_ptr<Layer>& ptr) {
      return ptr.get() == layer;
    });
    
    if (it != boundary) {
      (*it)->OnDetach();
      m_Layers.erase(it);
      --m_LayerInsertIndex;
    }
  }
  
  void LayerStack::PopOverlay(Layer* overlay) {
    const auto boundary = m_Layers.begin() + m_LayerInsertIndex;
    const auto it = std::find_if(boundary, m_Layers.end(),
                                 [overlay](const std::unique_ptr<Layer>& ptr) {
      return ptr.get() == overlay;
    });
    
    if (it != m_Layers.end()) {
      (*it)->OnDetach();
      m_Layers.erase(it);
    }
  }
  
  void LayerStack::OnEvent(Event& event) {
    // Reverse iteration: Overlays handle events before underlying gameplay layers
    for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it) {
      if (event.IsHandled()) {
        break;
      }
      (*it)->OnEvent(event);
    }
  }
  
} // namespace KanVerse

