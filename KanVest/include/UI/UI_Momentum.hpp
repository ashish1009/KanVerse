//
//  UI_Momentum.hpp
//  KanVest
//
//  Created by Ashish . on 15/01/26.
//

#pragma once

#include "Stock/StockMetadata.hpp"

namespace KanVest
{
  class UI_Momentum
  {
  public:
    static void ShowRSI(const StockData& stockData);
  };
} // namespace KanVest
