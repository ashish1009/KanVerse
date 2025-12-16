//
//  IndicatorUtils.hpp
//  KanVest
//
//  Created by Ashish . on 15/12/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest::Indicator::Utils
{
  /// This builds closes vector for day based candle (input can be any candle data, 5m, 1h, 1d, 1wk ...)
  /// - Parameter data: stock data
  std::vector<double> BuildDailyCloses(const StockData& data);
  /// This builds closes vector for day based candle
  /// - Parameter data: stock data
  std::vector<double> GetCandleCloses(const StockData& data);
} // namespace KanVest::Indicator::Utils
