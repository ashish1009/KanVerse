//
//  CandleDetector.hpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
//

#pragma once

#include "Stocks/StockData.hpp"

#include "Stocks/Analyzer/Candle/CandleTypes.hpp"

namespace KanVest::Candle
{
  // Single-candle detectors
  CandlePattern DetectDoji(const StockPoint& p);
  CandlePattern DetectHammer(const StockPoint& p);
  CandlePattern DetectShootingStar(const StockPoint& p);
  CandlePattern DetectMarubozu(const StockPoint& p);
  
  // Two-candle detectors (requires last two candles oldest->newest)
  CandlePattern DetectBullishEngulfing(const StockPoint& prev, const StockPoint& cur);
  CandlePattern DetectBearishEngulfing(const StockPoint& prev, const StockPoint& cur);
  
  // Three-candle detectors (requires sequence of three)
  CandlePattern DetectMorningStar(const std::vector<StockPoint>& window);
  CandlePattern DetectEveningStar(const std::vector<StockPoint>& window);
} // namespace KanVest::Candle
