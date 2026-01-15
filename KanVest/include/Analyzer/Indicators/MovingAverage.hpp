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
  static const std::vector<int> ValidMovingAveragePeriods = {5, 10, 20, 30, 50, 100, 150, 200};

  enum class MAPriceSource
  {
    Close,
    Limit
  };
  
  struct MAResult
  {
    std::map<int, std::vector<double>> dmaValues;
    std::map<int, std::vector<double>> emaValues;
  };
  
  class MovingAverage
  {
  public:
    [[nodiscard("Moving average is not used")]] static std::unordered_map<MAPriceSource, MAResult> Compute(const StockData& data);
    
  private:
    static std::vector<double> ComputeDMA(const std::vector<double>& closes, int period, MAPriceSource priceSource);
    static std::vector<double> ComputeEMA(const std::vector<double>& closes, int period, MAPriceSource priceSource);

    friend class MACD;
  };
} // namespace KanVest
