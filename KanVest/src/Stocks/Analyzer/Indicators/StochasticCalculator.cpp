//
//  StochasticCalculator.cpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
//

#include "StochasticCalculator.hpp"

#include "MovingAverage.hpp"

namespace KanVest
{
  StochasticResult Stochastic::Compute(const StockData& data, int period)
  {
    StochasticResult result;
    
    if (!data.IsValid() || data.history.size() < (size_t)period)
      return result;
    
    const auto& hist = data.history;
    
    size_t n = hist.size();
    result.kSeries.reserve(n);
    
    // --- Compute raw %K ---
    for (size_t i = period - 1; i < n; i++)
    {
      double highestHigh = -1e9;
      double lowestLow = 1e9;
      
      for (size_t j = i - period + 1; j <= i; j++)
      {
        highestHigh = std::max(highestHigh, hist[j].high);
        lowestLow = std::min(lowestLow, hist[j].low);
      }
      
      double denom = (highestHigh - lowestLow);
      
      double k = 0.0;
      if (denom != 0)
        k = ((hist[i].close - lowestLow) / denom) * 100.0;
      
      result.kSeries.push_back(k);
    }
    
    // --- %D = SMA(3) of %K ---
    result.dSeries = MovingAverages::ComputeSMA(result.kSeries, 3);
    
    if (!result.kSeries.empty())
      result.lastK = result.kSeries.back();
    
    if (!result.dSeries.empty())
      result.lastD = result.dSeries.back();
    
    return result;
  }
}
