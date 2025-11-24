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
  
  double GetPivotScore(const PivotResults& pivots, double livePrice, int totalCandles, double weight = 10.0)
  {
    if (totalCandles <= 0) return 0.0;
    
    const int maxLevels = 4;
    
    // --- Sort supports and resistances by strength (count + recency) ---
    auto sortByStrength = [&](const SRLevel& a, const SRLevel& b) {
      double scoreA = a.count + static_cast<double>(a.lastTouchIdx) / totalCandles;
      double scoreB = b.count + static_cast<double>(b.lastTouchIdx) / totalCandles;
      return scoreA > scoreB; // strongest first
    };
    
    std::vector<SRLevel> sortedSupports = pivots.supports;
    std::vector<SRLevel> sortedResistances = pivots.resistances;
    std::sort(sortedSupports.begin(), sortedSupports.end(), sortByStrength);
    std::sort(sortedResistances.begin(), sortedResistances.end(), sortByStrength);
    
    // --- Take top 4 levels for each ---
    int nSupports = std::min(maxLevels, (int)sortedSupports.size());
    int nResistances = std::min(maxLevels, (int)sortedResistances.size());
    
    double supportScore = 0.0;
    for (int i = 0; i < nSupports; i++)
    {
      double rel = (livePrice - sortedSupports[i].price) / livePrice; // bullish if price > support
      supportScore += std::clamp(rel, -1.0, 1.0);
    }
    if (nSupports > 0) supportScore /= nSupports; // average
    
    double resistanceScore = 0.0;
    for (int i = 0; i < nResistances; i++)
    {
      double rel = (sortedResistances[i].price - livePrice) / livePrice; // bearish if price < resistance
      resistanceScore += std::clamp(rel, -1.0, 1.0);
    }
    if (nResistances > 0) resistanceScore /= nResistances; // average
    
    // --- Combine support and resistance scores ---
    // Support positive → bullish, Resistance positive → bearish, so invert resistance
    double combined = (supportScore - resistanceScore) / 2.0;
    
    // Clamp and apply weight
    combined = std::clamp(combined, -1.0, 1.0);
    return combined * weight;
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
    recommendation.score += GetPivotScore(s_pivots, stockData.livePrice, (int32_t)stockData.history.size());

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
