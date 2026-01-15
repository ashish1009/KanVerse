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
    struct MovingAverage_UI_Data
    {
      bool show = true;
      int period = 5;
      int periodIdx = 0;
      glm::vec4 color = {0.8, 0.4, 0.1, 1.0};
    };

    static void ShowController(const StockData& stockData);
    
    static void PLotChart(const StockData& stockData);
    static void ComputeCandleWidth(const std::vector<double>& xs);

    static void ShowLinePlot(const StockData& stockData, const std::vector<double>& xs, const std::vector<double>& closes);
    static void ShowCandlePlot(const StockData& stockData, const std::vector<double>& xs, const std::vector<double>& closes,
                               const std::vector<double>& opens, const std::vector<double>& highs, const std::vector<double>& lows);

    static void ShowVolumes(const std::vector<double>& xs, const std::vector<double>& volumeY, const std::vector<double>& opens,
                            const std::vector<double>& closes, double volBottom);

    static void ShowCrossHair(const std::vector<double>& xs, double ymin, double ymax);
    
    static void DrawDashedHLine(double refValue, double xMin, double xMax, ImU32 color,
                                float thickness = 1.5f, float dashLen = 10.0f, float gapLen = 5.0f);
    static void ShowReferenceLine(float refValue, double yminPlot, double ymaxPlot, const std::vector<double>& xs, const ImU32& color);
    
    static void ShowTooltip(const StockData& stockData, const std::vector<CandleData>& filteredDaysCandles);

    static void ShowMAControler(const std::string& title, MovingAverage_UI_Data& data);
    static void ShowMAPlot(const MovingAverage_UI_Data& MA_UI_Data, const std::map<int, std::vector<double>>& MA_Data, const std::vector<double> &xs);

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
    
    // Indicator UI Data
    enum class Indicator {None, DMA, EMA};
    inline static Indicator s_selectedIndicator = Indicator::None;
    inline static std::unordered_map<int /* Period */, MovingAverage_UI_Data> s_DMA_UI_Data;
    inline static std::unordered_map<int /* Period */, MovingAverage_UI_Data> s_EMA_UI_Data;
  };
} // namespace KanVest
