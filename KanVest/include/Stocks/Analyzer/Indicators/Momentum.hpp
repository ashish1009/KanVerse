//
//  Momentum.hpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
//

#pragma once

#include "Stocks/StockData.hpp"

namespace KanVest
{
  struct RSISeries
  {
    // One-to-one with history; entries before `period` will be NaN (or 0) depending on use.
    std::vector<double> series;
    // Convenience: last computed RSI (or NaN if not enough data)
    double last = std::numeric_limits<double>::quiet_NaN();
  };
  
  class RSI
  {
  public:
    // Computes the RSI series using Wilder's smoothing (the method brokers and TradingView/AngelOne use).
    // - history: vector of StockPoint ordered oldest -> newest
    // - period: typical default 14
    // Returns full RSISeries where series[i] corresponds to history[i] (first period-1 entries will be NaN)
    static RSISeries Compute(const StockData& data, size_t period = 14);
    
    // Tooltip explanation for UI / tooltip text
    static std::string TooltipRSI();
  };
} // namespace KanVest
