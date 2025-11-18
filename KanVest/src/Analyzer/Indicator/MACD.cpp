//
//  MACD.cpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#include "MACD.hpp"

#include "MovingAverage.hpp"

namespace KanVest
{
  MACDResult MACD::Compute(const StockData& stock)
  {
    MACDResult result;
    
    // Validate stock data
    if (!stock.IsValid() || stock.history.size() < 50)
      return result;
    
    const size_t N = stock.history.size();
    
    // Extract close prices
    std::vector<double> closes;
    closes.reserve(N);
    
    for (const auto& candle : stock.history)
      closes.push_back(candle.close);
    
    // Compute EMAs
    auto ema12 = MovingAverages::ComputeEMA(closes, 12);
    auto ema26 = MovingAverages::ComputeEMA(closes, 26);
    
    // EMA sizes MUST match (your EMA function ensures this)
    if (ema12.size() != ema26.size())
      return result;
    
    size_t size = ema26.size();
    result.macdLine.reserve(size);
    
    // --- MACD Line = EMA12 - EMA26 ---
    for (size_t i = 0; i < size; i++)
      result.macdLine.push_back(ema12[i] - ema26[i]);
    
    // --- Signal Line = EMA(9) of MACD Line ---
    result.signalLine = MovingAverages::ComputeEMA(result.macdLine, 9);
    
    size_t align = result.signalLine.size();
    result.histogram.reserve(align);
    
    // Align MACD to Signal Line for histogram calculation
    size_t macdOffset = result.macdLine.size() - align;
    
    // --- Histogram = MACD - Signal ---
    for (size_t i = 0; i < align; i++)
    {
      double h = result.macdLine[i + macdOffset] - result.signalLine[i];
      result.histogram.push_back(h);
    }
    
    return result;
  }
} // namespace KanVest
