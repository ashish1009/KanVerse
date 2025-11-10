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
  namespace Utils
  {
    struct IntervalMapping
    {
      std::string higherInterval;
      std::string higherRange;
    };
    
    IntervalMapping GetHybridMapping(const std::string& interval)
    {
      if (interval == "1m" || interval == "5m" || interval == "15m")
        return {"1d", "1mo"};
      if (interval == "30m" || interval == "60m" || interval == "90m")
        return {"1d", "3mo"};
      if (interval == "1d")
        return {"1wk", "6mo"};
      if (interval == "1wk")
        return {"1mo", "1y"};
      
      // For 1mo or higher → no hybrid
      return {"", ""};
    }
  } // namespace Utils

  static std::string NormalizeSymbol(const std::string& input)
  {
    std::string symbol = input;
    std::transform(symbol.begin(), symbol.end(), symbol.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    
    // If Nifty symbol return same
    if (symbol == "%5ENSEI")
    {
      return symbol;
    }
    
    // Convert nifty to symbol
    if (symbol == "NIFTY")
    {
      return "%5ENSEI";
    }
    
    // Add NS for NSE
    if (symbol.find('.') == std::string::npos)
    {
      symbol += ".NS";  // Default to NSE
    }
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

//  Analysis::AnalysisReport StockManager::AnalyzeSelectedStock()
//  {
//    // Get long term and short term data
//    KanVest::StockData shortTerm;
//    KanVest::StockData longTerm;
//    KanVest::StockManager::GetShortTermStockData(s_selectedStockSymbol, shortTerm);
//    KanVest::StockManager::GetLongTermStockData(s_selectedStockSymbol, longTerm);
//    
//    if (shortTerm.IsValid() or longTerm.IsValid())
//    {
//      // Constant config for now
//      KanVest::Analysis::AnalyzerConfig cfg;
//      cfg.sma_short = 9;
//      cfg.sma_long = 21;
//      cfg.rsi_period = 14;
//      
//      KanVest::Analysis::StockAnalyzer analyzer(cfg);
//      
//      return analyzer.Analyze(shortTerm, &longTerm, &s_selectedHoldingData);
//    }
//    return Analysis::AnalysisReport();
//  }

  bool StockManager::AddStock(const std::string& symbolName)
  {
    std::scoped_lock lock(s_mutex);
    std::string symbol = NormalizeSymbol(symbolName);
    if (s_stockCache.find(symbol) == s_stockCache.end())
    {
      s_stockCache.emplace(symbol, StockData(symbol));
      s_longTermStockCache.emplace(symbol, StockData(symbol));
      return true;
    }
    return false;
  }

  bool StockManager::EditStock(const std::string& symbolName)
  {
    AddStock(symbolName);
    return true;
  }

  void StockManager::RemoveStock(const std::string& symbol)
  {
    std::scoped_lock lock(s_mutex);
    s_stockCache.erase(symbol);
    s_longTermStockCache.erase(symbol);
  }
  
  bool StockManager::GetShortTermStockData(const std::string& symbolName, StockData& outData)
  {
    std::scoped_lock lock(s_mutex);
    std::string symbol = NormalizeSymbol(symbolName);
    
    auto it = s_stockCache.find(symbol);
    if (it != s_stockCache.end())
    {
      outData = it->second;
      return true;
    }
    
    return false;
  }

  bool StockManager::GetLongTermStockData(const std::string& symbolName, StockData& outData)
  {
    std::scoped_lock lock(s_mutex);
    std::string symbol = NormalizeSymbol(symbolName);
    
    auto it = s_longTermStockCache.find(symbol);
    if (it != s_longTermStockCache.end())
    {
      outData = it->second;
      return true;
    }
    
    return false;
  }

  void StockManager::RefreshAll()
  {
    auto refresh = [](const std::unordered_map<std::string, StockData>& snapshot) {      
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
    };
    
    refresh(s_stockCache);
    refresh(s_longTermStockCache);
  }
  
  void StockManager::StartLiveUpdates(int intervalMilliseconds)
  {
    if (s_running)
    {
      return;
    }
    
    s_running = true;
    s_updateThread = std::thread(UpdateLoop, intervalMilliseconds);
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

  void StockManager::SetSelectedStockSymbol(const std::string& stockSymbol)
  {
    s_selectedStockSymbol = stockSymbol;
  }
  
  void StockManager::SetSelectedStockHoldingData(double atp, int qty, const std::string& stockSymbol)
  {
//    s_selectedHoldingData = KanVest::Analysis::HoldingInfo(atp, qty, stockSymbol);
  }
  
  const std::string& StockManager::GetSelectedStockSymbol()
  {
    return s_selectedStockSymbol;
  }
  
  StockData StockManager::GetSelectedStockData()
  {
    StockData selectedStockData;
    GetShortTermStockData(s_selectedStockSymbol, selectedStockData);
    return selectedStockData;
  }

  const std::unordered_map<std::string, StockData>& StockManager::GetStokCache()
  {
    std::scoped_lock lock(s_mutex);
    return s_stockCache;
  }
  const std::unordered_map<std::string, StockData>& StockManager::GetLongTermStokCache()
  {
    std::scoped_lock lock(s_mutex);
    return s_longTermStockCache;
  }

  void StockManager::SetCurrentInterval(const std::string& interval)
  {
    s_currentInterval = interval;
  }
  void StockManager::SetCurrentRange(const std::string& range)
  {
    s_currentRange = range;
  }
  
  const std::string& StockManager::GetCurrentInterval()
  {
    return s_currentInterval;
  }
  const std::string& StockManager::GetCurrentRange()
  {
    return s_currentRange;
  }

  void StockManager::UpdateLoop(int intervalMilliseconds)
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
      auto sleepFor = std::chrono::milliseconds(intervalMilliseconds) - elapsed;

      if (sleepFor > 0ms)
        std::this_thread::sleep_for(sleepFor);
    }
  }
  
  bool StockManager::UpdateStock(const std::string& symbolName)
  {
    try
    {
      auto FetchData = [](const std::string& symbol, const std::string& interval, const std::string& range) -> StockData {
        auto apiKeys = API_Provider::GetAPIKeys();
        std::string response = FetchStockFallbackData(symbol, interval, range, apiKeys);
        
        if (response.empty())
        {
          return StockData();
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
        return data;
      };
      
      std::string symbol = NormalizeSymbol(symbolName);
      StockData shortTermData = FetchData(symbol, s_currentInterval, s_currentRange);
       
      if (shortTermData.IsValid())
      {
        {
          std::scoped_lock lock(s_mutex);
          s_stockCache[symbol] = std::move(shortTermData);
        }
        
        auto map = Utils::GetHybridMapping(s_currentInterval);
        
        if (!map.higherInterval.empty())
        {
          StockData hierData = FetchData(symbol, map.higherInterval.c_str(), map.higherRange.c_str());
          if (hierData.IsValid())
          {
            std::scoped_lock lock(s_mutex);
            s_longTermStockCache[symbol] = std::move(hierData);
          }
        }
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
