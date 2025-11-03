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
    IK_PROFILE();
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
    IK_PROFILE();
    StockSummary result;
    const auto& h = s_activeStockData.history;
    
    // --- Step 0: Early exit if data too short ---
    if (h.size() < 10)
    {
      result.conclusion = "Not enough data for analysis.";
      return result;
    }
    
    // --- Step 1: Adaptive parameter tuning ---
    int smaPeriod = 20, rsiPeriod = 14, atrPeriod = 14;
    double momentumSensitivity = 0.012;
    double volHigh = 1.5, volLow = 0.7;
    
    // Decide based on interval and range
    if (s_currentInterval == "1m" && s_currentRange == "1d") {
      smaPeriod = 10; rsiPeriod = 7; atrPeriod = 7;
      momentumSensitivity = 0.02;
    }
    else if (s_currentInterval == "5m" && s_currentRange == "5d") {
      smaPeriod = 20; rsiPeriod = 14; atrPeriod = 14;
      momentumSensitivity = 0.015;
    }
    else if (s_currentInterval == "15m" && s_currentRange == "5d") {
      smaPeriod = 30; rsiPeriod = 14;
    }
    else if (s_currentInterval == "1d" && s_currentRange == "1mo") {
      smaPeriod = 20; rsiPeriod = 14;
    }
    else if (s_currentInterval == "1d" && (s_currentRange == "3mo" || s_currentRange == "6mo")) {
      smaPeriod = 50; rsiPeriod = 20; atrPeriod = 20;
    }
    else if (s_currentInterval == "1wk" || s_currentRange == "1y") {
      smaPeriod = 20; rsiPeriod = 14; atrPeriod = 14;
      volHigh = 2.0; // weekly data volume varies more
    }
    
    if (h.size() < smaPeriod)
    {
      result.conclusion = "Insufficient data for current timeframe.";
      return result;
    }
    
    // --- Step 2: Compute key indicators ---
    double close = h.back().close;
    double sma   = s_activeStockData.ComputeSMA(smaPeriod);
    double ema   = s_activeStockData.ComputeEMA(smaPeriod);
    double rsi   = s_activeStockData.ComputeRSI(rsiPeriod);
    double atr   = s_activeStockData.ComputeATR(atrPeriod);
    double vwap  = s_activeStockData.ComputeVWAP();
    double avgVol = s_activeStockData.ComputeAverageVolume(smaPeriod);
    double latestVol = h.back().volume;
    
    // --- Step 3: Adaptive thresholds based on volatility ---
    double atrPercent = (atr / close) * 100.0;
    double trendThreshold = 1.0 + (atrPercent / 200.0);
    
    // --- Step 4: Short-term direction confirmation ---
    bool recentRise = h.back().close > h[h.size() - 3].close;
    bool recentDrop = h.back().close < h[h.size() - 3].close;
    
    // --- Step 5: Determine trend (price vs SMA/EMA/VWAP) ---
    if (close > sma * trendThreshold && ema > sma && close > vwap && recentRise)
      result.trend = "Uptrend";
    else if (close < sma / trendThreshold && ema < sma && close < vwap && recentDrop)
      result.trend = "Downtrend";
    else
      result.trend = "Sideways";
    
    // --- Step 6: Momentum detection ---
    double emaDiff = fabs(ema - sma) / sma;
    if (emaDiff > momentumSensitivity * 2)
      result.momentum = "Strong";
    else if (emaDiff > momentumSensitivity)
      result.momentum = "Moderate";
    else
      result.momentum = "Weak";
    
    // --- Step 7: Volatility classification ---
    if (atrPercent > 3.0)
      result.volatility = "High";
    else if (atrPercent > 1.5)
      result.volatility = "Medium";
    else
      result.volatility = "Low";
    
    // --- Step 8: Volume classification ---
    double volRatio = latestVol / (avgVol + 1.0);
    if (volRatio > volHigh)
      result.volume = "High";
    else if (volRatio < volLow)
      result.volume = "Low";
    else
      result.volume = "Normal";
    
    // --- Step 9: Valuation (RSI + VWAP) ---
    if (rsi > 70 && close > vwap)
      result.valuation = "Overbought";
    else if (rsi < 30 && close < vwap)
      result.valuation = "Oversold";
    else
      result.valuation = "Fair";
    
    // --- Step 10: Human-readable conclusion ---
    std::string base =
    "Timeframe: " + std::string(s_currentInterval) + " / " + std::string(s_currentRange) + ". "
    "The stock is in a " + result.trend + " with " +
    result.momentum + " momentum, " + result.volatility +
    " volatility, and " + result.volume + " trading volume. ";
    
    if (result.valuation == "Overbought")
      base += "Trading above VWAP with extended momentum — caution for pullback.";
    else if (result.valuation == "Oversold")
      base += "Trading below VWAP with potential rebound setup.";
    else
      base += "Near VWAP, suggesting fair value equilibrium.";
    
    result.conclusion = base;
    return result;
  }
} // namespace KanVest
