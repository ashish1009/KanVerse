//
//  CandlePatternUI.hpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
//

#pragma once

#include "Stocks/StockData.hpp"

#include "Stocks/Analyzer/Candle/CandleTypes.hpp"

namespace KanVest::UI
{
  class CandlePatternsUI
  {
  public:
    // Renders pattern panel and markers on current chart. Returns nothing.
    static void RenderPatternList(const std::vector<std::pair<size_t, Candle::CandlePattern>>& patterns, const StockData& stock);
    static void RenderPatternMarkers(const std::vector<std::pair<size_t, Candle::CandlePattern>>& patterns, const StockData& stock, float chartLeftX, float chartTopY, float candleWidth, double priceToPixel);
  };
} // namespace KanVest::UI
