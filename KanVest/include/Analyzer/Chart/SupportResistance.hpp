//
//  SupportResistance.hpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
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
  
  struct SupportResistance
  {
    std::vector<SRLevel> supports;
    std::vector<SRLevel> resistances;
    
    SupportResistance(const StockData& data, int pivotRange = 3, double clusterTolerance = 0.30);
    
  private:
    void Compute(const StockData& data, int pivotRange = 3, double clusterTolerance = 0.30);
  };
} // namespace KanVest
