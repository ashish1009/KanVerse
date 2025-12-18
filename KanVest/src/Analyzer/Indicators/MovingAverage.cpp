//
//  MovingAverage.cpp
//  KanVest
//
//  Created by Ashish . on 15/12/25.
//

#include "MovingAverage.hpp"

#include "Analyzer/Indicators/IndicatorUtils.hpp"

namespace KanVest
{
  static std::vector<int> GetActivePeriods(const std::string& chartRange)
  {
    if (chartRange == "1mo") return {5, 10, 20};
    if (chartRange == "3mo") return {5, 10, 20, 30, 50};
    return {5, 10, 20, 30, 50, 100, 150, 200, 300};
  }
  
  MAResult MovingAverage::Compute(const StockData& data)
  {
    MAResult result;
    
    if (!data.IsValid())
    {
      return result;
    }
    
    // convert ANY raw history (1W,1D,1h..)-> daily
#if UseDailyCandle
    auto closesCandle = Indicator::Utils::BuildDailyCloses(data);
#else
    auto closesCandle = Indicator::Utils::GetCandleCloses(data);
#endif
    if (closesCandle.size() < 5)
    {
      return result;
    }
    
    auto periods = GetActivePeriods(data.range);
    
    for (int p : periods)
    {
      auto dma = ComputeDMA(closesCandle, p);
      auto ema = ComputeEMA(closesCandle, p);
      
      result.dmaValues[p] = dma;
      result.emaValues[p] = ema;
    }
    
    return result;
  }
  
  std::vector<double> MovingAverage::ComputeDMA(const std::vector<double>& closes, int period)
  {
    std::vector<double> dma(closes.size(), 0.0);
    if (closes.size() < static_cast<size_t>(period))
      return dma;
    
    double sum = 0.0;
    for (size_t i = 0; i < closes.size(); ++i)
    {
      sum += closes[i];
      if (i >= static_cast<size_t>(period))
        sum -= closes[i - period];
      if (i >= static_cast<size_t>(period - 1))
        dma[i] = sum / period;
    }
    
    return dma;
  }
  std::vector<double> MovingAverage::ComputeEMA(const std::vector<double>& closes, int period)
  {
    std::vector<double> ema(closes.size(), 0.0);
    if (closes.empty()) return ema;
    
    double mult = 2.0 / (period + 1.0);
    ema[0] = closes[0];
    
    for (size_t i = 1; i < closes.size(); ++i)
      ema[i] = (closes[i] - ema[i - 1]) * mult + ema[i - 1];
    
    return ema;
  }
} // namespace KanVest
