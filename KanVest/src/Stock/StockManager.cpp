//
//  StockManager.cpp
//  KanVest
//
//  Created by Ashish . on 08/01/26.
//

#include "StockManager.hpp"

namespace KanVest
{
  void StockManager::Initialize(int milliseconds)
  {
    s_running = true;
    s_worker = std::thread(WorkerLoop);
    
    s_updateDelayMs = milliseconds;
  }
  
  void StockManager::Shutdown()
  {
    s_running = false;
    if (s_worker.joinable())
    {
      s_worker.join();
    }
  }
  
  void StockManager::AddStockDataRequest(const std::string& symbol, Range range, Interval interval)
  {
    std::scoped_lock lock(s_mutex);
    s_stockDataRequests[symbol] = { symbol, range, interval, StockData(symbol), std::chrono::steady_clock::now() };
  }

  void StockManager::WorkerLoop()
  {
    
  }
} // namespace KanVest
