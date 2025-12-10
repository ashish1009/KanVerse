//
//  UI_Chart.hpp
//  KanVest
//
//  Created by Ashish . on 09/12/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  class Chart
  {
  public:
    /// This function shows the stock chart
    /// - Parameter stockData: stockData
    static void Show(const StockData& stockData);
  };
} // namespace KanVest
