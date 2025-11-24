//
//  MovingAverage.hpp
//  KanVest
//
//  Created by Ashish . on 23/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  struct MAResult
  {
    std::map<int, double> smaValues;
    std::map<int, double> emaValues;
  };

  class MovingAverage
  {
  public:
    static MAResult Compute(const StockData& data);
    
  private:
    static std::vector<double> ComputeSMA(const std::vector<double>& closes, int period);
    static std::vector<double> ComputeEMA(const std::vector<double>& closes, int period);
  };
} // namespace KanVest
