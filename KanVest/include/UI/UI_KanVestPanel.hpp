//
//  UI_KanVestPanel.hpp
//  KanVest
//
//  Created by Ashish . on 21/11/25.
//

#pragma once

#include "Portfolio/PortfolioController.hpp"

#include "Analyzer/Chart/Pivot.hpp"

namespace KanVest::UI
{
  class Panel
  {
  public:
    /// This function initializes the stock panel
    static void Initialize();
    /// This is primary stock UI panel
    static void Show();
    
    static void SetShadowTextureId(ImTextureID shadowTextureID);
    static void SetReloadTextureId(ImTextureID reloadTextureID);
    static void SetSettingTextureId(ImTextureID settingIconID);
    static void SetOpenEyeTextureId(ImTextureID openTextureID);
    static void SetCloseEyeTextureId(ImTextureID closeTextureID);

  private:
    static void ShowPortfolio();
    static void ShowPortfolioSummary(Portfolio* portfolio);
    static void ShowHolding(Holding& h);
    static void EditHolding(Portfolio* portfolio, Holding& h);
    static void NewHolding(Portfolio* portfolio);

    static void ShowStockData();
    static void ShowStockSearchBar(float height);
    static void ShowHoldingSearchBar(float height);
    
    static void ShowChart(float chartSize);
    static void ShowStockAnalyzerSummary();
    static void ShowStockTechnicalData();
    
    static void AddStockInManager(const std::string& symbol);
    
    static void DrawSRLevels(const PivotResults& pivots, double xMin, double xMax, double livePrice);
    
    inline static ImTextureID s_shadowTextureID = 0;
    inline static ImTextureID s_reloadIconID = 0;
    inline static ImTextureID s_settingIconID = 0;
    inline static ImTextureID s_openEyeTextureID = 0;
    inline static ImTextureID s_closeEyeTextureID = 0;

    inline static bool s_showCandle = true;
    inline static bool s_showPivots = true;
    inline static int32_t s_numPivotLevel = 1;

    inline static char s_searchedStockString[128] = "Nifty";
    inline static char s_searchedHoldingString[128] = "";
  };
} // namespace KanVest::UI
