//
//  IndicatorUtils.cpp
//  KanVest
//
//  Created by Ashish . on 26/11/25.
//

#include "IndicatorUtils.hpp"

namespace KanVest::Indicator::Utils
{
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
} // namespace KanVest::Indicator::Utils
