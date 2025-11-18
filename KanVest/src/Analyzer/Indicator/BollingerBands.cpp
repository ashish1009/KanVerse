//
//  BollingerBands.cpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#include "BollingerBands.hpp"

namespace KanVest
{
  BBResult BollingerBands::Compute(const StockData& stock, int period, double k)
  {
    BBResult result;
    
    if (!stock.IsValid() || stock.history.size() < (size_t)period)
      return result;
    
    // Extract closes
    std::vector<double> closes;
    closes.reserve(stock.history.size());
    for (const auto& p : stock.history)
      closes.push_back(p.close);
    
    const size_t N = closes.size();
    result.middle.reserve(N - period + 1);
    result.upper.reserve(N - period + 1);
    result.lower.reserve(N - period + 1);
    result.deviation.reserve(N - period + 1);
    
    // Sliding SMA + StdDev
    for (size_t i = period - 1; i < N; i++)
    {
      double sum = 0.0;
      for (int j = 0; j < period; j++)
        sum += closes[i - j];
      
      double sma = sum / period;
      
      double devSum = 0.0;
      for (int j = 0; j < period; j++)
      {
        double diff = closes[i - j] - sma;
        devSum += diff * diff;
      }
      
      double stddev = std::sqrt(devSum / period);
      
      result.middle.push_back(sma);
      result.upper.push_back(sma + k * stddev);
      result.lower.push_back(sma - k * stddev);
      result.deviation.push_back(stddev);
    }
    
    if (!result.middle.empty())
    {
      result.lastMiddle = result.middle.back();
      result.lastUpper  = result.upper.back();
      result.lastLower  = result.lower.back();
      result.valid = true;
    }
    
    return result;
  }
} // namespace KanVest
