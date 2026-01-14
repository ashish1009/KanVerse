//
//  UI_Chart.hpp
//  KanVest
//
//  Created by Ashish . on 12/01/26.
//

#pragma once

#include "Stock/StockManager.hpp"

namespace KanVest
{
  class Chart
  {
  public:
    /// This function shows the stock chart
    /// - Parameter stockData: stockData
    static void Show(const StockData& stockData);
    
  private:
    static void ShowController(const StockData& stockData);
    
    static void PLotChart(const StockData& stockData);
    static void ComputeCandleWidth(const std::vector<double>& xs);

    static void ShowLinePlot(const StockData& stockData, const std::vector<double>& xs, const std::vector<double>& closes);
    static void ShowCandlePlot(const StockData& stockData, const std::vector<double>& xs, const std::vector<double>& closes,
                               const std::vector<double>& opens, const std::vector<double>& highs, const std::vector<double>& lows);

    // Stock change cache
    inline static bool s_stockChanged = true;
    inline static std::string s_lastSymbol;
    inline static std::string s_lastRange;
    inline static std::string s_lastInterval;
    
    // Plot Type
    enum class PlotType {Line, Candle};
    inline static PlotType s_plotType = PlotType::Candle;
    
    // Candle Data
    inline static float s_candleWidth = 4.0f;
  };
} // namespace KanVest
