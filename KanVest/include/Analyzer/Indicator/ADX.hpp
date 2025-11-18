//
//  ADX.hpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  struct ADXResult
  {
    std::vector<double> adx;
    std::vector<double> plusDI;
    std::vector<double> minusDI;
    
    double last = 0.0;
    bool valid = false;
  };
  
  class ADXCalculator
  {
  public:
    static ADXResult Compute(const StockData& stock, int period = 14);
  };
} // namespace KanVest
