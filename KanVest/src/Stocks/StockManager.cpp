//
//  StockManager.cpp
//  KanVest
//
//  Created by Ashish . on 06/11/25.
//

#include "StockManager.hpp"

namespace KanVest
{
  void StockManager::AddStock(const std::string& symbol)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_stockCache.find(symbol) == m_stockCache.end())
    {
      m_stockCache.emplace(symbol, StockData(symbol));
    }
  }
  
  void StockManager::RemoveStock(const std::string& symbol)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_stockCache.erase(symbol);
  }
  
  bool StockManager::GetStock(const std::string& symbol, StockData& outData)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_stockCache.find(symbol);
    if (it != m_stockCache.end())
    {
      outData = it->second;
      return true;
    }
    return false;
  }
} // namespace KanVest
