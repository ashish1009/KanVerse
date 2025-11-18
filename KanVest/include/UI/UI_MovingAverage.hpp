//
//  UI_MovingAverage.hpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  class UI_MovingAverage
  {
  public:
    static void ShowSMA(const StockData& stockData, const std::string& range);
    static void ShowEMA(const StockData& stockData, const std::string& range);
    
  private:
    static void ShowMovingAverageData(const StockData& stockData, const std::unordered_map<int, double>& maMap, const std::string& maString);
  };
} // namespace KanVest
