//
//  UI_Stochastic.hpp
//  KanVest
//
//  Created by Ashish . on 19/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  class UI_Stochastic
  {
  public:
    static void Show(const StockData& stockData);
  };
} // namespace KanVest
