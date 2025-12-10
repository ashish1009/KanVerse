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
    static void DrawDashedHLine(double refValue, double xMin, double xMax, ImU32 color, float thickness = 1.5f, float dashLen = 10.0f, float gapLen = 5.0f);
    static void ShowReferenceLine(float refValue, double yminPlot, double ymaxPlot, const std::vector<double>& xs, const ImU32& color);
    static void ShowLinePlot(const StockData& stockData, const std::vector<double>& xs, const std::vector<double>& closes);
  };
} // namespace KanVest
