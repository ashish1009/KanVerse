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
  static std::string NormalizeSymbol(const std::string& input)
  {
    std::string symbol = input;
    std::transform(symbol.begin(), symbol.end(), symbol.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    
    if (symbol == "NIFTY") return "%5ENSEI";
    
    if (symbol.find('.') == std::string::npos)
      symbol += ".NS";  // Default to NSE
    
    return symbol;
  }
  
  static std::string FetchStockFallbackData(const std::string& symbol,
                                            const std::string& interval,
                                            const std::string& range,
                                            const APIKeys& keys)
  {
    std::string data = StockAPI::FetchLiveData(symbol, interval, range);
    
    // Fallback: try .BO if .NS fails
    if (data.find("\"" + keys.price + "\"") == std::string::npos &&
        symbol.find(".NS") != std::string::npos)
    {
      std::string altSymbol = symbol.substr(0, symbol.find(".NS")) + ".BO";
      std::string altData = StockAPI::FetchLiveData(altSymbol, interval, range);
      
      if (altData.find("\"" + keys.price + "\"") != std::string::npos)
        return altData;  // Return alternate data
    }
    
    return data;
  }


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
    {
      return;
    }
    
    s_running = true;
    s_updateThread = std::thread(UpdateLoop, intervalSeconds);
  }
  
  void StockManager::StopLiveUpdates()
  {
    if (!s_running)
    {
      return;
    }
    
    s_running = false;
    if (s_updateThread.joinable())
    {
      s_updateThread.join();
    }
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
  
  void StockManager::UpdateStock(const std::string& symbolName)
  {
    try
    {
      auto apiKeys = API_Provider::GetAPIKeys();
      std::string symbol = NormalizeSymbol(symbolName);
      std::string response = FetchStockFallbackData(symbol, "1d", "1mo", apiKeys);
      
      if (response.empty())
      {
        return;
      }
      
      StockData data(symbol);
      data.shortName       = StockParser::StockParser::ExtractString(response, apiKeys.shortName);
      data.longName        = StockParser::StockParser::ExtractString(response, apiKeys.longName);
      data.instrumentType  = StockParser::StockParser::ExtractString(response, apiKeys.instrumentType);
      data.timezone        = StockParser::StockParser::ExtractString(response, apiKeys.timezone);
      data.range           = StockParser::StockParser::ExtractString(response, apiKeys.range);
      data.dataGranularity = StockParser::StockParser::ExtractString(response, apiKeys.dataGranularity);
      data.currency        = StockParser::StockParser::ExtractString(response, apiKeys.currency);
      data.exchangeName    = StockParser::StockParser::ExtractString(response, apiKeys.exchangeName);

      data.livePrice     = StockParser::ExtractValue(response, apiKeys.price);
      data.prevClose     = StockParser::ExtractValue(response, apiKeys.prevClose);
      data.change        = data.livePrice - data.prevClose;
      data.changePercent = StockParser::ExtractValue(response, apiKeys.changePercent);
      
      if (data.changePercent == -1 && data.prevClose > 0)
      {
        data.changePercent = (data.change / data.prevClose) * 100.0;
      }
      
      data.volume       = StockParser::ExtractValue(response, apiKeys.volume);
      data.fiftyTwoHigh = StockParser::ExtractValue(response, apiKeys.fiftyTwoHigh);
      data.fiftyTwoLow  = StockParser::ExtractValue(response, apiKeys.fiftyTwoLow);
      data.dayHigh      = StockParser::ExtractValue(response, apiKeys.dayHigh);
      data.dayLow       = StockParser::ExtractValue(response, apiKeys.dayLow);

      std::vector<double> timestamps = StockParser::ExtractArray(response, "timestamp");
      std::vector<double> closes     = StockParser::ExtractArray(response, "close");
      std::vector<double> opens      = StockParser::ExtractArray(response, "open");
      std::vector<double> lows       = StockParser::ExtractArray(response, "low");
      std::vector<double> highs      = StockParser::ExtractArray(response, "high");
      std::vector<double> volumes    = StockParser::ExtractArray(response, "volume");
      
      size_t count = std::min({timestamps.size(), closes.size(), lows.size(), volumes.size()});
      data.history.reserve(count);
      
      for (size_t i = 0; i < count; ++i)
      {
        StockPoint p;
        p.timestamp = timestamps[i];
        p.open      = opens[i];
        p.close     = closes[i];
        p.low       = lows[i];
        p.high      = highs[i];
        p.volume    = volumes[i];
        p.range     = highs[i] - lows[i];
        
        data.history.push_back(p);
      }

      
      {
        std::scoped_lock lock(s_mutex);
        s_stockCache[symbol] = std::move(data);
      }
    }
    catch (const std::exception& e)
    {
      std::cerr << "[StockManager] Failed to update stock '" << symbolName << "': " << e.what() << "\n";
    }
  }
} // namespace KanVest
