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
  
  // ============================================================================
  //  Function: AnalyzeStock
  //  Purpose : Analyze a given stock's historical data and summarize its trend,
  //            momentum, volatility, volume activity, and valuation.
  //
  //  Enhancement: VWAP (Volume Weighted Average Price) is now included to assess
  //               whether the stock is trading at a premium or discount to
  //               average traded price — a strong institutional indicator.
  //
  //  Dependencies: The StockData class must implement the following methods:
  //                - ComputeSMA(int period)
  //                - ComputeEMA(int period)
  //                - ComputeRSI(int period)
  //                - ComputeATR(int period)
  //                - ComputeVWAP()
  //                - ComputeAverageVolume(int period)
  // ============================================================================
  StockSummary StockController::AnalyzeStock()
  {
    StockSummary result;
    
    const auto& h = s_activeStockData.history;
    
    // Require at least 30 candles to make statistically meaningful calculations.
    if (h.size() < 20)
    {
      result.conclusion = "Not enough data for analysis.";
      return result;
    }
    
    // --------------------------------------------------------------------------
    // 🧩 Step 1: Extract essential computed values
    // --------------------------------------------------------------------------
    double close      = h.back().close;             // Latest closing price
    double sma20      = s_activeStockData.ComputeSMA(20);        // 20-period Simple Moving Average
    double ema20      = s_activeStockData.ComputeEMA(20);        // 20-period Exponential Moving Average
    double rsi14      = s_activeStockData.ComputeRSI(14);        // 14-period Relative Strength Index
    double atr14      = s_activeStockData.ComputeATR(14);        // 14-period Average True Range
    double vwap       = s_activeStockData.ComputeVWAP();         // Volume Weighted Average Price
    double avgVol     = s_activeStockData.ComputeAverageVolume(20); // 20-period Average Volume
    double latestVol  = h.back().volume;            // Most recent trading volume

    // --------------------------------------------------------------------------
    // 📈 Step 2: Detect the overall trend (now also compared with VWAP)
    // --------------------------------------------------------------------------
    if (close > sma20 * 1.01 && ema20 > sma20 && close > vwap)
      result.trend = "Uptrend";
    else if (close < sma20 * 0.99 && ema20 < sma20 && close < vwap)
      result.trend = "Downtrend";
    else
      result.trend = "Sideways";
    
    // --------------------------------------------------------------------------
    // ⚡ Step 3: Determine momentum strength (based on SMA/EMA divergence)
    // --------------------------------------------------------------------------
    double emaDiff = fabs(ema20 - sma20) / sma20;
    if (emaDiff > 0.02)
      result.momentum = "Strong";
    else if (emaDiff > 0.01)
      result.momentum = "Moderate";
    else
      result.momentum = "Weak";
    
    // --------------------------------------------------------------------------
    // 🌊 Step 4: Measure volatility using ATR (percentage of price range)
    // --------------------------------------------------------------------------
    double atrPercent = (atr14 / close) * 100.0;
    if (atrPercent > 3.0)
      result.volatility = "High";
    else if (atrPercent > 1.5)
      result.volatility = "Medium";
    else
      result.volatility = "Low";
    
    // --------------------------------------------------------------------------
    // 📊 Step 5: Evaluate volume activity (compared to recent average)
    // --------------------------------------------------------------------------
    double volRatio = latestVol / (avgVol + 1.0); // +1 prevents divide-by-zero
    if (volRatio > 1.5)
      result.volume = "High";
    else if (volRatio < 0.7)
      result.volume = "Low";
    else
      result.volume = "Normal";
    
    // --------------------------------------------------------------------------
    // 💰 Step 6: Valuation — combine RSI & VWAP for better signal
    // --------------------------------------------------------------------------
    if (rsi14 > 70 && close > vwap)
      result.valuation = "Overbought";
    else if (rsi14 < 30 && close < vwap)
      result.valuation = "Oversold";
    else
      result.valuation = "Fair";
    
    // --------------------------------------------------------------------------
    // 🧠 Step 7: Combine everything into a human-readable summary
    // --------------------------------------------------------------------------
    std::string base = "The stock is in a " + result.trend + " with " +
    result.momentum + " momentum, " + result.volatility +
    " volatility, and " + result.volume + " trading volume. ";
    
    // Add VWAP-based contextual message for valuation insight
    if (result.valuation == "Overbought")
    {
      base += "It is trading above VWAP, showing bullish enthusiasm but potential short-term exhaustion.";
    }
    else if (result.valuation == "Oversold")
    {
      base += "It is trading below VWAP, showing discounted levels and possible rebound opportunity.";
    }
    else
    {
      base += "Price is near VWAP, suggesting fair valuation and balanced market sentiment.";
    }
    
    result.conclusion = base;
    return result;
  }
} // namespace KanVest
