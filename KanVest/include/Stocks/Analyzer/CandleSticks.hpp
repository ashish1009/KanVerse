//
//  CandleSticks.hpp
//  KanVest
//
//  Created by Ashish . on 07/11/25.
//

#pragma once

#include "Stocks/Analyzer/AnalyzerStrucutre.hpp"

#include "Stocks/StockManager.hpp"

namespace KanVest::Analysis::Candles
{
  // Detect classic single-bar patterns; returns PatternHit list
  std::vector<PatternHit> DetectSingleBarPatterns(const std::vector<StockPoint>& history);
  
  // Detect multi-bar candlestick patterns (3-bar like morning star, engulfing etc.)
  std::vector<PatternHit> DetectMultiBarPatterns(const std::vector<StockPoint>& history);
} // namespace KanVest::Analysis::Candles
