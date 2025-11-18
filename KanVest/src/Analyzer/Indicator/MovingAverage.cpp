//
//  MovingAverage.cpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#include "MovingAverage.hpp"

namespace KanVest
{
  // --- Utility: get applicable MA periods based on chart range ---
  static std::vector<int> GetActivePeriods(const std::string& chartRange)
  {
    if (chartRange == "1mo")
      return {5, 10, 20};
    if (chartRange == "3mo")
      return {5, 10, 20, 30, 50};
    return {5, 10, 20, 30, 50, 100, 150, 200};
  }
  
  // --- Optimized SMA computation (Angel One aligned) ---
  std::vector<double> MovingAverages::ComputeSMA(const std::vector<double>& closes, int period)
  {
    std::vector<double> sma(closes.size(), 0.0);
    if (closes.size() < static_cast<size_t>(period))
      return sma;
    
    double sum = 0.0;
    for (size_t i = 0; i < closes.size(); ++i)
    {
      sum += closes[i];
      if (i >= static_cast<size_t>(period))
        sum -= closes[i - period];
      if (i >= static_cast<size_t>(period - 1))
        sma[i] = sum / period;
    }
    return sma;
  }
  
  // --- Optimized EMA computation (Angel One aligned) ---
  std::vector<double> MovingAverages::ComputeEMA(const std::vector<double>& closes, int period)
  {
    std::vector<double> ema(closes.size(), 0.0);
    if (closes.empty())
      return ema;
    
    double multiplier = 2.0 / (period + 1.0);
    ema[0] = closes[0];  // Angel One starts EMA with first close
    
    for (size_t i = 1; i < closes.size(); ++i)
    {
      ema[i] = (closes[i] - ema[i - 1]) * multiplier + ema[i - 1];
    }
    
    return ema;
  }
  
  MAResult MovingAverages::Compute(const StockData& data, const std::string& chartRange)
  {
    MAResult result;
    if (!data.IsValid() || data.history.size() < 5)
      return result;
    
    std::vector<double> closes;
    closes.reserve(data.history.size());
    for (auto& p : data.history)
      closes.push_back(p.close);
    
    auto periods = GetActivePeriods(chartRange);
    
    for (int p : periods)
    {
      auto sma = ComputeSMA(closes, p);
      auto ema = ComputeEMA(closes, p);
      
      if (!sma.empty())
        result.smaValues[p] = sma.back();  // latest aligned value
      if (!ema.empty())
        result.emaValues[p] = ema.back();
    }
    
    return result;
  }
} // namespace KanVest
