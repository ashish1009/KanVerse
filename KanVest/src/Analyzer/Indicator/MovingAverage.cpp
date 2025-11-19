//
//  MovingAverage.cpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
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
  
  // -------------------------------------------------------------
  // Convert raw candles → daily closes based on UNIX timestamps
  // -------------------------------------------------------------
  std::vector<double> MovingAverages::BuildDailyCloses(const StockData& data)
  {
    if (!data.IsValid())
      return {};
    
    std::map<int64_t, double> dailyMap;    // key = YYYYMMDD, value = close
    
    for (const auto& p : data.history)
    {
      // Convert timestamp → date bucket (UTC)
      time_t t = p.timestamp;
      tm* g = gmtime(&t);
      
      int y = g->tm_year + 1900;
      int m = g->tm_mon + 1;
      int d = g->tm_mday;
      
      int64_t dateKey = y * 10000 + m * 100 + d;
      
      // Use the latest close for each date
      dailyMap[dateKey] = p.close;
    }
    
    // Now build a continuous daily sequence (fill gaps)
    std::vector<double> daily;
    
    auto it = dailyMap.begin();
    if (it == dailyMap.end()) return daily;
    
    int64_t prevDate = it->first;
    double prevClose = it->second;
    
    daily.push_back(prevClose);
    ++it;
    
    for (; it != dailyMap.end(); ++it)
    {
      int64_t curDate = it->first;
      double curClose = it->second;
      
      // Fill date gaps with previous close (forward fill)
      int y1 = (int)prevDate / 10000;
      int m1 = ((int)prevDate % 10000) / 100;
      int d1 = (int)prevDate % 100;
      
      int y2 = (int)curDate / 10000;
      int m2 = ((int)curDate % 10000) / 100;
      int d2 = (int)curDate % 100;
      
      // Convert to time_t for date increment
      tm tm1 = {};
      tm1.tm_year = y1 - 1900;
      tm1.tm_mon  = m1 - 1;
      tm1.tm_mday = d1;
      time_t t1 = timegm(&tm1);
      
      tm tm2 = {};
      tm2.tm_year = y2 - 1900;
      tm2.tm_mon  = m2 - 1;
      tm2.tm_mday = d2;
      time_t t2 = timegm(&tm2);
      
      // Insert missing days
      for (time_t t = t1 + 86400; t < t2; t += 86400)
        daily.push_back(prevClose);
      
      // Insert current close
      daily.push_back(curClose);
      
      prevDate = curDate;
      prevClose = curClose;
    }
    
    return daily;
  }
  
  // -------------------------------------------------------------
  // SMA
  // -------------------------------------------------------------
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
  
  // -------------------------------------------------------------
  // EMA
  // -------------------------------------------------------------
  std::vector<double> MovingAverages::ComputeEMA(const std::vector<double>& closes, int period)
  {
    std::vector<double> ema(closes.size(), 0.0);
    if (closes.empty()) return ema;
    
    double mult = 2.0 / (period + 1.0);
    ema[0] = closes[0];
    
    for (size_t i = 1; i < closes.size(); ++i)
      ema[i] = (closes[i] - ema[i - 1]) * mult + ema[i - 1];
    
    return ema;
  }
  
  // -------------------------------------------------------------
  // Compute MA for DAILY-NORMALIZED data (interval independent)
  // -------------------------------------------------------------
  MAResult MovingAverages::Compute(const StockData& data, const std::string& chartRange)
  {
    MAResult result;
    
    if (!data.IsValid())
      return result;
    
    // FIX: convert ANY raw history (1W,1D,1h..)-> daily
    auto dailyCloses = BuildDailyCloses(data);
    if (dailyCloses.size() < 5)
      return result;
    
    auto periods = GetActivePeriods(chartRange);
    
    for (int p : periods)
    {
      auto sma = ComputeSMA(dailyCloses, p);
      auto ema = ComputeEMA(dailyCloses, p);
      
      result.smaValues[p] = sma.back();
      result.emaValues[p] = ema.back();
    }
    return result;
  }
} // namespace KanVest
