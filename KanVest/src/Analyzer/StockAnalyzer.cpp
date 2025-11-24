//
//  StockAnalyzer.cpp
//  KanVest
//
//  Created by Ashish . on 23/11/25.
//

#include "StockAnalyzer.hpp"

#include "Analyzer/Indicator/MovingAverage.hpp"

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

  Recommendation Analyzer::AnalzeStock(const StockData &stockData)
  {
    Recommendation recommendation;
    
    // Technicals computation
    MAResult maResults = MovingAverage::Compute(stockData);
    
    // Score
    recommendation.score += GetSMAScore(stockData.livePrice, maResults.smaValues);
    recommendation.score += GetEMAScore(stockData.livePrice, maResults.emaValues);
    
    return recommendation;
  }
  
  void Analyzer::SetHoldings(const Holding &holding)
  {
    s_stockHolding = holding;
  }
} // namespace KanVest
