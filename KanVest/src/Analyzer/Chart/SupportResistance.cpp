//
//  SupportResistance.cpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#include "SupportResistance.hpp"

namespace KanVest
{
  bool isSwingHigh(const StockData& data, int i, int left, int right)
  {
    for (int j = 1; j <= left; j++)
      if (data.history[i].high <= data.history[i - j].high) return false;
    
    for (int j = 1; j <= right; j++)
      if (data.history[i].high <= data.history[i + j].high) return false;
    
    return true;
  }
  
  bool isSwingLow(const StockData& data, int i, int left, int right)
  {
    for (int j = 1; j <= left; j++)
      if (data.history[i].low >= data.history[i - j].low) return false;
    
    for (int j = 1; j <= right; j++)
      if (data.history[i].low >= data.history[i + j].low) return false;
    
    return true;
  }
  
  void SupportResistance::Compute(const StockData& data, int pivotRange, double clusterTolerance )
  {
    const auto& candles = data.history;
    int size = (int)candles.size();
    
    if (size < pivotRange * 2 + 1)
      return;
    
    for (int i = pivotRange; i < size - pivotRange; i++)
    {
      double high = candles[i].high;
      double low  = candles[i].low;
      
      // -----------------------------------------------------
      // Detect Resistance (Swing High)
      // -----------------------------------------------------
      if (isSwingHigh(data, i, pivotRange, pivotRange))
      {
        bool merged = false;
        
        for (auto& r : resistances)
        {
          if (fabs(r.price - high) <= clusterTolerance)
          {
            // merge cluster
            r.price = (r.price * r.count + high) / (r.count + 1);
            r.count++;
            r.lastTouchIdx = i;
            merged = true;
            break;
          }
        }
        
        if (!merged)
        {
          resistances.push_back({ high, 1, i });
        }
      }
      
      // -----------------------------------------------------
      // Detect Support (Swing Low)
      // -----------------------------------------------------
      if (isSwingLow(data, i, pivotRange, pivotRange))
      {
        bool merged = false;
        
        for (auto& s : supports)
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
        
        if (!merged)
        {
          supports.push_back({ low, 1, i });
        }
      }
    }
  }
} // namespace KanVest
