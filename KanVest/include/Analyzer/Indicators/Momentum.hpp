//
//  Momentum.hpp
//  KanVest
//
//  Created by Ashish . on 15/01/26.
//

#pragma once

#include "Stock/StockMetadata.hpp"

namespace KanVest
{
  struct RSISeries
  {
    std::vector<double> series;
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
  };
} // namespace KanVest
