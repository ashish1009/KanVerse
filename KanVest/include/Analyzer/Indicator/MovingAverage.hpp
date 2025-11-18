//
//  MovingAverage.hpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

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
    static std::vector<double> ComputeSMA(const std::vector<double>& closes, int period);
    static std::vector<double> ComputeEMA(const std::vector<double>& closes, int period);
    
    // NEW: Normalize ANY interval (1D, 1W, 1h) → DAILY close series
    static std::vector<double> BuildDailyCloses(const StockData& data);
    
    static MAResult Compute(const StockData& data, const std::string& chartRange);
  };
} // namespace KanVest
