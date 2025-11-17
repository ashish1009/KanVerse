//
//  StockManager.hpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
//

#pragma once

#include "Stock/StockData.hpp"
#include "Stock/StockParser.hpp"
#include "Stock/StockAPI.hpp"

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
  
  // Cache key representing symbol+interval+range
  struct CacheKey
  {
    std::string symbol;
    std::string interval;
    std::string range;
    
    bool operator==(const CacheKey& o) const noexcept
    {
      return symbol == o.symbol && interval == o.interval && range == o.range;
    }
  };
  
  struct CacheKeyHash
  {
    size_t operator()(const CacheKey& k) const noexcept
    {
      size_t h1 = std::hash<std::string>{}(k.symbol);
      size_t h2 = std::hash<std::string>{}(k.interval);
      size_t h3 = std::hash<std::string>{}(k.range);
      return h1 ^ (h2 * 16777619u) ^ (h3 << 1);
    }
  };
}
