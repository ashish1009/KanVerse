//
//  PerformanceProfiler.hpp
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#pragma once

namespace KanViz
{
  /// This structure stores the timer for a node (scope)
  class PerfNode
  {
  public:
    std::string name;
    double durationMicro = 0.0;
    std::vector<std::unique_ptr<PerfNode>> children;
    PerfNode* parent = nullptr;
  };
  
  class PerformanceProfile
  {
  public:
    static PerformanceProfile& Get();
    
    /// Called at start of frame
    void BeginFrame();
    
    /// Called at end of frame
    void EndFrame();
    
    /// Clears data
    void Clear();
    
    /// Returns root node of current frame
    PerfNode* GetRoot() const { return m_root.get(); }
    
    /// Internal: Push a node into tree
    PerfNode* PushNode(const std::string& name);
    
    /// Internal: Pop a node out of tree
    void PopNode(double durationMicro);
    
  private:
    PerformanceProfile() = default;
    ~PerformanceProfile() = default;
    
    Scope<PerfNode> m_root = nullptr;
    PerfNode* m_current = nullptr;
  };
} // namespace KanViz
