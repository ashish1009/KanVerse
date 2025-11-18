//
//  Stochastic.hpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  struct StochasticResult
  {
    std::vector<double> kSeries;   // %K values
    std::vector<double> dSeries;   // %D values
    double lastK = 0.0;
    double lastD = 0.0;
  };
  
  class Stochastic
  {
  public:
    static StochasticResult Compute(const StockData& data, int period = 14);
  };
} // namespace KanVest
