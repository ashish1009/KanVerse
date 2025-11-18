//
//  MACD.hpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  struct MACDResult
  {
    std::vector<double> macdLine;
    std::vector<double> signalLine;
    std::vector<double> histogram;
  };
  
  class MACDCalculator
  {
  public:
    static MACDResult Compute(const StockData& stock);
  };
} // namespace KanVest
