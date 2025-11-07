//
//  StockUI.hpp
//  KanVest
//
//  Created by Ashish . on 05/11/25.
//

#pragma once

#include "Portfolio/Portfolio.hpp"

namespace KanVest
{
  /// This class responsible for Stock UI Panel
  class StockUI
  {
  public:
    /// This function initializes Stock UI
    static void Initialize(ImTextureID reloadIconID);
    
    /// This is primary stock UI panel
    static void StockPanel();
    
  private:
    static void AddStockInManager(const std::string& symbol);

    static void ShowStockDetails();
    static void ShowStcokBasicData();

    static void ShowPortfolio();
    static void ShowWatchlist();

    static void DrawCandleChart();
    static void DrawChartController();

    static void DrawPortfolioTable(Portfolio* portfolio);
    static void SearchBar();

    inline static bool s_showCandle = true;
    inline static ImTextureID s_reloadIconID = 0;
    inline static char s_searchedString[128] = "Nifty";
  };
} // namespace KanVest
