//
//  StockData.cpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#include "StockData.hpp"

namespace KanVest
{
  StockData::StockData(const std::string& name)
  : symbol(name) {}
  
  bool StockData::IsValid() const
  {
    return !symbol.empty() && !history.empty();
  }
  
  double StockData::ComputeSMA(int period)
  {
    if (history.size() < period)
    {
      return 0.0;
    }
    
    double sum = 0.0;
    for (int32_t i = (int32_t)history.size() - period; i < (int32_t)history.size(); ++i)
    {
      sum += history[i].close;
    }
    return sum / period;
  }
  
  double StockData::ComputeEMA(int period)
  {
    if (history.size() < period)
    {
      return 0.0;
    }
    
    double k = 2.0 / (period + 1.0);
    double ema = history[history.size() - period].close;
    for (size_t i = history.size() - period + 1; i < history.size(); ++i)
    {
      ema = (history[i].close * k) + (ema * (1.0 - k));
    }
    return ema;
  }

  double StockData::ComputeATR(int period)
  {
    if (history.size() < period + 1)
    {
      return 0.0;
    }
    
    std::vector<double> tr;
    for (size_t i = 1; i < history.size(); ++i)
    {
      double hl = history[i].high - history[i].low;
      double hc = fabs(history[i].high - history[i - 1].close);
      double lc = fabs(history[i].low - history[i - 1].close);
      tr.push_back(std::max({hl, hc, lc}));
    }
    double sum = 0.0;
    for (int i = (int32_t)tr.size() - period; i < (int32_t)tr.size(); ++i)
    {
      sum += tr[i];
    }
    return sum / period;
  }
  
  double StockData::ComputeAverageVolume(int period)
  {
    if (history.size() < period)
    {
      return 0.0;
    }
    
    double sum = 0.0;
    for (int i = (int32_t)history.size() - period; i < (int32_t)history.size(); ++i)
    {
      sum += history[i].volume;
    }
    return sum / period;
  }

  double StockData::ComputeRSI(int period)
  {
    if (history.size() < period + 1)
    {
      return 0.0;
    }
    double gain = 0.0, loss = 0.0;
    for (size_t i = (int32_t)history.size() - period; i < (int32_t)history.size(); ++i)
    {
      double diff = history[i].close - history[i - 1].close;
      if (diff > 0) gain += diff;
      else loss -= diff;
    }
    double avgGain = gain / period;
    double avgLoss = loss / period;
    
    if (avgLoss == 0)
    {
      return 100.0;
    }
    
    double rs = avgGain / avgLoss;
    return 100.0 - (100.0 / (1.0 + rs));
  }
  
  double StockData::ComputeMACD()
  {
    double ema12 = ComputeEMA(12);
    double ema26 = ComputeEMA(26);
    return ema12 - ema26;
  }
  
  double StockData::ComputeVWAP()
  {
    double totalPV = 0.0, totalVol = 0.0;
    for (const auto& p : history)
    {
      double typical = (p.high + p.low + p.close) / 3.0;
      totalPV += typical * p.volume;
      totalVol += p.volume;
    }
    return totalVol > 0 ? totalPV / totalVol : 0.0;
  }
} // namespace KanVest
