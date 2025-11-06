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
    static void AddStock(const std::string& symbol);
    
    /// Remove a stock symbol
    static void RemoveStock(const std::string& symbol);
    
    /// Get stock data (thread-safe)
    static bool GetStock(const std::string& symbol, StockData& outData);
    
    /// This returns stock cache
    static const std::unordered_map<std::string, StockData>& GetStokCache();

  private:
    inline static std::unordered_map<std::string, StockData> m_stockCache;
    inline static std::mutex m_mutex;
    inline static std::atomic<bool> m_running = false;
    inline static std::thread m_updateThread;
  };
} // namespace KanVest
