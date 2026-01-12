//
//  UI_Chart.hpp
//  KanVest
//
//  Created by Ashish . on 12/01/26.
//

#pragma once

#include "Stock/StockManager.hpp"

namespace KanVest
{
  class Chart
  {
  public:
    /// This function shows the stock chart
    /// - Parameter stockData: stockData
    static void Show(const StockData& stockData);
    
  private:
    static void ShowController(const StockData& stockData);
  };
} // namespace KanVest
