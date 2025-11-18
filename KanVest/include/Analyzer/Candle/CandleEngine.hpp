//
//  CandleEngine.hpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

#include "Analyzer/Candle/CandleData.hpp"

namespace KanVest::Candle
{
  class CandlePatternEngine
  {
  public:
    // Analyze the latest N candles and return detected patterns with their candle index
    // indices are relative to stock.history (0 = oldest)
    static std::vector<std::pair<size_t, CandlePattern>> Analyze(const StockData& stock, size_t lookback = 100);
  };
} // namespace KanVest::Candle
