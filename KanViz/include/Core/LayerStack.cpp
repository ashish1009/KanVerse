//
//  LayerStack.cpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#include "LayerStack.hpp"

namespace KanViz
{
  LayerStack::LayerStack()
  : VectorIterator<Ref<Layer>>(m_layers)
  {
  }
  
  LayerStack::~LayerStack()
  {
    // Ensure all layers are detached properly before clearing
    for (auto& layer : m_layers)
    {
      if (layer)
        layer->OnDetach();
    }
    m_layers.clear();
    m_layerInsertIndex = 0;
  }
  
  void LayerStack::PushLayer(const Ref<Layer>& layer)
  {
    IK_PROFILE();
    if (!layer || Contain(layer))
    {
      IK_LOG_ERROR(LogModule::Layers, "Attempted to push a null or duplicate layer.");
      return;
    }
    
    m_layers.insert(m_layers.begin() + static_cast<ptrdiff_t>(m_layerInsertIndex), layer);
    m_layerInsertIndex++;
    
    IK_LOG_INFO(LogModule::Layers,
                "Pushed Layer '{0}' at position {1}. Total Layers: {2}",
                layer->GetName().c_str(), m_layerInsertIndex - 1, m_layers.size());
    
    layer->OnAttach();
  }
  
  void LayerStack::PopLayer(const Ref<Layer>& layer)
  {
    IK_PROFILE();
    if (!layer) return;
    
    auto it = std::find(m_layers.begin(), m_layers.begin() + static_cast<ptrdiff_t>(m_layerInsertIndex), layer);
    if (it != m_layers.begin() + static_cast<ptrdiff_t>(m_layerInsertIndex))
    {
      layer->OnDetach();
      m_layers.erase(it);
      m_layerInsertIndex--;
      
      IK_LOG_WARN(LogModule::Layers,
                  "Popped Layer '{0}' from index {1}. Total Layers: {2}",
                  layer->GetName().c_str(), m_layerInsertIndex, m_layers.size());
    }
  }
  
  void LayerStack::PushOverlay(const Ref<Layer>& layer)
  {
    IK_PROFILE();
    if (!layer || Contain(layer))
    {
      IK_LOG_ERROR(LogModule::Layers, "Attempted to push a null or duplicate overlay.");
      return;
    }
    
    m_layers.emplace_back(layer);
    
    IK_LOG_INFO(LogModule::Layers,
                "Pushed Overlay '{0}' at the end. Total Layers: {1}",
                layer->GetName().c_str(), m_layers.size());
    
    layer->OnAttach();
  }
  
  void LayerStack::PopOverlay(const Ref<Layer>& layer)
  {
    IK_PROFILE();
    if (!layer) return;
    
    auto it = std::find(m_layers.begin() + static_cast<ptrdiff_t>(m_layerInsertIndex), m_layers.end(), layer);
    if (it != m_layers.end())
    {
      layer->OnDetach();
      m_layers.erase(it);
      
      IK_LOG_WARN(LogModule::Layers,
                  "Popped Overlay '{0}' from the end. Total Layers: {1}",
                  layer->GetName().c_str(), m_layers.size());
    }
  }
  
  bool LayerStack::Contain(const Ref<Layer>& layer) const noexcept
  {
    return std::ranges::contains(m_layers, layer);
  }
} // namespace KanViz
