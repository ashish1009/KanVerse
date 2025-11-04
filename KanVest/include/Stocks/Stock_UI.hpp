//
//  Stock_UI.hpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#pragma once

#include "Stocks/StockData.hpp"

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
    static void UpdateStockData(const std::string& symbol);
    
    static void ShowStockData();
    static void ShowStockInsights();
    static void ShowStockTechnicals();

    static void SearchBar();
    static void ShowStcokBasicData(const StockData& stockData);

    static void DrawCandleChart(const StockData& stockData);
    static void DrawChartController(const StockData& stockData);
    
    static void ShowPortfolio();
    
    inline static ImTextureID s_reloadIconID = 0;
    inline static bool s_showCandle = true;
    
    inline static double s_lastUpdateTime = 0.0;
    inline static std::string s_lastUpdatedString = "Never";
  };
} // namespace KanVest
