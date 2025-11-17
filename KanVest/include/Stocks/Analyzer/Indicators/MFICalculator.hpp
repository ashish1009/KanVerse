//
//  MFICalculator.hpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
//

#pragma once

#include "Stocks/StockData.hpp"

namespace KanVest
{
  struct MFIResult
  {
    std::vector<double> series;   // full MFI values
    double last = 0.0;            // latest MFI
    bool valid = false;
  };

  class MFICalculator
  {
  public:
    static MFIResult Compute(const StockData& data, int period = 14);
  };
} // namespace KanVest
