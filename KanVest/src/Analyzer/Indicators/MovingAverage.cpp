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
    return ValidMovingAveragePeriods;
  }
  
  std::unordered_map<MAPriceSource, MAResult> MovingAverage::Compute(const StockData& data)
  {
    std::unordered_map<MAPriceSource, MAResult> result;
    
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
    
    for (int32_t i = 0; i < (int32_t)MAPriceSource::Limit; i++)
    {
      for (int p : periods)
      {
        result[(MAPriceSource)i].dmaValues[p] = ComputeDMA(closesCandle, p, (MAPriceSource)i);
        result[(MAPriceSource)i].emaValues[p] = ComputeEMA(closesCandle, p, (MAPriceSource)i);
      }
    }
    return result;
  }

  std::vector<double> MovingAverage::ComputeDMA(const std::vector<double>& closes, int period, MAPriceSource priceSource)
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
  
  std::vector<double> MovingAverage::ComputeEMA(const std::vector<double>& closes, int period, MAPriceSource priceSource)
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
