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
  enum class Range
  {
    _1D, _5D, _1MO, _6MO, _YTD, _1Y, _5Y, _MAX
  };

  enum class Interval
  {
    _1M, _2M, _5M, _15M, _30M, _60M, _90M, _1H, _1D, _5D, _1WK, _1MO, _3MO
  };

  class StockManager
  {
  public:
    /// This function fetch data for stock from symbol, range and interval
    /// - Parameters:
    ///   - stockSymbolName: stock symbol name (May or May not contain .NS)
    ///   - range: Range ("1d","5d","1mo","6mo","ytd","1y","5y","max")
    ///   - interval: Interval ("1m","2m","5m","15m","30m","60m","90m","1h","1d","5d","1wk","1mo","3mo")
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
