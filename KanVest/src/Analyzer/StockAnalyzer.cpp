//
//  StockAnalyzer.cpp
//  KanVest
//
//  Created by Ashish . on 23/11/25.
//

#include "StockAnalyzer.hpp"

namespace KanVest
{
  // Normalize raw score (-count to +count) to -1 to +1
  inline double NormalizeScore(int raw, int count)
  {
    return static_cast<double>(raw) / static_cast<double>(count);
  }

  // ------------------------------------------------------------
  // SMA Score  (Weight = 10)
  // ------------------------------------------------------------
  float GetSMAScore(double price, const std::map<int, double>& smaValues, double weight = 10.0)
  {
    if (smaValues.empty()) return 0.0;
    
    int rawScore = 0;
    for (const auto& kv : smaValues)
    {
      rawScore += (price > kv.second) ? 1 : -1;
    }
    
    // Normalize to -1 → +1
    double normalized = NormalizeScore(rawScore, (int32_t)smaValues.size());
    
    // Weight scaling
    return normalized * weight;
  }
  
  // ------------------------------------------------------------
  // EMA Score  (Weight = 10)
  // ------------------------------------------------------------
  float GetEMAScore(double price, const std::map<int, double>& emaValues, double weight = 10.0)
  {
    if (emaValues.empty()) return 0.0;
    
    int rawScore = 0;
    for (const auto& kv : emaValues)
    {
      rawScore += (price > kv.second) ? 1 : -1;
    }
    
    // Normalize (-1 to +1)
    double normalized = static_cast<double>(rawScore) /
    static_cast<double>(emaValues.size());
    
    return normalized * weight;
  }

  // ------------------------------------------------------------
  // EMA Score  (Weight = 10)
  // ------------------------------------------------------------
  double GetRSIScore(const RSISeries& rsiData, double weight = 10.0)
  {
    double rsi = rsiData.last;    // latest value
    
    if (std::isnan(rsi))
      return 0.0;
    
    // Normalize RSI:
    // 0  -> +1 (strong bullish)
    // 50 -> 0  (neutral)
    // 100-> -1 (strong bearish)
    double normalized = (50.0 - rsi) / 50.0;
    
    // clamp to [-1, 1]
    normalized = std::max(-1.0, std::min(1.0, normalized));
    
    return normalized * weight;
  }

  Recommendation Analyzer::AnalzeStock(const StockData &stockData)
  {
    Recommendation recommendation;
    
    // Technicals computation
    s_maResults = MovingAverage::Compute(stockData);
    s_rsiSeries = RSI::Compute(stockData);
    s_macdResult = MACD::Compute(stockData);

    // Score
    recommendation.score += GetSMAScore(stockData.livePrice, s_maResults.smaValues);
    recommendation.score += GetEMAScore(stockData.livePrice, s_maResults.emaValues);
    recommendation.score += GetRSIScore(s_rsiSeries);

    return recommendation;
  }
  
  void Analyzer::SetHoldings(const Holding &holding)
  {
    s_stockHolding = holding;
  }
  
  const std::map<int, double>& Analyzer::GetSMA()
  {
    return s_maResults.smaValues;
  }
  const std::map<int, double>& Analyzer::GetEMA()
  {
    return s_maResults.emaValues;
  }
  const RSISeries& Analyzer::GetRSI()
  {
    return s_rsiSeries;
  }
  const MACDResult& Analyzer::GetMACD()
  {
    return s_macdResult;
  }
} // namespace KanVest
