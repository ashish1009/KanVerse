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
  /// Minimal thread-pool for controlled concurrency
  class ThreadPool
  {
  public:
    ThreadPool(size_t workerCount = std::thread::hardware_concurrency())
    : m_done(false)
    {
      if (workerCount == 0) workerCount = 2;
      for (size_t i = 0; i < workerCount; ++i)
        m_workers.emplace_back([this]() { WorkerLoop(); });
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
