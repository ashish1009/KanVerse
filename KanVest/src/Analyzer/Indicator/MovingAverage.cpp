//
//  MovingAverage.cpp
//  KanVest
//
//  Created by Ashish . on 23/11/25.
//

#include "MovingAverage.hpp"

namespace KanVest
{
  MAResult MovingAverage::Compute(const StockData& data)
  {
    MAResult result;

    if (!data.IsValid())
    {
      return result;
    }

    return result;
  }
} // namespace KanVest
