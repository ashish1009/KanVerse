//
//  MathUtils.cpp
//  KanVest
//
//  Created by Ashish . on 10/11/25.
//

#include "TechnicalUtils.hpp"

namespace KanVest
{
  // --------------------------
  // Simple Moving Average (SMA)
  // --------------------------
  double TechnicalUtils::ComputeSMA(const std::vector<StockPoint>& history, int period)
  {
    if (history.size() < period) return 0.0;
    
    double sum = 0.0;
    for (size_t i = history.size() - period; i < history.size(); ++i)
    {
      sum += history[i].close;
    }
    return sum / period;
  }
  
  // --------------------------
  // Exponential Moving Average (EMA)
  // --------------------------
  double TechnicalUtils::ComputeEMA(const std::vector<StockPoint>& history, int period)
  {
    if (history.size() < period) return 0.0;
    
    double ema = ComputeSMA(history, period); // initial EMA = SMA of first period
    double multiplier = 2.0 / (period + 1);
    
    for (size_t i = history.size() - period; i < history.size(); ++i)
    {
      ema = ((history[i].close - ema) * multiplier) + ema;
    }
    return ema;
  }
  
  // --------------------------
  // Relative Strength Index (RSI)
  // --------------------------
  double TechnicalUtils::ComputeRSI(const std::vector<StockPoint>& history, int period)
  {
    if (history.size() <= period) return 50.0; // neutral
    
    double gain = 0.0, loss = 0.0;
    for (size_t i = history.size() - period; i < history.size(); ++i)
    {
      double diff = history[i].close - history[i-1].close;
      if (diff > 0) gain += diff;
      else loss -= diff; // loss is positive
    }
    
    if (loss == 0) return 100.0;
    double rs = gain / loss;
    double rsi = 100 - (100 / (1 + rs));
    return rsi;
  }
  
  // --------------------------
  // MACD and Signal line
  // --------------------------
  std::tuple<double,double> TechnicalUtils::ComputeMACD(const std::vector<StockPoint>& history, int fastPeriod, int slowPeriod, int signalPeriod)
  {
    if (history.size() < slowPeriod) return {0.0, 0.0};
    
    double emaFast = ComputeEMA(history, fastPeriod);
    double emaSlow = ComputeEMA(history, slowPeriod);
    double macd = emaFast - emaSlow;
    
    // Signal line
    std::vector<double> macdHistory;
    for (size_t i = history.size() - signalPeriod; i < history.size(); ++i)
    {
      double fast = ComputeEMA(std::vector<StockPoint>(history.begin(), history.begin()+i+1), fastPeriod);
      double slow = ComputeEMA(std::vector<StockPoint>(history.begin(), history.begin()+i+1), slowPeriod);
      macdHistory.push_back(fast - slow);
    }
    
    double signal = std::accumulate(macdHistory.begin(), macdHistory.end(), 0.0) / macdHistory.size();
    return {macd, signal};
  }
  
  // --------------------------
  // Average True Range (ATR)
  // --------------------------
  double TechnicalUtils::ComputeATR(const std::vector<StockPoint>& history, int period)
  {
    if (history.size() < period + 1) return 0.0;
    
    std::vector<double> trValues;
    for (size_t i = history.size() - period; i < history.size(); ++i)
    {
      double highLow = history[i].high - history[i].low;
      double highClose = std::abs(history[i].high - history[i-1].close);
      double lowClose = std::abs(history[i].low - history[i-1].close);
      
      double tr = std::max({highLow, highClose, lowClose});
      trValues.push_back(tr);
    }
    
    double atr = std::accumulate(trValues.begin(), trValues.end(), 0.0) / trValues.size();
    return atr;
  }
  
  // --------------------------
  // Volume Weighted Average Price (VWAP)
  // --------------------------
  double TechnicalUtils::ComputeVWAP(const std::vector<StockPoint>& history)
  {
    double cumulativePriceVolume = 0.0;
    double cumulativeVolume = 0.0;
    
    for (const auto& point : history)
    {
      double typicalPrice = (point.high + point.low + point.close) / 3.0;
      cumulativePriceVolume += typicalPrice * point.volume;
      cumulativeVolume += point.volume;
    }
    
    if (cumulativeVolume == 0) return 0.0;
    return cumulativePriceVolume / cumulativeVolume;
  }
} // namespace KanVest
