//
//  Portfolio.hpp
//  KanVest
//
//  Created by Ashish . on 21/11/25.
//

#pragma once

namespace KanVest
{
  struct Holding
  {
    // To be serialize
    std::string symbolName;
    float averagePrice = 0.0f;
    uint32_t quantity = 0;
    
    // Compute dynamic not serisalize
    float stockValue;
    float investment;
    float value;
    float dayChange;
    float dayChangePercent;
    float profitLoss;
    float profitLossPercent;
  };
} // namespace KanVest
