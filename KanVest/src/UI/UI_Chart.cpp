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
  
  static void GetTimeString(char* buf, size_t bufSize, uint64_t timestamp, const std::string& range)
  {
    if (bufSize == 0) return;
    
    time_t t = static_cast<time_t>(timestamp);
    struct tm tm{};
    localtime_r(&t, &tm);   // Use IST instead of UTC
    
    size_t written = 0;
    if (range == "1d")
    {
      written = std::strftime(buf, bufSize, "%I:%M %p", &tm);
    }
    else
    {
      written = std::strftime(buf, bufSize, "%Y-%m-%d %I:%M %p", &tm);
    }
    
    // Ensure null-termination in case strftime fails
    if (written == 0)
      buf[0] = '\0';
  }

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
    std::vector<double> xs, opens, highs, lows, closes, volumes;
    xs.reserve(filteredDaysCandles.size());
    opens.reserve(filteredDaysCandles.size());
    highs.reserve(filteredDaysCandles.size());
    lows.reserve(filteredDaysCandles.size());
    closes.reserve(filteredDaysCandles.size());
    volumes.reserve(filteredDaysCandles.size());

    double ymin = DBL_MAX;
    double ymax = -DBL_MAX;
    double maxVolume = 0.0;
    
    for (size_t i = 0; i < filteredDaysCandles.size(); ++i)
    {
      const auto& c = filteredDaysCandles[i];
      
      xs.push_back((double)i);
      opens.push_back(c.open);
      highs.push_back(c.high);
      lows.push_back(c.low);
      closes.push_back(c.close);
      volumes.push_back((double)c.volume);
      
      ymin = std::min(ymin, c.low);
      ymax = std::max(ymax, c.high);
      maxVolume = std::max(maxVolume, (double)c.volume);
    }
    
    // Allocate bottom 22% of chart for volume
    double volBottom = ymin;
    double volTop    = ymin + (ymax - ymin) * 0.22;
    
    // Build scaled volume Y-values
    std::vector<double> volumeY;
    volumeY.reserve(volumes.size());
    for (double v : volumes)
    {
      double t = v / maxVolume;                 // normalize 0–1
      double y = volBottom + t * (volTop - volBottom);
      volumeY.push_back(y);
    }
    
    // --------- X labels (unchanged) ---------
    std::vector<std::string> labelStrings;
    std::vector<const char*> labelPtrs;
    std::vector<double> labelPositions;

    // Decide label step based on number of points: aim for ~8-12 labels max.
    int targetLabels = 10;
    size_t n = filteredDaysCandles.size();
    size_t labelStep = n > 0 ? std::max<size_t>(1, (n + targetLabels - 1) / targetLabels) : 1;
    
    for (size_t i = 0; i < n; i += labelStep)
    {
      char buf[64];
      GetTimeString(buf, 64, filteredDaysCandles[i].timestamp, stockData.range);
      
      labelStrings.emplace_back(buf);
      labelPositions.push_back((double)i);
    }
    
    for (auto& s : labelStrings)
    {
      labelPtrs.push_back(s.c_str());
    }
    
    // Start plotting
    KanVasX::ScopedColor ChartBg(ImGuiCol_WindowBg, Color::Background);
    KanVasX::ScopedColor ChartBorderBg(ImGuiCol_FrameBg, Color::Background);
    KanVasX::ScopedColor ButtonHovered(ImGuiCol_ButtonHovered, Color::Background);
    
    static const auto ChartFlag = ImPlotFlags_NoFrame | ImPlotFlags_NoMenus;
    if (ImPlot::BeginPlot("##StockPlot", ImVec2(ImGui::GetContentRegionAvail().x, 500.0f), ChartFlag))
    {
      ImPlot::SetupAxes("", "", ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoGridLines, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoGridLines);
      ImPlot::SetupAxisLimits(ImAxis_Y1, ymin - 1.0, ymax + 1.0, ImGuiCond_Always);
      
      if (!labelPositions.empty())
      {
        ImPlot::SetupAxisTicks(ImAxis_X1, labelPositions.data(), (int)labelPositions.size(), labelPtrs.data());
      }
      
      // -------- PRICE PLOT --------
      switch (s_plotType)
      {
        case PlotType::Line:
          ShowLinePlot(stockData, xs, closes);
          break;
        case PlotType::Candle:
          ShowCandlePlot(stockData, xs, closes, opens, highs, lows);
          break;
        default:
          break;
      }
      
      // -------- VOLUME PLOT (VISIBLE AT BOTTOM) --------
      ImPlot::SetNextFillStyle(ImVec4(0.3f, 0.6f, 1.0f, 0.35f));
      ImPlot::PlotBars("Volume", xs.data(), volumeY.data(), (int)xs.size(), 0.45);
      
      // -------- TOOLTIP + REFERENCE LINE --------
      ShowTooltip(stockData, filteredDaysCandles);
      ShowReferenceLine(stockData.prevClose, ymin - 1.0, ymax + 1.0, xs, Color::Text);
      
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
    
    // Reference string
    std::string referenceString = "Prev Close: " + KanVest::UI::Utils::FormatDoubleToString(refValue);
    
    // Apply pixel offset (0 right, +10 down)
    pixPos.x += (ImGui::GetContentRegionAvail().x - 1.5 * ImGui::CalcTextSize(referenceString.c_str()).x);
    pixPos.y += 5;
    
    // Draw text manually with no background
    ImDrawList* dl = ImPlot::GetPlotDrawList();
    dl->AddText(Font(Header_24), ImGui::GetFontSize(), pixPos, color, referenceString.c_str());
  }
  
  void Chart::ShowLinePlot(const StockData& stockData, const std::vector<double>& xs, const std::vector<double>& closes)
  {
    double priceChange = stockData.livePrice - stockData.prevClose;
    ImU32 color = priceChange > 0 ? Color::Cyan : Color::Red;
    
    ImVec4 col4 = ImGui::ColorConvertU32ToFloat4(color);
    ImPlot::SetNextLineStyle(col4, 2.0f);
    ImPlot::PlotLine("", xs.data(), closes.data(), static_cast<int>(xs.size()));
  }

  void Chart::ShowCandlePlot(const StockData &stockData,
                             const std::vector<double>& xs,
                             const std::vector<double>& closes,
                             const std::vector<double>& opens,
                             const std::vector<double>& highs,
                             const std::vector<double>& lows)
  {
    // Transparent plot line
    ImVec4 col4 = ImGui::ColorConvertU32ToFloat4(Color::Null);
    ImPlot::SetNextLineStyle(col4, 2.0f);
    ImPlot::PlotLine("", xs.data(), closes.data(), static_cast<int>(xs.size()));

    ImDrawList* drawList = ImPlot::GetPlotDrawList();
    for (size_t i = 0; i < xs.size(); ++i)
    {
      ImU32 color = (closes[i] >= opens[i]) ? Color::Cyan : Color::Red;
      
      ImVec2 pHigh  = ImPlot::PlotToPixels(ImPlotPoint(xs[i], highs[i]));
      ImVec2 pLow   = ImPlot::PlotToPixels(ImPlotPoint(xs[i], lows[i]));
      ImVec2 pOpen  = ImPlot::PlotToPixels(ImPlotPoint(xs[i], opens[i]));
      ImVec2 pClose = ImPlot::PlotToPixels(ImPlotPoint(xs[i], closes[i]));
      
      // Wick
      drawList->AddLine(pLow, pHigh, IM_COL32(200, 200, 200, 255));
      
      float top = std::min(pOpen.y, pClose.y);
      float bottom = std::max(pOpen.y, pClose.y);
      float cx = pOpen.x;
      float width = 4.0f;
      
      // Filled body and border
      drawList->AddRectFilled(ImVec2(cx - width, top), ImVec2(cx + width, bottom), color);
      drawList->AddRect(ImVec2(cx - width, top), ImVec2(cx + width, bottom), IM_COL32(40, 40, 40, 255));
    }
  }
  
  void Chart::ShowTooltip(const StockData& stockData, const std::vector<CandleData>& filteredDaysCandles)
  {
    if (ImPlot::IsPlotHovered())
    {
      ImPlotPoint mouse = ImPlot::GetPlotMousePos();
      
      // Find nearest candle index
      int idx = (int)std::round(mouse.x);
      idx = std::clamp(idx, 0, (int)filteredDaysCandles.size() - 1);
      
      const CandleData& candle = filteredDaysCandles[idx];
      
      char dateTimeBuf[64];
      GetTimeString(dateTimeBuf, 64, filteredDaysCandles[idx].timestamp, stockData.range);
      
      // Draw tooltip near the cursor
      {
        KanVasX::ScopedFont formattedText(Font(FixedWidthHeader_12));
        ImGui::BeginTooltip();
        ImGui::TextColored(ImVec4(1, 0.8f, 0, 1), "%s", dateTimeBuf);
        ImGui::Separator();
        ImGui::Text("Open   : %.2f", candle.open);
        ImGui::Text("High   : %.2f", candle.high);
        ImGui::Text("Low    : %.2f", candle.low);
        ImGui::Text("Close  : %.2f", candle.close);
        ImGui::Text("Volume : %s", UI::Utils::FormatLargeNumber(candle.volume).c_str());
        ImGui::EndTooltip();
      }
    }
  }
} // namespace KanVest
