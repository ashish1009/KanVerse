//
//  MovingAverage.hpp
//  KanVest
//
//  Created by Ashish . on 14/01/26.
//

#pragma once

#include "Stock/StockMetadata.hpp"

namespace KanVest
{
  static const std::vector<int> ValidPeriods = {5, 10, 20, 30, 50, 100, 150, 200};
  
  struct MAResult
  {
    std::map<int, std::vector<double>> dmaValues;
  };
  
  class MovingAverage
  {
  public:
    static MAResult Compute(const StockData& data);
    
  private:
    static std::vector<double> ComputeDMA(const std::vector<double>& closes, int period);
    
    friend class MACD;
  };
} // namespace KanVest
