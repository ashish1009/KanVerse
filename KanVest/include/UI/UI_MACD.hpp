//
//  UI_MACD.hpp
//  KanVest
//
//  Created by Ashish . on 19/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  class UI_MACD
  {
  public:
    static void Show(const StockData& stockData);
  };
} // namespace KanVest
