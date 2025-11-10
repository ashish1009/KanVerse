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
    inline static const char* ValidIntervals[] =
    {
      "1m", "2m", "5m", "15m", "30m", "60m", "90m", "1h", "1d", "5d", "1wk", "1mo", "3mo"
    };
    
    inline static const char* ValidRange[] =
    {
      "1d", "5d", "1mo", "6mo", "ytd", "1y", "5y", "max"
    };
    
    inline static std::unordered_map<std::string, std::vector<std::string>> RangeIntervalMap =
    {
      {"1d",  {"1m", "2m", "5m", "15m", "30m", "60m"}},
      {"5d",  {"1m", "2m", "5m", "15m", "30m", "60m"}},
      {"1mo", {"5m", "15m", "30m", "60m", "1d"}},
      {"6mo", {"1h", "1d", "1wk"}},
      {"ytd", {"1h", "1wk", "1mo"}},
      {"1y",  {"1d", "1wk", "1mo"}},
      {"5y",  {"1d", "1wk", "1mo"}},
      {"max", {"1d", "1wk", "1mo"}},
    };
    
    /// This function analyzes the selected stock
//    static Analysis::AnalysisReport AnalyzeSelectedStock();

    /// Add a new stock symbol for live tracking
    static bool AddStock(const std::string& symbol);

    /// Add a new stock symbol for live tracking
    static bool EditStock(const std::string& symbol);

    /// Remove a stock symbol
    static void RemoveStock(const std::string& symbol);
    
    /// Get stock data (thread-safe)
    static bool GetShortTermStockData(const std::string& symbol, StockData& outData);
    /// Get stock data (thread-safe)
    static bool GetLongTermStockData(const std::string& symbol, StockData& outData);

    /// Force immediate refresh of all symbols
    static void RefreshAll();
    
    /// Start background updating thread
    static void StartLiveUpdates(int intervalMilliseconds = 100);
    
    /// Stop background updating thread
    static void StopLiveUpdates();

    /// This function updates the active stock data
    static void SetSelectedStockSymbol(const std::string& stockSymbol);
    /// This function updates the active stock holding data
    static void SetSelectedStockHoldingData(double atp, int qty, const std::string& stockSymbol);
    
    /// This function returns the active stock data
    static const std::string& GetSelectedStockSymbol();
    
    /// This function returns the active stock data
    static StockData GetSelectedStockData();

    /// This returns stock cache
    static const std::unordered_map<std::string, StockData>& GetStokCache();
    /// This returns stock cache
    static const std::unordered_map<std::string, StockData>& GetLongTermStokCache();

    /// This function update url time itnerval
    /// - Parameter interval:  url range
    static void SetCurrentInterval(const std::string& interval);
    /// This function returns the update refresh time
    static const std::string& GetCurrentInterval();
    
    /// This function update url range
    /// - Parameter range: new range
    static void SetCurrentRange(const std::string& range);
    /// This function returns the url range
    static const std::string& GetCurrentRange();

  private:
    static void UpdateLoop(int intervalMilliseconds);
    static bool UpdateStock(const std::string& symbol);

    inline static std::unordered_map<std::string, StockData> s_stockCache;
    inline static std::unordered_map<std::string, StockData> s_longTermStockCache;
    inline static std::string s_selectedStockSymbol = {"Nifty"};
//    inline static KanVest::Analysis::HoldingInfo s_selectedHoldingData;

    inline static std::string s_currentInterval = "60m";
    inline static std::string s_currentRange = "1mo";

    inline static std::mutex s_mutex;
    inline static std::atomic<bool> s_running = false;
    inline static std::thread s_updateThread;
  };
} // namespace KanVest
