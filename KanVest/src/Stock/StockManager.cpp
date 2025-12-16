//
//  StockManager.cpp
//  KanVest
//
//  Created by Ashish . on 02/12/25.
//

#include "StockManager.hpp"

#include "Stock/StockUtils.hpp"
#include "Stock/StockParser.hpp"
#include "Stock/StockAPI.hpp"

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

  StockData StockManager::GetLatestStockData(const std::string &symbol)
  {
    std::scoped_lock lock(s_mutex);

    // Return Empty Stock if no cache is present
    if (s_stockDataRequests.count(symbol) == 0)
    {
      static StockData EmptyStockData;
      return EmptyStockData;
    }

    // Return data from cache
    return s_stockDataRequests[symbol].cachedData;
  }

  void StockManager::WorkerLoop()
  {
    while (s_running)
    {
      auto UpdateStockData = [](std::unordered_map<std::string, StockRequest>& data) {
        std::vector<std::tuple<std::string, Range, Interval>> jobs;
        
        {
          // Copy work out quickly
          std::scoped_lock lock(s_mutex);
          for (auto& [symbol, req] : data)
          {
            jobs.emplace_back(symbol, req.range, req.interval);
          }
        }
        
        // Store futures to avoid [[nodiscard]] warning ---
        std::vector<std::future<void>> futures;
        futures.reserve(jobs.size());
        
        // Launch one async fetch per symbol
        for (auto& [symbol, range, interval] : jobs)
        {
          futures.emplace_back(std::async(std::launch::async, [symbol, range, interval, &data]()
                                          {
            StockData newData = Fetch(symbol, range, interval);
            
            // Update cached data
            {
              std::scoped_lock lock(s_mutex);
              if (auto it = data.find(symbol); it != data.end())
              {
                it->second.cachedData = newData;
              }
            }
          }));
        }
        
        // Futures go out of scope → destructor waits or is fire-and-forget
        // (std::launch::async ensures the task runs)
        std::this_thread::sleep_for(std::chrono::milliseconds(s_updateDelayMs.load()));
      };
      
      UpdateStockData(s_stockDataRequests);
    }
  }

  StockData StockManager::Fetch(const std::string& stockSymbolName, Range range, Interval interval)
  {
    static StockData EmotyData;
    // Normalize the symbol. Add required .NS to Fetch data
    std::string symbol = Utils::NormalizeSymbol(stockSymbolName);

    // Get API Keys to extract data
    APIKeys apiKeys = API_Provider::GetAPIKeys();

    // Fetch Data
    std::string response = FetchStockFallbackData(symbol, range, interval, apiKeys);
    if (response.empty())
    {
      return EmotyData;
    }

    StockData finalData(stockSymbolName);

    // --- Basic Info ---
    finalData.currency        = StockParser::StockParser::ExtractString(response, apiKeys.currency);
    finalData.exchangeName    = StockParser::StockParser::ExtractString(response, apiKeys.exchangeName);
    finalData.shortName       = StockParser::StockParser::ExtractString(response, apiKeys.shortName);
    finalData.longName        = StockParser::StockParser::ExtractString(response, apiKeys.longName);
    finalData.instrumentType  = StockParser::StockParser::ExtractString(response, apiKeys.instrumentType);
    finalData.timezone        = StockParser::StockParser::ExtractString(response, apiKeys.timezone);
    finalData.range           = StockParser::StockParser::ExtractString(response, apiKeys.range);
    finalData.dataGranularity = StockParser::StockParser::ExtractString(response, apiKeys.dataGranularity);

    // --- Price Info ---
    finalData.livePrice     = StockParser::ExtractValue(response, apiKeys.price);
    finalData.prevClose     = StockParser::ExtractValue(response, apiKeys.prevClose);

    // --- Change Info ---
    finalData.change        = finalData.livePrice - finalData.prevClose;
    finalData.changePercent = StockParser::ExtractValue(response, apiKeys.changePercent);
    if (finalData.changePercent == -1 && finalData.prevClose > 0)
    {
      finalData.changePercent = (finalData.change / finalData.prevClose) * 100.0;
    }

    // --- Volume Info ---
    finalData.volume       = StockParser::ExtractValue(response, apiKeys.volume);

    // --- High-Low Info ---
    finalData.fiftyTwoHigh = StockParser::ExtractValue(response, apiKeys.fiftyTwoHigh);
    finalData.fiftyTwoLow  = StockParser::ExtractValue(response, apiKeys.fiftyTwoLow);
    finalData.dayHigh      = StockParser::ExtractValue(response, apiKeys.dayHigh);
    finalData.dayLow       = StockParser::ExtractValue(response, apiKeys.dayLow);

    // --- Historical Candles ---
    std::vector<double> timestamps = StockParser::ExtractArray(response, "timestamp");
    std::vector<double> closes     = StockParser::ExtractArray(response, "close");
    std::vector<double> opens      = StockParser::ExtractArray(response, "open");
    std::vector<double> lows       = StockParser::ExtractArray(response, "low");
    std::vector<double> highs      = StockParser::ExtractArray(response, "high");
    std::vector<double> volumes    = StockParser::ExtractArray(response, "volume");

    size_t count = std::min({timestamps.size(), closes.size(), lows.size(), volumes.size()});
    finalData.candleHistory.reserve(count);

    for (size_t i = 0; i < count; ++i)
    {
      CandleData candle;
      candle.timestamp = timestamps[i];
      candle.open      = opens[i];
      candle.close     = closes[i];
      candle.low       = lows[i];
      candle.high      = highs[i];
      candle.volume    = volumes[i];
      candle.range     = highs[i] - lows[i];
      finalData.candleHistory.push_back(candle);
    }

    return finalData;
  }
  
  std::string StockManager::FetchStockFallbackData(const std::string& symbol, Range range, Interval interval, const APIKeys& keys)
  {
    std::string data = StockAPI::FetchLiveData(symbol, range, interval);
    
    if (data.find("\"" + keys.price + "\"") == std::string::npos and symbol.find(".NS") != std::string::npos)
    {
      std::string altSymbol = symbol.substr(0, symbol.find(".NS")) + ".BO";
      std::string altData = StockAPI::FetchLiveData(altSymbol, range, interval);
      if (altData.find("\"" + keys.price + "\"") != std::string::npos)
      {
        return altData;
      }
    }
    return data;
  }
} //  // namespace KanVest
