//
//  StockManager.hpp
//  KanVest
//
//  Created by Ashish . on 02/12/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  class StockManager
  {
  public:
    /// This function fetch data for stock from symbol, range and interval
    /// - Parameters:
    ///   - stockSymbolName: stock symbol name (May or May not contain .NS)
    ///   - range: Range ("1d","5d","1mo","6mo","ytd","1y","5y","max")
    ///   - interval: Interval ("1m","2m","5m","15m","30m","60m","90m","1h","1d","5d","1wk","1mo","3mo")
    [[nodiscard("Stock Data can not be discarded")]] static StockData GetStockData(const std::string& stockSymbolName, const std::string& range, const std::string& interval);
    
  private:
  };
} // namespace KanVest
