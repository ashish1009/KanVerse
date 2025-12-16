//
//  IndicatorUtils.cpp
//  KanVest
//
//  Created by Ashish . on 15/12/25.
//

#include "IndicatorUtils.hpp"

namespace KanVest::Indicator::Utils
{
  std::vector<double> BuildDailyCloses(const StockData& data)
  {
    if (!data.IsValid())
      return {};
    
    std::map<int64_t, double> dailyMap; // key = YYYYMMDD → last close
    
    for (const auto& p : data.candleHistory)
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
  std::vector<double> GetCandleCloses(const StockData& data)
  {
    if (!data.IsValid())
      return {};

    std::vector<double> closes;

    for (const auto& p : data.candleHistory)
    {
      closes.emplace_back(p.close);
    }
    return closes;
  }
} // namespace KanVest::Indicator::Utils
