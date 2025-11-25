//
//  Pivot.cpp
//  KanVest
//
//  Created by Ashish . on 24/11/25.
//

#include "Pivot.hpp"

namespace KanVest
{
  // -------------------------------------
  // Swing High / Low with equality tolerance
  // -------------------------------------
  bool isSwingHigh(const StockData& data, int i, int range)
  {
    const auto& h = data.history;
    for (int j = 1; j <= range; j++)
      if (h[i].high < h[i - j].high || h[i].high < h[i + j].high)
        return false;
    return true;
  }
  
  bool isSwingLow(const StockData& data, int i, int range)
  {
    const auto& h = data.history;
    for (int j = 1; j <= range; j++)
      if (h[i].low > h[i - j].low || h[i].low > h[i + j].low)
        return false;
    return true;
  }
  
  // -------------------------------------
  // Pivot strength score
  // -------------------------------------
  double computeScore(const SRLevel& lvl, int totalCandles)
  {
    double recency = static_cast<double>(lvl.lastTouchIdx) / totalCandles;
    return lvl.count * 2.0 + recency; // weighted count + recency
  }
  
  // -------------------------------------
  // Pivot detection with clustering
  // -------------------------------------
  PivotResults Pivot::Compute(const StockData& data, int pivotRange, double unused)
  {
    PivotResults result;
    const auto& candles = data.history;
    int size = static_cast<int>(candles.size());
    if (size < pivotRange * 2 + 1) return result;
    
    // Dynamic cluster tolerance (~0.7% of last close)
    double clusterTolerance = candles.back().close * 0.007;
    
    for (int i = pivotRange; i < size - pivotRange; i++)
    {
      double high = candles[i].high;
      double low = candles[i].low;
      
      // RESISTANCE
      if (isSwingHigh(data, i, pivotRange))
      {
        bool merged = false;
        for (auto& r : result.resistances)
        {
          if (fabs(r.price - high) <= clusterTolerance)
          {
            r.price = (r.price * r.count + high) / (r.count + 1);
            r.count++;
            r.lastTouchIdx = i;
            merged = true;
            break;
          }
        }
        if (!merged) result.resistances.push_back({high, 1, i});
      }
      
      // SUPPORT
      if (isSwingLow(data, i, pivotRange))
      {
        bool merged = false;
        for (auto& s : result.supports)
        {
          if (fabs(s.price - low) <= clusterTolerance)
          {
            s.price = (s.price * s.count + low) / (s.count + 1);
            s.count++;
            s.lastTouchIdx = i;
            merged = true;
            break;
          }
        }
        if (!merged) result.supports.push_back({low, 1, i});
      }
    }
    
    // Sort strongest first
    auto sorter = [&](const SRLevel& a, const SRLevel& b) {
      return computeScore(a, size) > computeScore(b, size);
    };
    std::sort(result.supports.begin(), result.supports.end(), sorter);
    std::sort(result.resistances.begin(), result.resistances.end(), sorter);
        
    return result;
  }
} // namespace KanVest
