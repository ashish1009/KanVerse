//
//  KanVestPanel.hpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
//

#pragma once

namespace KanVest::UI
{
  class Panel
  {
  public:
    /// This is primary stock UI panel
    static void Show();
    
    static void SetShadowTextureId(ImTextureID shadowTextureID);
    
  private:
    static void ShowStockAnalyzerTable();
    static void ShowPortfolioChartTable();
    
    static void ShowPortfolio();
    static void ShowChart();
    
    inline static bool s_showCandle = true;
    inline static ImTextureID s_shadowTextureID = 0;
  };
} // namespace KanVest
