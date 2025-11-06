//
//  StockManager.hpp
//  KanVest
//
//  Created by Ashish . on 06/11/25.
//

#pragma once

#include "Stocks/StockData.hpp"

namespace KanVest
{
  /// This class manages all stocks
  class StockManager
  {
  public:
    /// Add a new stock symbol for live tracking
    void AddStock(const std::string& symbol);
    
    /// Remove a stock symbol
    void RemoveStock(const std::string& symbol);
    
    /// Get stock data (thread-safe)
    bool GetStock(const std::string& symbol, StockData& outData);

  private:
    std::unordered_map<std::string, StockData> m_stockCache;
    std::mutex m_mutex;
    std::atomic<bool> m_running = false;
    std::thread m_updateThread;
  };
} // namespace KanVest
