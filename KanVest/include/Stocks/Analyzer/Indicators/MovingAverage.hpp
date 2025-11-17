//
//  MovingAverage.hpp
//  KanVest
//
//  Created by Ashish . on 13/11/25.
//

#pragma once

#include "Stocks/StockData.hpp"

namespace KanVest
{
  struct MAResult
  {
    std::unordered_map<int, double> smaValues;
    std::unordered_map<int, double> emaValues;
  };
  
  class MovingAverages
  {
  public:
    // Compute both SMA and EMA based on available data and chart range
    static MAResult Compute(const StockData& data, const std::string& chartRange);
    
    // For internal use or testing
    static std::vector<double> ComputeSMA(const std::vector<double>& closes, int period);
    static std::vector<double> ComputeEMA(const std::vector<double>& closes, int period);
    
    // Tooltip explanations for UI
    static std::string TooltipSMA();
    static std::string TooltipEMA();
  };
} // namespace KanVest
