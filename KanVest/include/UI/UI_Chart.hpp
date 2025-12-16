//
//  UI_Chart.hpp
//  KanVest
//
//  Created by Ashish . on 09/12/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  class Chart
  {
  public:
    /// This function shows the stock chart
    /// - Parameter stockData: stockData
    static void Show(const StockData& stockData);
    
  private:
    static void PLotChart(const StockData& stockData);
    static void ShowVolumes(const std::vector<double>& xs, const std::vector<double>& volumeY, const std::vector<double>& opens, const std::vector<double>& closes, double volBottom);
    
    static void ShowLinePlot(const StockData& stockData, const std::vector<double>& xs, const std::vector<double>& closes);
    static void ShowCandlePlot(const StockData& stockData,
                               const std::vector<double>& xs,
                               const std::vector<double>& closes,
                               const std::vector<double>& opens,
                               const std::vector<double>& highs,
                               const std::vector<double>& lows);

    static void ShowCrossHair(const std::vector<double>& xs, double ymin, double ymax);
    static void ComputeCandleWidth(const std::vector<double>& xs);

    static void DrawDashedHLine(double refValue, double xMin, double xMax, ImU32 color, float thickness = 1.5f, float dashLen = 10.0f, float gapLen = 5.0f);
    static void ShowReferenceLine(float refValue, double yminPlot, double ymaxPlot, const std::vector<double>& xs, const ImU32& color);

    static void ShowTooltip(const StockData& stockData, const std::vector<CandleData>& filteredDaysCandles);
    
    static void ShowController(const StockData& stockData);

    static void ShowDMA(const StockData& stockData, const std::vector<double>& xs);

    // Plot Type
    enum class PlotType {Line, Candle};
    inline static PlotType s_plotType = PlotType::Candle;
    
    // Candle Data
    inline static float s_candleWidth = 4.0f;
    
    // Stock change cache
    inline static bool s_stockChanged = true;
    inline static std::string s_lastSymbol;
    inline static std::string s_lastRange;
    inline static std::string s_lastInterval;
    
    // Technicals
    inline static bool s_showDMA = false;
    inline static int s_DMAPeriod = 5;
    inline static glm::vec4 s_DMAColor = {0.8, 0.4, 0.1, 1.0};
  };
} // namespace KanVest
