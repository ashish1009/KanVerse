//
//  MFI.hpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  struct MFIResult
  {
    std::vector<double> series;   // full MFI values
    double last = 0.0;            // latest MFI
    bool valid = false;
  };
  
  class MFI
  {
  public:
    static MFIResult Compute(const StockData& data, int period = 14);
  };
} // namespace KanVest
