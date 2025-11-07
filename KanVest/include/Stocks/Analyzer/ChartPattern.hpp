//
//  ChartPattern.hpp
//  KanVest
//
//  Created by Ashish . on 07/11/25.
//

#pragma once

#include "Stocks/Analyzer/AnalyzerStrucutre.hpp"
#include "Stocks/StockManager.hpp"

namespace KanVest::Analysis::ChartPatterns
{
  // Detect double top/bottom (returns hits with neckline level and measured move)
  std::vector<PatternHit> DetectDoubleTopBottom(const std::vector<StockPoint>& history);
  
  // Detect Head & Shoulders and Inverse H&S
  std::vector<PatternHit> DetectHeadAndShoulders(const std::vector<StockPoint>& history);
  
  // Detect triangles, flags, pennants (continuation)
  std::vector<PatternHit> DetectTrianglesFlags(const std::vector<StockPoint>& history);
  
  // Generic trend detection (HH/HL / LH/LL)
  PatternHit DetectTrendStructure(const std::vector<StockPoint>& history);
} // namespace KanVest::Analysis::ChartPatterns
