//
//  StockManager.cpp
//  KanVest
//
//  Created by Ashish . on 03/11/25.
//

#include "StockManager.hpp"

namespace KanVest
{
  void StockManager::SetActiveStock(const StockData &stockData)
  {
    s_activeStockData = stockData;
  }
  
  void StockManager::SetCurrentInterval(const char* interval)
  {
    s_currentInterval = interval;
  }
  void StockManager::SetCurrentRange(const char* range)
  {
    s_currentRange = range;
  }
  
  const char* StockManager::GetCurrentInterval()
  {
    return s_currentInterval;
  }
  const char* StockManager::GetCurrentRange()
  {
    return s_currentRange;
  }
  
  const StockData& StockManager::GetActiveStock()
  {
    return s_activeStockData;
  }
} // namespace KanVest
