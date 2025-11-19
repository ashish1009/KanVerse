//
//  UI_Momentum.hpp
//  KanVest
//
//  Created by Ashish . on 19/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  class UI_Momentum
  {
  public:
    static void ShowRSI(const StockData& stockData);
  };
} // namespace KanVest
