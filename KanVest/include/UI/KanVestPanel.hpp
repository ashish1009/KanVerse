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
    static void SetReloadTextureId(ImTextureID reloadTextureID);

  private:
    static void ShowStockAnalyzerTable();
    static void ShowPortfolioChartTable();
    
    static void ShowPortfolio();
    static void ShowChart();
    
    static void ShowStockData();
    
    static void ShowSearchBar();
    static void ShowStockBasicData();
    
    static void ShowStockTechnicalData();

    static void AddStockInManager(const std::string& symbol);

    inline static bool s_showCandle = true;
    inline static char s_searchedString[128] = "Nifty";
    inline static ImTextureID s_shadowTextureID = 0;
    inline static ImTextureID s_reloadIconID = 0;
  };
} // namespace KanVest
