//
//  StockManager.cpp
//  KanVest
//
//  Created by Ashish . on 22/11/25.
//

#include "StockManager.hpp"

#include "URL_API/APIProvider.hpp"

#include "Stock/StockParser.hpp"
#include "Stock/StockAPI.hpp"

namespace KanVest
{
  namespace Utils
  {
    static std::string NormalizeSymbol(const std::string& input)
    {
      std::string symbol = input;
      std::transform(symbol.begin(), symbol.end(), symbol.begin(),
                     [](unsigned char c) { return std::toupper(c); });
      
      if (symbol == "%5ENSEI") return symbol;
      if (symbol == "NIFTY") return "%5ENSEI";
      if (symbol.find('.') == std::string::npos)
        symbol += ".NS";
      return symbol;
    }
    
    static std::string FetchStockFallbackData(const std::string& symbol, const std::string& interval, const std::string& range, const APIKeys& keys)
    {
      std::string data = StockAPI::FetchLiveData(symbol, interval, range);
      
      if (data.find("\"" + keys.price + "\"") == std::string::npos &&
          symbol.find(".NS") != std::string::npos)
      {
        std::string altSymbol = symbol.substr(0, symbol.find(".NS")) + ".BO";
        std::string altData = StockAPI::FetchLiveData(altSymbol, interval, range);
        if (altData.find("\"" + keys.price + "\"") != std::string::npos)
          return altData;
      }
      
      return data;
    }
    
    // Internal fetch + parse utility
    static StockData FetchAndParse(const std::string& symbol, const std::string& interval, const std::string& range)
    {
      auto apiKeys = API_Provider::GetAPIKeys();
      std::string response = FetchStockFallbackData(symbol, interval, range, apiKeys);
      if (response.empty()) return StockData();
      
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
      if (data.changePercent == -1 && data.prevClose > 0) data.changePercent = (data.change / data.prevClose) * 100.0;
      
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
      
      return data;
    }
  } // namespace Utils
  
  // Stock operations
  bool StockManager::AddStock(const std::string& symbolName)
  {
    std::scoped_lock lock(s_mutex);
    std::string symbol = Utils::NormalizeSymbol(symbolName);
    if (std::find(s_symbols.begin(), s_symbols.end(), symbol) == s_symbols.end())
    {
      s_symbols.push_back(symbol);
      
      // ensure an empty placeholder for active cache to avoid UI checks
      s_activeCache.emplace(symbol, StockData(symbol));
      return true;
    }
    return false;
  }

  bool StockManager::EditStock(const std::string& symbol)
  {
    return AddStock(symbol);
  }

  void StockManager::RemoveStock(const std::string& symbolName)
  {
    std::scoped_lock lock(s_mutex);
    std::string symbol = Utils::NormalizeSymbol(symbolName);
    s_symbols.erase(std::remove(s_symbols.begin(), s_symbols.end(), symbol), s_symbols.end());
    s_activeCache.erase(symbol);
    
    // remove all cached variants for this symbol
    for (auto it = s_stockDataCache.begin(); it != s_stockDataCache.end(); )
    {
      if (it->first.symbol == symbol) it = s_stockDataCache.erase(it); else ++it;
    }
  }
  
  bool StockManager::GetStockData(const std::string& symbolName, StockData& outData)
  {
    std::scoped_lock lock(s_mutex);
    std::string symbol = Utils::NormalizeSymbol(symbolName);
    auto it = s_activeCache.find(symbol);
    if (it != s_activeCache.end())
    {
      outData = it->second;
      return true;
    }
    return false;
  }

  // Refresh operations
  void StockManager::RefreshAllBlocking()
  {
    // For blocking refresh we create a temporary small threadpool and wait via futures
    std::vector<std::future<void>> futs;
    std::vector<std::string> symbolsCopy;
    {
      std::scoped_lock lock(s_mutex);
      symbolsCopy = s_symbols;
    }
    
    for (const auto& sym : symbolsCopy)
    {
      futs.push_back(std::async(std::launch::async, [sym]() {
        UpdateStockInternal(sym, false);
      }));
    }
    for (auto& f : futs) f.wait();
  }
  void StockManager::RefreshStockAsync(const std::string& symbolName)
  {
    std::string symbol = Utils::NormalizeSymbol(symbolName);
    if (!s_threadPool) s_threadPool = std::make_unique<ThreadPool>(4);
    s_threadPool->Enqueue([symbol]() { UpdateStockInternal(symbol, false); });
  }
  
  bool StockManager::RefreshStockBlocking(const std::string& symbolName)
  {
    std::string symbol = Utils::NormalizeSymbol(symbolName);
    auto fut = std::async(std::launch::async, [symbol]() { return UpdateStockInternal(symbol, false); });
    return fut.get();
  }
  
  // Live updates
  void StockManager::StartLiveUpdates(int intervalMilliseconds)
  {
    if (s_running)
      return;
    
    if (!s_threadPool)
      s_threadPool = std::make_unique<ThreadPool>(4);
    
    s_running = true;
    s_updateThread = std::thread(UpdateLoop, intervalMilliseconds);
  }
  
  void StockManager::StopLiveUpdates()
  {
    if (!s_running)
      return;
    
    s_running = false;
    if (s_updateThread.joinable())
      s_updateThread.join();
  }
  
  // Selection / active stock
  void StockManager::SetSelectedStockSymbol(const std::string& stockSymbol)
  {
    std::scoped_lock lock(s_mutex);
    s_selectedStockSymbol = Utils::NormalizeSymbol(stockSymbol);
    // ensure active cache entry exists
    if (s_activeCache.find(s_selectedStockSymbol) == s_activeCache.end())
      s_activeCache.emplace(s_selectedStockSymbol, StockData(s_selectedStockSymbol));
  }
  
  const std::string& StockManager::GetSelectedStockSymbol()
  {
    return s_selectedStockSymbol;
  }
  
  StockData StockManager::GetSelectedStockData()
  {
    StockData d;
    GetStockData(s_selectedStockSymbol, d);
    return d;
  }
  
  const std::unordered_map<std::string, StockData>& StockManager::GetStockCache()
  {
    std::scoped_lock lock(s_mutex);
    return s_activeCache;
  }
  
  // Interval / range
  void StockManager::SetCurrentInterval(const std::string& interval)
  {
    std::scoped_lock lock(s_mutex);
    s_currentInterval = interval;
    // after changing current interval, map activeCache from cached value if exists
    for (auto& sym : s_symbols)
    {
      CacheKey k{sym, s_currentInterval, s_currentRange};
      auto it = s_stockDataCache.find(k);
      if (it != s_stockDataCache.end()) s_activeCache[sym] = it->second;
    }
  }
  
  const std::string& StockManager::GetCurrentInterval()
  {
    return s_currentInterval;
  }
  
  void StockManager::SetCurrentRange(const std::string& range)
  {
    std::scoped_lock lock(s_mutex);
    s_currentRange = range;
    for (auto& sym : s_symbols)
    {
      CacheKey k{sym, s_currentInterval, s_currentRange};
      auto it = s_stockDataCache.find(k);
      if (it != s_stockDataCache.end()) s_activeCache[sym] = it->second;
    }
  }
  
  const std::string& StockManager::GetCurrentRange()
  {
    return s_currentRange;
  }
  
  // Private APIs
  void StockManager::UpdateLoop(int intervalMilliseconds)
  {
    using namespace std::chrono_literals;
    while (s_running)
    {
      auto start = std::chrono::steady_clock::now();
      try {
        RefreshAllBlocking(); // blocking update ensures periodic consistency
      } catch (const std::exception& e) {
        std::cerr << "[StockManager] Exception in UpdateLoop: " << e.what() << "\n";
      }
      auto elapsed = std::chrono::steady_clock::now() - start;
      auto sleepFor = std::chrono::milliseconds(intervalMilliseconds) - elapsed;
      if (sleepFor > 0ms)
        std::this_thread::sleep_for(sleepFor);
    }
  }
  
  bool StockManager::UpdateStockInternal(const std::string& symbolName, bool forceRefresh)
  {
    try
    {
      std::string symbol = Utils::NormalizeSymbol(symbolName);
      CacheKey key{symbol, s_currentInterval, s_currentRange};
      
      {
        std::scoped_lock lock(s_mutex);
        auto it = s_stockDataCache.find(key);
        if (it != s_stockDataCache.end() && it->second.IsValid() && !forceRefresh)
        {
          // already cached -- ensure active cache points to this data
          s_activeCache[symbol] = it->second;
          return true;
        }
      }
      
      // Fetch and parse outside lock
      StockData newData = Utils::FetchAndParse(symbol, key.interval, key.range);
      if (!newData.IsValid()) return false;
      
      {
        std::scoped_lock lock(s_mutex);
        s_stockDataCache[key] = newData;
        s_activeCache[symbol] = newData; // update active view
      }
      return true;
    }
    catch (const std::exception& e)
    {
      std::cerr << "[StockManager] Failed to update stock '" << symbolName << "': " << e.what() << "\n";
    }
    return false;
  }

} // namespace KanVest
