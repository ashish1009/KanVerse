//
//  MFICalculator.cpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
//

#include "MFICalculator.hpp"

namespace KanVest
{
  MFIResult MFICalculator::Compute(const StockData& data, int period )
  {
    MFIResult r;
    
    if (!data.IsValid() || data.history.size() < (size_t)period + 1)
      return r;
    
    size_t N = data.history.size();
    
    std::vector<double> tp;    // typical price
    std::vector<double> mf;    // raw money flow
    tp.reserve(N);
    mf.reserve(N);
    
    for (const auto& p : data.history)
    {
      double typical = (p.high + p.low + p.close) / 3.0;
      tp.push_back(typical);
      mf.push_back(typical * p.volume);
    }
    
    r.series.reserve(N);
    
    for (size_t i = period; i < N; i++)
    {
      double posFlow = 0.0;
      double negFlow = 0.0;
      
      for (size_t j = i - period + 1; j <= i; j++)
      {
        if (tp[j] > tp[j - 1])
          posFlow += mf[j];
        else if (tp[j] < tp[j - 1])
          negFlow += mf[j];
      }
      
      if (negFlow == 0.0)
      {
        r.series.push_back(100.0);
        continue;
      }
      
      double ratio = posFlow / negFlow;
      double mfi = 100.0 - (100.0 / (1.0 + ratio));
      
      r.series.push_back(mfi);
    }
    
    if (!r.series.empty())
    {
      r.last = r.series.back();
      r.valid = true;
    }
    
    return r;
  }
} // namespace KanVest
