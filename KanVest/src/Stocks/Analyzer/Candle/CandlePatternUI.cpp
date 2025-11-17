//
//  CandlePatternUI.cpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
//

#include "CandlePatternUI.hpp"

namespace KanVest::UI
{
  void CandlePatternsUI::RenderPatternList(const std::vector<std::pair<size_t, Candle::CandlePattern>>& patterns, const StockData& stock)
  {
    ImGui::Begin("Patterns");
    if (patterns.empty())
    {
      ImGui::Text("No patterns detected in lookback range.");
      ImGui::End();
      return;
    }
    
    
    for (auto &pr : patterns)
    {
      size_t idx = pr.first;
      const auto &p = pr.second;
      const auto &candle = stock.history[idx];
      
      
      ImU32 col = p.bullish ? IM_COL32(0,255,0,255) : (p.bearish ? IM_COL32(255,0,0,255) : IM_COL32(255,200,0,255));
//      ImGui::TextColored(col, "%s (idx=%zu) [%.0f%%]", p.name.c_str(), idx, p.confidence*100.0);
      ImGui::TextWrapped("%s", p.description.c_str());
      ImGui::Text("O: %.2f H: %.2f L: %.2f C: %.2f", candle.open, candle.high, candle.low, candle.close);
      ImGui::Separator();
    }
    
    
    ImGui::End();
  }
  
  
  void CandlePatternsUI::RenderPatternMarkers(const std::vector<std::pair<size_t, Candle::CandlePattern>>& patterns, const StockData& stock, float chartLeftX, float chartTopY, float candleWidth, double priceToPixel)
  {
    // chartLeftX, chartTopY are top-left coordinates of chart area
    // candleWidth is horizontal spacing per candle
    // priceToPixel is a multiplier to convert price to vertical pixels (pixelY = chartTopY + (maxPrice - price)*priceToPixel)
    ImDrawList* dl = ImGui::GetWindowDrawList();
    if (!dl) return;
    
    
    // find max price in history for mapping (simple approach)
    double maxPrice = -1e18;
    for (const auto &p : stock.history) maxPrice = std::max(maxPrice, p.high);
    
    
    for (auto &pr : patterns)
    {
      size_t idx = pr.first;
      const auto &pat = pr.second;
      const auto &candle = stock.history[idx];
      
      
      float cx = chartLeftX + (float)(idx) * candleWidth + candleWidth * 0.5f;
      float cy = (float)(chartTopY + (maxPrice - candle.high) * priceToPixel) - 8.0f; // place above high
      
      
      ImU32 col = pat.bullish ? IM_COL32(0,200,0,255) : (pat.bearish ? IM_COL32(200,0,0,255) : IM_COL32(200,200,0,255));
      dl->AddCircleFilled(ImVec2(cx, cy), 6.0f, col);
      dl->AddText(ImVec2(cx + 8, cy - 8), IM_COL32(255,255,255,255), pat.name.c_str());
    }
  }
}
