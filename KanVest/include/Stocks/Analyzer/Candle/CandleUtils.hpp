//
//  CandleUtils.hpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
//

#pragma once

#include "Stocks/StockData.hpp"

#include "Stocks/Analyzer/Candle/CandleTypes.hpp"

namespace KanVest::Candle
{
  class CandleUtils
  {
  public:
    static double BodySize(const StockPoint& c);
    static double UpperWick(const StockPoint& c);
    static double LowerWick(const StockPoint& c);
    static bool IsBull(const StockPoint& c);
    static bool IsBear(const StockPoint& c);
    static bool IsDoji(const StockPoint& c, double threshold = 0.1);
  };
} // namespace KanVest::Candle
