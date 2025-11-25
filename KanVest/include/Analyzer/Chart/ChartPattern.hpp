//
//  ChartPattern.hpp
//  KanVest
//
//  Created by Ashish . on 25/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  enum class ChartPatternType
  {
    NONE,
    DOUBLE_TOP,
    DOUBLE_BOTTOM,
    HEAD_AND_SHOULDERS,
    INVERSE_HEAD_AND_SHOULDERS,
    CUP_AND_HANDLE,
    TRIANGLE_ASCENDING,
    TRIANGLE_DESCENDING,
    TRIANGLE_SYMMETRICAL,
    FLAG,
    PENNANT
  };
  
  struct ChartPatternResult
  {
    ChartPatternType type = ChartPatternType::NONE;
    int startIndex = -1;
    int peakIndex = -1; // main reference point (peak or trough)
    int endIndex = -1;
    std::string name;
    double confidence = 0.0; // 0..1
    
    bool IsValid() const { return type != ChartPatternType::NONE; }
  };
  
  class ChartPatternEngine
  {
  public:
    ChartPatternEngine() {}
    
    // Analyze provided candles and return detected patterns
    // lookback: how many most recent candles to analyze (-1 = all)
    static std::vector<ChartPatternResult> Analyze(const std::vector<StockPoint>& candles, int lookback = -1);
    
  private:
    static ChartPatternResult DetectDoubleTop(const std::vector<StockPoint>& c, int searchStart, int searchEnd);
    static ChartPatternResult DetectDoubleBottom(const std::vector<StockPoint>& c, int searchStart, int searchEnd);
    static ChartPatternResult DetectHeadAndShoulders(const std::vector<StockPoint>& c, int searchStart, int searchEnd);
    static ChartPatternResult DetectInverseHeadAndShoulders(const std::vector<StockPoint>& c, int searchStart, int searchEnd);
    static ChartPatternResult DetectCupAndHandle(const std::vector<StockPoint>& c, int searchStart, int searchEnd);
    static ChartPatternResult DetectTriangles(const std::vector<StockPoint>& c, int searchStart, int searchEnd);
    
    // Utility helpers
    static bool IsLocalMax(const std::vector<StockPoint>& c, int idx, int halfWindow = 3);
    static bool IsLocalMin(const std::vector<StockPoint>& c, int idx, int halfWindow = 3);
    static double PeakHeight(const StockPoint& p) { return p.high; }
    static double TroughHeight(const StockPoint& p) { return p.low; }
  };
} // namespace KanVest
