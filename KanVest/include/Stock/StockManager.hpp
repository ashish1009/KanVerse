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
  struct StockRequest
  {
    std::string symbol;
    Range range;
    Interval interval;
    
    StockData cachedData;
    std::chrono::steady_clock::time_point lastUpdated;
  };

  class StockManager
  {
  public:
    /// This function intializes the stock manager data
    static void Initialize(int milliseconds = 10);
    /// This function shuts down the stock manager data
    static void Shutdown();
    
    /// This function adds the request for stock
    /// - Parameters:
    ///   - symbol: stock symbpl
    ///   - range: range of stock fetch
    ///   - interval: interval of stock fetch
    static void AddRequest(const std::string& symbol, Range range, Interval interval);
    /// This function returns the stock data for symbol
    /// - Parameters:
    ///   - symbol: stock symbpl
    [[nodiscard("Stock Data can not be discarded")]] static StockData GetLatest(const std::string& symbol);

  private:
    /// This is worker loop
    static void WorkerLoop();
    /// This function fetch data for stock as fallback, search for .BS if not available in .NS
    /// This function fetch data for stock as fallback, search for .BS if not available in .NS
    [[nodiscard("Stock Data can not be discarded")]] static StockData Fetch(const std::string& symbol, Range range, Interval interval);
    /// This function fetch data from URL
    /// - Parameters:
    ///   - symbol: stock symbol
    ///   - range: range
    ///   - interval: interval
    ///   - keys: keys
    static std::string FetchStockFallbackData(const std::string& symbol, Range range, Interval interval, const APIKeys& keys);
    
    inline static std::unordered_map<std::string, StockRequest> s_requests;
    inline static std::mutex s_mutex;
    inline static std::atomic<bool> s_running = false;
    inline static std::thread s_worker;
    inline static std::atomic<int> s_updateDelayMs = 10; // default 10ms
  };
} // namespace KanVest
