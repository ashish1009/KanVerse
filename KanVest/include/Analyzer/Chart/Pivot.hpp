//
//  Pivot.hpp
//  KanVest
//
//  Created by Ashish . on 24/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  struct SRLevel
  {
    double price;
    int count;        // how many touches (strength)
    int lastTouchIdx; // recency
  };
  
  struct PivotResults
  {
    std::vector<SRLevel> supports;
    std::vector<SRLevel> resistances;
  };
  
  struct Pivot
  {
  public:
    static PivotResults Compute(const StockData& data, int pivotRange = 3, double clusterTolerance = 0.30);
  };
} // namespace KanVest
