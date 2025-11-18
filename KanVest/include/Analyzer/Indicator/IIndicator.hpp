//
//  IIndicator.hpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  class IIndicator
  {
  public:
    virtual ~IIndicator() = default;
    
    // Compute indicator based on stock data
    virtual void Compute(const StockData& data) = 0;
    
    // Reset internal state
    virtual void Reset() = 0;
  };
}
