//
//  StockSummary.cpp
//  KanVest
//
//  Created by Ashish . on 03/11/25.
//

#include "StockSummary.hpp"

#include "Stocks/StockData.hpp"

namespace KanVest
{
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
  StockSummary AnalyzeStock(StockData& data)
  {
    StockSummary result;
    
    const auto& h = data.history;
    
    // Require at least 30 candles to make statistically meaningful calculations.
    if (h.size() < 30)
    {
      result.conclusion = "Not enough data for analysis.";
      return result;
    }
    
    // --------------------------------------------------------------------------
    // 🧩 Step 1: Extract essential computed values
    // --------------------------------------------------------------------------
    double close      = h.back().close;             // Latest closing price
    double sma20      = data.ComputeSMA(20);        // 20-period Simple Moving Average
    double ema20      = data.ComputeEMA(20);        // 20-period Exponential Moving Average
    double rsi14      = data.ComputeRSI(14);        // 14-period Relative Strength Index
    double atr14      = data.ComputeATR(14);        // 14-period Average True Range
    double vwap       = data.ComputeVWAP();         // Volume Weighted Average Price
    double avgVol     = data.ComputeAverageVolume(20); // 20-period Average Volume
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
}
