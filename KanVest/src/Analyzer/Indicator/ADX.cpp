//
//  ADX.cpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#include "ADX.hpp"

#include "MovingAverage.hpp"

namespace KanVest
{
  ADXResult ADXCalculator::Compute(const StockData& stock, int period)
  {
    ADXResult result;
    
    if (!stock.IsValid() || stock.history.size() < (size_t)period + 2)
      return result;
    
    const size_t N = stock.history.size();
    
    std::vector<double> tr(N), plusDM(N), minusDM(N);
    
    // ---------------------------
    // 1. TR, +DM, -DM
    // ---------------------------
    for (size_t i = 1; i < N; i++)
    {
      const auto& prev = stock.history[i - 1];
      const auto& curr = stock.history[i];
      
      double highDiff = curr.high - prev.high;
      double lowDiff  = prev.low - curr.low;
      
      double TR1 = curr.high - curr.low;
      double TR2 = fabs(curr.high - prev.close);
      double TR3 = fabs(curr.low  - prev.close);
      
      tr[i] = std::max(TR1, std::max(TR2, TR3));
      
      plusDM[i]  = (highDiff > 0 && highDiff > lowDiff) ? highDiff : 0.0;
      minusDM[i] = (lowDiff  > 0 && lowDiff  > highDiff) ? lowDiff : 0.0;
    }
    
    // ---------------------------
    // 2. Smooth TR and DM
    // ---------------------------
    std::vector<double> trSm, plusSm, minusSm;
    trSm.reserve(N); plusSm.reserve(N); minusSm.reserve(N);
    
    double trSum = 0, plusSum = 0, minusSum = 0;
    
    for (int i = 1; i <= period; i++)
    {
      trSum    += tr[i];
      plusSum  += plusDM[i];
      minusSum += minusDM[i];
    }
    
    trSm.push_back(trSum);
    plusSm.push_back(plusSum);
    minusSm.push_back(minusSum);
    
    for (size_t i = period + 1; i < N; i++)
    {
      trSum    = trSum    - (trSum / period)    + tr[i];
      plusSum  = plusSum  - (plusSum / period)  + plusDM[i];
      minusSum = minusSum - (minusSum / period) + minusDM[i];
      
      trSm.push_back(trSum);
      plusSm.push_back(plusSum);
      minusSm.push_back(minusSum);
    }
    
    const size_t offset = trSm.size();
    
    // ---------------------------
    // 3. DI+ & DI-
    // ---------------------------
    result.plusDI.resize(offset);
    result.minusDI.resize(offset);
    
    for (size_t i = 0; i < offset; i++)
    {
      result.plusDI[i]  = (plusSm[i]  / trSm[i]) * 100.0;
      result.minusDI[i] = (minusSm[i] / trSm[i]) * 100.0;
    }
    
    // ---------------------------
    // 4. DX
    // ---------------------------
    std::vector<double> dx(offset);
    
    for (size_t i = 0; i < offset; i++)
    {
      double sumDI = result.plusDI[i] + result.minusDI[i];
      dx[i] = (sumDI == 0) ? 0 : fabs(result.plusDI[i] - result.minusDI[i]) / sumDI * 100.0;
    }
    
    // ---------------------------
    // 5. ADX = EMA(DX)
    // ---------------------------
    result.adx = MovingAverages::ComputeEMA(dx, period);
    
    if (!result.adx.empty())
    {
      result.last = result.adx.back();
      result.valid = true;
    }
    
    return result;
  }
} // namespace KanVest

