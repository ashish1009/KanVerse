//
//  StockManager.hpp
//  KanVest
//
//  Created by Ashish . on 06/11/25.
//

#pragma once

#include "Stocks/StockData.hpp"
#include "Stocks/StockParser.hpp"
#include "Stocks/StockAPI.hpp"
#include "URL_API/APIProvider.hpp"

namespace KanVest
{
  /// Minimal thread-pool for controlled concurrency
  class ThreadPool
  {
  public:
    ThreadPool(size_t workerCount = std::thread::hardware_concurrency())
    : m_done(false)
    {
      if (workerCount == 0) workerCount = 2;
      for (size_t i = 0; i < workerCount; ++i)
      {
        m_workers.emplace_back([this]() { WorkerLoop(); });
      }
    }
    
    ~ThreadPool()
    {
      {
        std::scoped_lock lock(m_mutex);
        m_done = true;
      }
      m_cv.notify_all();
      for (auto& t : m_workers)
        if (t.joinable()) t.join();
    }
    
    template<typename Fn>
    void Enqueue(Fn&& fn)
    {
      {
        std::scoped_lock lock(m_mutex);
        m_queue.emplace(std::function<void()>(std::forward<Fn>(fn)));
      }
      m_cv.notify_one();
    }
    
  private:
    void WorkerLoop()
    {
      while (true)
      {
        std::function<void()> task;
        {
          std::unique_lock lock(m_mutex);
          m_cv.wait(lock, [this]() { return m_done || !m_queue.empty(); });
          if (m_done && m_queue.empty()) return;
          task = std::move(m_queue.front());
          m_queue.pop();
        }
        try { task(); } catch (const std::exception& e) { std::cerr << "[ThreadPool] task exception: " << e.what() << "\n"; }
      }
    }
    
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_done;
  };
  
  // Cache key representing symbol+interval+range
  struct CacheKey
  {
    std::string symbol;
    std::string interval;
    std::string range;
    
    bool operator==(const CacheKey& o) const noexcept {
      return symbol == o.symbol && interval == o.interval && range == o.range;
    }
  };
  
  struct CacheKeyHash
  {
    size_t operator()(const CacheKey& k) const noexcept {
      // simple but effective hashing
      size_t h1 = std::hash<std::string>{}(k.symbol);
      size_t h2 = std::hash<std::string>{}(k.interval);
      size_t h3 = std::hash<std::string>{}(k.range);
      return h1 ^ (h2 * 16777619u) ^ (h3 << 1);
    }
  };
  
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
      {"1d",  {"1m", "5m", "15m", "30m", "60m"}},
      {"5d",  {"1m", "5m", "15m", "30m", "60m"}},
      {"1mo", {"5m", "15m", "30m", "60m", "1d"}},
      {"6mo", {"1h", "1d", "1wk"}},
      {"ytd", {"1h", "1wk", "1mo"}},
      {"1y",  {"1d", "1wk", "1mo"}},
      {"5y",  {"1d", "1wk", "1mo"}},
      {"max", {"1d", "1wk", "1mo"}},
    };
    
    /// Add a new stock symbol for live tracking
    static bool AddStock(const std::string& symbol);
    
    /// Edit (alias for Add for now)
    static bool EditStock(const std::string& symbol);
    
    /// Remove a stock symbol
    static void RemoveStock(const std::string& symbol);
    
    /// Get stock data (thread-safe) for current interval/range
    static bool GetStockData(const std::string& symbol, StockData& outData);
    
    /// Force immediate refresh of all symbols (non-blocking: returns immediately)
    static void RefreshAll();
    
    /// Force immediate refresh of all symbols and wait until finished
    static void RefreshAllBlocking();
    
    /// Start background updating thread
    static void StartLiveUpdates(int intervalMilliseconds = 1000);
    
    /// Stop background updating thread
    static void StopLiveUpdates();
    
    /// This function updates the active stock symbol
    static void SetSelectedStockSymbol(const std::string& stockSymbol);
    
    /// This function returns the active stock symbol
    static const std::string& GetSelectedStockSymbol();
    
    /// This function returns the active stock data for the current interval/range
    static StockData GetSelectedStockData();
    
    /// This returns stock cache for the currently selected interval/range (symbol -> StockData)
    static const std::unordered_map<std::string, StockData>& GetStockCache();
    
    /// Set/get current interval
    static void SetCurrentInterval(const std::string& interval);
    static const std::string& GetCurrentInterval();
    
    /// Set/get current range
    static void SetCurrentRange(const std::string& range);
    static const std::string& GetCurrentRange();
    
  private:
    static void UpdateLoop(int intervalMilliseconds);
    static bool UpdateStockInternal(const std::string& symbol, bool forceRefresh = false);
    
    // thread-safe caches
    inline static std::unordered_map<CacheKey, StockData, CacheKeyHash> s_cache;
    
    // convenience map for fast UI reads: symbol -> StockData for current interval/range
    inline static std::unordered_map<std::string, StockData> s_activeCache;
    
    inline static std::vector<std::string> s_symbols; // list of normalized symbols being tracked
    inline static std::string s_selectedStockSymbol;
    
    inline static std::string s_currentInterval = "60m";
    inline static std::string s_currentRange = "1mo";
    
    inline static std::mutex s_mutex;
    inline static std::atomic<bool> s_running = false;
    inline static std::thread s_updateThread;
    
    // thread pool used for refresh calls (controls concurrency)
    inline static std::unique_ptr<ThreadPool> s_threadPool;
  };
} // nameapce KanVest
