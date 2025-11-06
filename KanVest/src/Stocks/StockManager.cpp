//
//  StockManager.cpp
//  KanVest
//
//  Created by Ashish . on 06/11/25.
//

#include "StockManager.hpp"

#include "Stocks/StockParser.hpp"
#include "Stocks/StockAPI.hpp"

#include "URL_API/APIProvider.hpp"

namespace KanVest
{
  void StockManager::AddStock(const std::string& symbol)
  {
    std::scoped_lock lock(s_mutex);
    if (s_stockCache.find(symbol) == s_stockCache.end())
    {
      s_stockCache.emplace(symbol, StockData(symbol));
      std::cout << "[StockManager] Added stock: " << symbol << std::endl;
    }
  }
  
  void StockManager::RemoveStock(const std::string& symbol)
  {
    std::scoped_lock lock(s_mutex);
    s_stockCache.erase(symbol);
    std::cout << "[StockManager] Removed stock: " << symbol << std::endl;
  }
  
  bool StockManager::GetStock(const std::string& symbol, StockData& outData)
  {
    std::scoped_lock lock(s_mutex);
    auto it = s_stockCache.find(symbol);
    if (it != s_stockCache.end())
    {
      outData = it->second;
      return true;
    }
    return false;
  }
  
  void StockManager::RefreshAll()
  {
    std::unordered_map<std::string, StockData> snapshot;
    {
      std::scoped_lock lock(s_mutex);
      snapshot = s_stockCache; // Copy current symbols
    }
    
    // Run parallel async updates
    std::vector<std::future<void>> futures;
    futures.reserve(snapshot.size());
    
    for (auto& [symbol, _] : snapshot)
    {
      futures.push_back(std::async(std::launch::async, [symbol]() {
        UpdateStock(symbol);
      }));
    }
    
    for (auto& f : futures)
      f.wait();
  }
  
  void StockManager::StartLiveUpdates(int intervalSeconds)
  {
    if (s_running)
      return;
    
    s_running = true;
    s_updateThread = std::thread(UpdateLoop, intervalSeconds);
    std::cout << "[StockManager] Live updates started (interval: " << intervalSeconds << "s)\n";
  }
  
  void StockManager::StopLiveUpdates()
  {
    if (!s_running)
      return;
    
    s_running = false;
    if (s_updateThread.joinable())
      s_updateThread.join();
    
    std::cout << "[StockManager] Live updates stopped\n";
  }
  
  const std::unordered_map<std::string, StockData>& StockManager::GetStokCache()
  {
    return s_stockCache;
  }
  
  // ----------------------------
  // Private Implementation
  // ----------------------------
  
  void StockManager::UpdateLoop(int intervalSeconds)
  {
    using namespace std::chrono_literals;
    
    while (s_running)
    {
      auto start = std::chrono::steady_clock::now();
      
      try
      {
        RefreshAll();
      }
      catch (const std::exception& e)
      {
        std::cerr << "[StockManager] Exception in UpdateLoop: " << e.what() << "\n";
      }
      
      // Sleep until next interval
      auto elapsed = std::chrono::steady_clock::now() - start;
      auto sleepFor = std::chrono::seconds(intervalSeconds) - elapsed;
      
      if (sleepFor > 0s)
        std::this_thread::sleep_for(sleepFor);
    }
  }
  
  void StockManager::UpdateStock(const std::string& symbol)
  {
    try
    {
      auto apiKeys = API_Provider::GetAPIKeys();
      std::string response = StockAPI::FetchLiveData(symbol, "1d", "1mo");
      if (response.empty())
        return;
      
      StockData data(symbol);
      data.livePrice     = StockParser::ExtractValue(response, apiKeys.price);
      data.prevClose     = StockParser::ExtractValue(response, apiKeys.prevClose);
      data.changePercent = StockParser::ExtractValue(response, apiKeys.changePercent);
      data.volume        = StockParser::ExtractValue(response, apiKeys.volume);
      data.currency      = StockParser::ExtractString(response, apiKeys.currency);
      data.exchangeName  = StockParser::ExtractString(response, apiKeys.exchangeName);
      data.shortName     = StockParser::ExtractString(response, apiKeys.shortName);
      data.longName      = StockParser::ExtractString(response, apiKeys.longName);
      data.fiftyTwoHigh  = StockParser::ExtractValue(response, apiKeys.fiftyTwoHigh);
      data.fiftyTwoLow   = StockParser::ExtractValue(response, apiKeys.fiftyTwoLow);
      data.dayHigh       = StockParser::ExtractValue(response, apiKeys.dayHigh);
      data.dayLow        = StockParser::ExtractValue(response, apiKeys.dayLow);
      
      {
        std::scoped_lock lock(s_mutex);
        s_stockCache[symbol] = std::move(data);
      }
    }
    catch (const std::exception& e)
    {
      std::cerr << "[StockManager] Failed to update stock '" << symbol << "': " << e.what() << "\n";
    }
  }
} // namespace KanVest
