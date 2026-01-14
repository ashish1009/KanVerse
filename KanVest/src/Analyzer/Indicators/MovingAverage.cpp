//
//  MovingAverage.cpp
//  KanVest
//
//  Created by Ashish . on 14/01/26.
//

#include "MovingAverage.hpp"

#include "Analyzer/Indicators/IndicatorUtils.hpp"

namespace KanVest
{
  static std::vector<int> GetActivePeriods(const std::string& chartRange)
  {
    if (chartRange == "1mo") return {5, 10, 20};
    if (chartRange == "3mo") return {5, 10, 20, 30, 50};
    return ValidPeriods;
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
      result.dmaValues[p] = dma;
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
} // namespace KanVest
