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
    static bool AddStock(const std::string& symbol);

    /// Add a new stock symbol for live tracking
    static bool EditStock(const std::string& symbol);

    /// Remove a stock symbol
    static void RemoveStock(const std::string& symbol);
    
    /// Get stock data (thread-safe)
    static bool GetStock(const std::string& symbol, StockData& outData);
    
    /// Force immediate refresh of all symbols
    static void RefreshAll();
    
    /// Start background updating thread
    static void StartLiveUpdates(int intervalSeconds = 30);
    
    /// Stop background updating thread
    static void StopLiveUpdates();

    /// This returns stock cache
    static const std::unordered_map<std::string, StockData>& GetStokCache();

  private:
    static void UpdateLoop(int intervalSeconds);
    static bool UpdateStock(const std::string& symbol);

    inline static std::unordered_map<std::string, StockData> s_stockCache;
    inline static StockData s_selectedStock = {"Nifty"};
    
    inline static std::string s_currentInterval = "1d";
    inline static std::string s_currentRange = "1mo";

    inline static std::mutex s_mutex;
    inline static std::atomic<bool> s_running = false;
    inline static std::thread s_updateThread;
  };
} // namespace KanVest
