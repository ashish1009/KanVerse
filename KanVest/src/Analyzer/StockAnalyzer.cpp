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
  
  double GetMACDScore(const MACDResult& macd, double weight = 10.0)
  {
    const auto& macdSeries   = macd.macdLine;
    const auto& signalSeries = macd.signalLine;
    const auto& histSeries   = macd.histogram;
    
    size_t n = macdSeries.size();
    if (n < 2) return 0.0;
    
    double macdNow  = macdSeries[n - 1];
    double macdPrev = macdSeries[n - 2];
    
    double sigNow   = signalSeries[n - 1];
    
    double histNow  = histSeries[n - 1];
    double histPrev = histSeries[n - 2];
    
    // --- individual normalized parts (-1 or +1) ---
    double crossover = (macdNow > sigNow) ? +1.0 : -1.0;
    double momentum  = (histNow > histPrev) ? +1.0 : -1.0;
    double slope     = (macdNow > macdPrev) ? +1.0 : -1.0;
    
    // --- Normalize to range -1 to +1 ---
    double raw = (crossover + momentum + slope) / 3.0;
    
    // --- Apply weight ---
    return raw * weight;
  }
  
  Recommendation Analyzer::AnalzeStock(const StockData &stockData)
  {
    Recommendation recommendation;
    
    // Technicals computation
    s_maResults = MovingAverage::Compute(stockData);
    s_rsiSeries = RSI::Compute(stockData);
    s_macdResult = MACD::Compute(stockData);

    // chart
    s_pivots = Pivot::Compute(stockData);

    // Score
    recommendation.score += GetSMAScore(stockData.livePrice, s_maResults.smaValues);
    recommendation.score += GetEMAScore(stockData.livePrice, s_maResults.emaValues);
    recommendation.score += GetRSIScore(s_rsiSeries);
    recommendation.score += GetMACDScore(s_macdResult);
    
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
  
  const PivotResults& Analyzer::GetPivots()
  {
    return s_pivots;
  }
} // namespace KanVest
