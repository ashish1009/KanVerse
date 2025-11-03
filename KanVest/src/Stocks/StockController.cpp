//
//  StockController.cpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#include "StockController.hpp"

#include "Stocks/StockAPI.hpp"
#include "Stocks/StockParser.hpp"

#include "Stocks/API_Provider.hpp"

namespace KanVest
{
  // ------------------------------
  // 1. SYMBOL RESOLVER
  // ------------------------------
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
  
  // ------------------------------
  // 2. DATA FETCHER
  // ------------------------------
  static std::string FetchStockData(const std::string& symbol,
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
  
  // ------------------------------
  // 3. DATA PARSER
  // ------------------------------
  static void ParseBasicInfo(StockData& stockData, const std::string& json, const APIKeys& keys)
  {
    stockData.shortName       = StockParser::StockParser::ExtractString(json, keys.shortName);
    stockData.longName        = StockParser::StockParser::ExtractString(json, keys.longName);
    stockData.instrumentType  = StockParser::StockParser::ExtractString(json, keys.instrumentType);
    stockData.timezone        = StockParser::StockParser::ExtractString(json, keys.timezone);
    stockData.range           = StockParser::StockParser::ExtractString(json, keys.range);
    stockData.dataGranularity = StockParser::StockParser::ExtractString(json, keys.dataGranularity);
    stockData.currency        = StockParser::StockParser::ExtractString(json, keys.currency);
    stockData.exchangeName    = StockParser::StockParser::ExtractString(json, keys.exchangeName);
  }
  
  static void ParseLiveMetrics(StockData& stockData, const std::string& json, const APIKeys& keys)
  {
    stockData.livePrice     = StockParser::ExtractValue(json, keys.price);
    stockData.prevClose     = StockParser::ExtractValue(json, keys.prevClose);
    stockData.change        = stockData.livePrice - stockData.prevClose;
    stockData.changePercent = StockParser::ExtractValue(json, keys.changePercent);
    
    if (stockData.changePercent == -1 && stockData.prevClose > 0)
      stockData.changePercent = (stockData.change / stockData.prevClose) * 100.0;
    
    stockData.volume       = StockParser::ExtractValue(json, keys.volume);
    stockData.fiftyTwoHigh = StockParser::ExtractValue(json, keys.fiftyTwoHigh);
    stockData.fiftyTwoLow  = StockParser::ExtractValue(json, keys.fiftyTwoLow);
    stockData.dayHigh      = StockParser::ExtractValue(json, keys.dayHigh);
    stockData.dayLow       = StockParser::ExtractValue(json, keys.dayLow);
  }
  
  static void ParseHistory(StockData& stockData, const std::string& json)
  {
    std::vector<double> timestamps = StockParser::ExtractArray(json, "timestamp");
    std::vector<double> closes     = StockParser::ExtractArray(json, "close");
    std::vector<double> opens      = StockParser::ExtractArray(json, "open");
    std::vector<double> lows       = StockParser::ExtractArray(json, "low");
    std::vector<double> highs      = StockParser::ExtractArray(json, "high");
    std::vector<double> volumes    = StockParser::ExtractArray(json, "volume");
    
    size_t count = std::min({timestamps.size(), closes.size(), lows.size(), volumes.size()});
    stockData.history.reserve(count);
    
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
      
      stockData.history.push_back(p);
    }
  }
  
  // ------------------------------
  // 4. STOCK CONTROLLER MAIN
  // ------------------------------
  StockData StockController::UpdateStockData(const std::string& symbolName, const std::string& interval, const std::string& range)
  {
    APIKeys keys = API_Provider::GetAPIKeys();
    std::string symbol = NormalizeSymbol(symbolName);
    
    std::string json = FetchStockData(symbol, interval, range, keys);
    
    StockData stockData(symbolName);
    ParseBasicInfo(stockData, json, keys);
    ParseLiveMetrics(stockData, json, keys);
    ParseHistory(stockData, json);
    
    return stockData;
  }

  void StockController::SetRefreshInterval(float refreshInterval)
  {
    s_refreshInterval = refreshInterval;
  }

  float StockController::GetRefreshInterval()
  {
    return s_refreshInterval;
  }
  
  void StockController::SetActiveStockData(const StockData &stockData)
  {
    s_activeStockData = stockData;
  }
  
  void StockController::SetCurrentInterval(const char* interval)
  {
    s_currentInterval = interval;
  }
  void StockController::SetCurrentRange(const char* range)
  {
    s_currentRange = range;
  }
  
  const char* StockController::GetCurrentInterval()
  {
    return s_currentInterval;
  }
  const char* StockController::GetCurrentRange()
  {
    return s_currentRange;
  }
  
  const StockData& StockController::GetActiveStockData()
  {
    return s_activeStockData;
  }
} // namespace KanVest
