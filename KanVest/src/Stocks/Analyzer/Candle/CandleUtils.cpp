//
//  CandleUtils.cpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
//

#include "CandleUtils.hpp"

namespace KanVest::Candle
{
  double CandleUtils::BodySize(const StockPoint& c) { return std::fabs(c.close - c.open); }
  double CandleUtils::UpperWick(const StockPoint& c) { return c.high - std::max(c.open, c.close); }
  double CandleUtils::LowerWick(const StockPoint& c) { return std::min(c.open, c.close) - c.low; }
  bool CandleUtils::IsBull(const StockPoint& c) { return c.close > c.open; }
  bool CandleUtils::IsBear(const StockPoint& c) { return c.close < c.open; }
   
  bool CandleUtils::IsDoji(const StockPoint& c, double threshold)
  {
    double range = c.high - c.low;
    if (range <= 0.0) return false;
    return BodySize(c) <= range * threshold;
  }
} // namespace KanVest::Candle
