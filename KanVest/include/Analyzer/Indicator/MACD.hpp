//
//  MACD.hpp
//  KanVest
//
//  Created by Ashish . on 24/11/25.
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
  
  class MACD
  {
  public:
    static MACDResult Compute(const StockData& stock);
  };
} // namespace KanVest
