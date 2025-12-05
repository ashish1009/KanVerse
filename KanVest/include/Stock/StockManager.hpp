//
//  StockManager.hpp
//  KanVest
//
//  Created by Ashish . on 02/12/25.
//

#pragma once

#include "Stock/StockData.hpp"

#include "URL_API/APIProvider.hpp"

namespace KanVest
{
  class StockManager
  {
  public:
    /// This function fetch data for stock from symbol, range and interval
    /// - Parameters:
    ///   - stockSymbolName: stock symbol name (May or May not contain .NS)
    ///   - range: Range for URL
    ///   - interval: Interval for URL
    [[nodiscard("Stock Data can not be discarded")]] static StockData GetStockData(const std::string& stockSymbolName, Range range, Interval interval);
    
  private:
    /// This function fetch data for stock as fallback, search for .BS if not available in .NS
    /// - Parameters:
    ///   - symbol: Stock symbol name
    ///   - range: Range for URL
    ///   - interval: Interval for URL
    ///   - keys: API Keys
    [[nodiscard("Stock Data can not be discarded")]] static std::string FetchStockFallbackData(const std::string& symbol, Range range, Interval interval, const APIKeys& keys);
  };
} // namespace KanVest
