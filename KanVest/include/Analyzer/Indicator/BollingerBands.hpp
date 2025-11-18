//
//  BollingerBands.hpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  struct BBResult
  {
    std::vector<double> middle;
    std::vector<double> upper;
    std::vector<double> lower;
    std::vector<double> deviation;
    
    double lastMiddle = 0.0;
    double lastUpper  = 0.0;
    double lastLower  = 0.0;
    
    bool valid = false;
  };
  
  class BollingerBands
  {
  public:
    static BBResult Compute(const StockData& stock, int period = 20, double k = 2.0);
  };
} // namespace KanVest
