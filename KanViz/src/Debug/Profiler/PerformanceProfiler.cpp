//
//  PerformanceProfiler.cpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#include "PerformanceProfiler.hpp"

namespace KanViz
{
  PerformanceProfile& PerformanceProfile::Get()
  {
    static PerformanceProfile instance;
    return instance;
  }
  
  void PerformanceProfile::BeginFrame()
  {
    m_root = std::make_unique<PerfNode>();
    m_root->name = "Frame";
    m_current = m_root.get();
  }
  
  void PerformanceProfile::EndFrame()
  {
    // Nothing yet, could dump or aggregate later
  }
  
  void PerformanceProfile::Clear()
  {
    m_root.reset();
    m_current = nullptr;
  }
  
  PerfNode* PerformanceProfile::PushNode(const std::string& name)
  {
    auto node = std::make_unique<PerfNode>();
    node->name = name;
    node->parent = m_current;
    
    PerfNode* rawPtr = node.get();
    if (m_current)
    {
      m_current->children.push_back(std::move(node));
      m_current = rawPtr;
    }
    return rawPtr;
  }
  
  void PerformanceProfile::PopNode(double durationMicro)
  {
    if (m_current)
    {
      m_current->durationMicro = durationMicro;
      if (m_current->parent)
      {
        m_current = m_current->parent;
      }
    }
  }
} // namespace KanViz
