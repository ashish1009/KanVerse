//
//  UI_Chart.cpp
//  KanVest
//
//  Created by Ashish . on 09/12/25.
//

#include "UI_Chart.hpp"

#include "Stock/StockUtils.hpp"

#include "UI/UI_Utils.hpp"

namespace KanVest
{
#define Font(font) KanVest::UI::Font::Get(KanVest::UI::FontType::font)
  
  using Align = KanVasX::UI::AlignX;
  using Color = KanVasX::Color;

  void Chart::Show(const StockData &stockData)
  {
    IK_PERFORMANCE_FUNC("Chart::Show");
    
    // Check if stock data is valid
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(Font(Header_24), "No Chart Available !!", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, Color::Error);
      return;
    }
    
    // Get stock's candle history data
    const auto& candleHistory = stockData.candleHistory;
    
    // Check if candle history is empty
    if (candleHistory.empty())
    {
      KanVasX::UI::Text(Font(Header_24), "No Candle Data Available !!", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, Color::Error);
      return;
    }
    
    // Keep only trading days (weekdays)
    std::vector<CandleData> filteredDaysCandles = Utils::FilterTradingDays(candleHistory);
    
    // We'll use sequential x indices so weekends/holidays are skipped visually.
    std::vector<double> xs, opens, highs, lows, closes;
    xs.reserve(filteredDaysCandles.size());
    opens.reserve(filteredDaysCandles.size());
    highs.reserve(filteredDaysCandles.size());
    lows.reserve(filteredDaysCandles.size());
    closes.reserve(filteredDaysCandles.size());
    
    double ymin = DBL_MAX;
    double ymax = -DBL_MAX;
    
    for (size_t i = 0; i < filteredDaysCandles.size(); ++i)
    {
      const auto& candle = filteredDaysCandles[i];
      xs.push_back(static_cast<double>(i)); // sequential index
      opens.push_back(candle.open);
      highs.push_back(candle.high);
      lows.push_back(candle.low);
      closes.push_back(candle.close);
      ymin = std::min(ymin, candle.low);
      ymax = std::max(ymax, candle.high);
    }
    
    // Build human-readable labels locally (e.g., "2025-11-03").
    // To avoid too many labels, we'll place a label every few points.
    std::vector<std::string> labelStrings;
    std::vector<const char*> labelPtrs;
    std::vector<double> labelPositions;

    // Decide label step based on number of points: aim for ~8-12 labels max.
    int targetLabels = 10;
    size_t n = filteredDaysCandles.size();
    size_t labelStep = 1;
    if (n > 0)
    {
      labelStep = std::max<size_t>(1, (n + targetLabels - 1) / targetLabels);
    }
    
    labelStrings.reserve((n + labelStep - 1) / labelStep);
    labelPositions.reserve(labelStrings.capacity());
    labelPtrs.reserve(labelStrings.capacity());

    // Lable string
    for (size_t i = 0; i < n; i += labelStep)
    {
      time_t t = static_cast<time_t>(filteredDaysCandles[i].timestamp);
      
      struct tm tm{};
      localtime_r(&t, &tm);   // Use IST instead of UTC
      
      char buf[64];
      
      // Date + Time + AM/PM
      if (stockData.range == "1d")
      {
        std::strftime(buf, sizeof(buf), "%I:%M %p", &tm);
      }
      else
      {
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M %p", &tm);
      }
      
      labelStrings.emplace_back(buf);
      labelPositions.push_back(static_cast<double>(i));
    }

    // Convert label strings to char* for ImPlot
    for (auto &s : labelStrings)
    {
      labelPtrs.push_back(s.c_str());
    }

    // Start plotting
    KanVasX::ScopedColor ChartBg(ImGuiCol_WindowBg, Color::Background);
    KanVasX::ScopedColor ChartBorderBg(ImGuiCol_FrameBg, Color::Background);
    KanVasX::ScopedColor ButtonHovered(ImGuiCol_ButtonHovered, Color::Background);

    static const auto ChartFlag =  ImPlotFlags_NoFrame | ImPlotFlags_NoMenus;
    if (ImPlot::BeginPlot("##StockPlot", ImVec2(ImGui::GetContentRegionAvail().x - 1.0f, 500.0f), ChartFlag))
    {
      // We use AutoFit for X and set Y limits explicitly
      ImPlot::SetupAxes("", "", ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoGridLines, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoGridLines);
      
      // Axis limit setup
      ImPlot::SetupAxisLimits(ImAxis_Y1, ymin - 1.0, ymax + 1.0, ImGuiCond_Always);

      // Setup X axis ticks with our custom positions and labels (compressed timeline)
      if (!labelPositions.empty())
      {
        // ImPlot::SetupAxisTicks accepts arrays of positions and labels.
        // Provide positions (double*) and labels (const char*[])
        ImPlot::SetupAxisTicks(ImAxis_X1, labelPositions.data(), static_cast<int>(labelPositions.size()), labelPtrs.data());
      }

      // Plot close line
      ShowLinePlot(stockData, xs, closes);

      // Reference Line and value
      ShowReferenceLine(stockData.prevClose, ymin - 1.0, ymax + 1.0, xs, Color::Alpha(Color::Gray, 0.5f));

      ImPlot::EndPlot();
    }
  }
  
  void Chart::DrawDashedHLine(double refValue, double xMin, double xMax, ImU32 color, float thickness, float dashLen, float gapLen)
  {
    // Convert start & end plot coordinates to pixel positions
    ImVec2 p1 = ImPlot::PlotToPixels(xMin, refValue);
    ImVec2 p2 = ImPlot::PlotToPixels(xMax, refValue);
    
    ImDrawList* dl = ImPlot::GetPlotDrawList();
    float x = p1.x;
    
    while (x < p2.x)
    {
      float xEnd = x + dashLen;
      if (xEnd > p2.x)
      {
        xEnd = p2.x;
      }
      
      dl->AddLine(ImVec2(x, p1.y), ImVec2(xEnd, p1.y), color, thickness);
      x += dashLen + gapLen;
    }
  }
  
  void Chart::ShowReferenceLine(float refValue, double yminPlot, double ymaxPlot, const std::vector<double>& xs, const ImU32& color)
  {
    if (refValue < yminPlot)
    {
      refValue = static_cast<float>(yminPlot);
    }
    if (refValue > ymaxPlot)
    {
      refValue = static_cast<float>(ymaxPlot);
    }
    
    // Dashed line
    DrawDashedHLine(refValue, xs.front(), xs.back(), color, 1.5f, 5.0f, 5.0f);
    
    // Convert plot coordinates to pixel position
    ImVec2 pixPos = ImPlot::PlotToPixels(xs.front(), refValue);
    
    // Apply pixel offset (0 right, +10 down)
    pixPos.x += 0;
    pixPos.y += 5;
    
    // Draw text manually with no background
    ImDrawList* dl = ImPlot::GetPlotDrawList();
    dl->AddText(Font(Header_24), ImGui::GetFontSize(), pixPos,
                color, ("Prev Close: " + KanVest::UI::Utils::FormatDoubleToString(refValue)).c_str());
    
  }
  
  void Chart::ShowLinePlot(const StockData& stockData, const std::vector<double>& xs, const std::vector<double>& closes)
  {
    double priceChange = stockData.livePrice - stockData.prevClose;
    ImU32 color = priceChange > 0 ? Color::Cyan : Color::Red;
    
    ImVec4 col4 = ImGui::ColorConvertU32ToFloat4(color);
    ImPlot::SetNextLineStyle(col4, 2.0f);
    ImPlot::PlotLine("", xs.data(), closes.data(), static_cast<int>(xs.size()));
  }

} // namespace KanVest
