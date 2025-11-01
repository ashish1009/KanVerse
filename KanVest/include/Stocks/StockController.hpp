//
//  StockController.hpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#pragma once

#include "Stocks/StockData.hpp"

namespace KanVest
{            
  /// This class manages stock data updation
  class StockController
  {
  public:
    /// This function fetches real time data for stock and returns the stock data
    /// - Parameter symbol: symbolName
    static StockData UpdateStockData(const std::string& symbolName, const std::string& period1, const std::string& period2, const std::string& interval, const std::string& range);
  };
} // namespace KanVest
