//
//  Momentum.cpp
//  KanVest
//
//  Created by Ashish . on 15/01/26.
//

#include "Momentum.hpp"

namespace KanVest
{
  // Helper: safe NaN
  static constexpr double NaN() { return std::numeric_limits<double>::quiet_NaN(); }
  
  RSISeries RSI::Compute(const StockData& data, size_t period)
  {
    RSISeries out;

    // Get candle history data
    const auto& history = data.candleHistory;
    const size_t n = history.size();

    // Validate data
    if (!data.IsValid() or n < 2)
    {
      out.last = NaN();
      return out;
    }
    
    // Reserve rsi data size and assign all as NAN
    out.series.assign(n, NaN());
    
    // Validate if chart have enough candle as period If we do not have period size candles
    if (n <= period)
    {
      out.last = NaN();
      return out;
    }
    
    // --- Step 1: initial avg gain/loss for first `period` candles ---
    double gainSum = 0.0;
    double lossSum = 0.0;
    
    for (size_t i = 1; i <= period; ++i)
    {
      double diff = history[i].close - history[i - 1].close;
      if (diff > 0)
        gainSum += diff;
      else
        lossSum += -diff;
    }
    
    double avgGain = gainSum / period;
    double avgLoss = lossSum / period;
    
    // First RSI at index = period
    double rsi = NaN();
    if (avgLoss == 0.0)
      rsi = 100.0;
    else
      rsi = 100.0 - (100.0 / (1.0 + (avgGain / avgLoss)));
    
    out.series[period] = rsi;
    
    // --- Step 2: Wilder smoothing for remaining candles ---
    for (size_t i = period + 1; i < n; ++i)
    {
      double diff = history[i].close - history[i - 1].close;
      double gain = diff > 0 ? diff : 0.0;
      double loss = diff < 0 ? -diff : 0.0;
      
      avgGain = (avgGain * (period - 1) + gain) / period;
      avgLoss = (avgLoss * (period - 1) + loss) / period;
      
      if (avgLoss == 0.0)
        rsi = 100.0;
      else if (avgGain == 0.0)
        rsi = 0.0;
      else
        rsi = 100.0 - (100.0 / (1.0 + (avgGain / avgLoss)));
      
      out.series[i] = rsi;
    }
    
    out.last = out.series[n - 1];
    
    return out;
  }
} // namespace KanVest
