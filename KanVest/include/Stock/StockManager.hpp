//
//  StockManager.hpp
//  KanVest
//
//  Created by Ashish . on 08/01/26.
//

#pragma once

#include "Stock/StockMetadata.hpp"

#include "URL_API/API_Provider.hpp"

namespace KanVest
{
  /// This structure stores the stock symbol to request data from URL
  struct StockRequest
  {
    std::string symbol;
    Range range;
    Interval interval;
    
    StockData cachedData;
    std::chrono::steady_clock::time_point lastUpdated;
  };

  /// This class managers stocks data
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
    static void AddStockDataRequest(const std::string& symbol, Range range, Interval interval);

  private:
    /// This is worker loop
    static void WorkerLoop();

    inline static std::unordered_map<std::string, StockRequest> s_stockDataRequests;

    inline static std::mutex s_mutex;
    inline static std::atomic<bool> s_running = false;
    inline static std::thread s_worker;
    inline static std::atomic<int> s_updateDelayMs = 10;
  };
} // namespace KanVest
