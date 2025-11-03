//
//  StockController.hpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#pragma once

#include "Stocks/StockData.hpp"
#include "Stocks/StockSummary.hpp"

namespace KanVest
{            
  /// This class manages stock data updation
  class StockController
  {
  public:
    /// This function fetches real time data for stock and returns the stock data
    /// - Parameter symbolName: symbol name
    /// - Parameter interval: intervel of data
    /// - Parameter range: range of data
    static StockData UpdateStockData(const std::string& symbolName, const std::string& interval, const std::string& range);
    
    /// This function analyze the stock
    /// - Parameter data: stock data
    static StockSummary AnalyzeStock(const StockData& data);

    static void SetRefreshInterval(float refreshInterval);
    static float GetRefreshInterval();
    
  private:
    inline static float s_refreshInterval = 1.0f;
  };
} // namespace KanVest
