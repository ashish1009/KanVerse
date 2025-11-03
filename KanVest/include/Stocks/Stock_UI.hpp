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
    static StockData UpdateStockData(const std::string& symbol);
    
    static void StockAnalyzer();
    static void StockVestData();

    static void SearchBar();

//    static void DrawCandleChart(const std::vector<StockPoint>& history);
//    static void ShowBasicStockData(const StockData& stockData);
//    static void ChartHelper(const StockData& stockData);
    
    inline static ImTextureID s_reloadIconID = 0;
//    inline static bool s_showCandle = true;
  };
} // namespace KanVest
