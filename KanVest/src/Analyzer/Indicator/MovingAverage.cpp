//
//  MovingAverage.cpp
//  KanVest
//
//  Created by Ashish . on 23/11/25.
//

#include "MovingAverage.hpp"

namespace KanVest
{
  static std::vector<int> GetActivePeriods(const std::string& chartRange)
  {
    if (chartRange == "1mo") return {5, 10, 20};
    if (chartRange == "3mo") return {5, 10, 20, 30, 50};
    return {5, 10, 20, 30, 50, 100, 150, 200};
  }
  
  std::vector<double> BuildDailyCloses(const StockData& data)
  {
    if (!data.IsValid())
      return {};
    
    std::map<int64_t, double> dailyMap; // key = YYYYMMDD → last close
    
    for (const auto& p : data.history)
    {
      time_t t = p.timestamp;
      tm* g = gmtime(&t);
      
      int y = g->tm_year + 1900;
      int m = g->tm_mon  + 1;
      int d = g->tm_mday;
      
      int64_t key = y * 10000 + m * 100 + d;
      
      // overwrite = keep last close of the day
      dailyMap[key] = p.close;
    }
    
    // Extract in sorted order
    std::vector<double> daily;
    daily.reserve(dailyMap.size());
    
    for (auto& kv : dailyMap)
      daily.push_back(kv.second);
    
    return daily;
  }

  MAResult MovingAverage::Compute(const StockData& data)
  {
    MAResult result;

    if (!data.IsValid())
    {
      return result;
    }

    // FIX: convert ANY raw history (1W,1D,1h..)-> daily
    auto dailyCloses = BuildDailyCloses(data);
    if (dailyCloses.size() < 5)
    {
      return result;
    }

    auto periods = GetActivePeriods(data.range);
    
    for (int p : periods)
    {
      auto sma = ComputeSMA(dailyCloses, p);
      auto ema = ComputeEMA(dailyCloses, p);
      
      result.smaValues[p] = sma.back();
      result.emaValues[p] = ema.back();
    }

    return result;
  }
  
  std::vector<double> MovingAverage::ComputeSMA(const std::vector<double>& closes, int period)
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
