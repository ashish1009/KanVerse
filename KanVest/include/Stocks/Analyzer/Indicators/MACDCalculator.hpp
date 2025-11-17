//
//  MACDCalculator.hpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
//

#pragma once

#include "Stocks/StockData.hpp"

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
