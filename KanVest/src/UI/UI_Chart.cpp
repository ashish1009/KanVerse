//
//  UI_Chart.cpp
//  KanVest
//
//  Created by Ashish . on 12/01/26.
//

#include "UI_Chart.hpp"

#include "UI/UI_Utils.hpp"

#include "Stock/StockUtils.hpp"

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
    localtime_r(&t, &tm);
    
    size_t written = 0;
    if (range == "1d")
      written = std::strftime(buf, bufSize, "%I:%M %p", &tm);
    else
      written = std::strftime(buf, bufSize, "%Y-%m-%d %I:%M %p", &tm);
    
    if (written == 0)
      buf[0] = '\0';
  }

  void Chart::Show(const StockData &stockData)
  {
    IK_PERFORMANCE_FUNC("Chart::Show");
    
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(Font(Header_24), "No Chart available for stock", Align::Left, {20.0f, 10.0f}, Color::Error);
      return;
    }
    
    ShowController(stockData);
    PLotChart(stockData);
  }
  
  void Chart::ShowController(const StockData& stockData)
  {
    if (!stockData.IsValid())
    {
      return;
    }
    
    // Button Setting
    static constexpr ImVec2 buttonSize = {50, 30};
    static constexpr float buttonRounding = 10.0f;
    
    // Range controller -------------------------------------------------------------------
    for (const auto& range : API_Provider::GetValidRangesString())
    {
      auto buttonColor = range == stockData.range ? KanVasX::Color::Button : KanVasX::Color::BackgroundDark;
      auto textColor = range == stockData.range ? KanVasX::Color::Text : KanVasX::Color::TextMuted;

      std::string uniqueLabel = range + "##Range";
      if (KanVasX::UI::DrawButton(uniqueLabel, nullptr, buttonColor, textColor, false, buttonRounding, buttonSize))
      {
        Range currentRange = API_Provider::GetRangeEnumFromString(range);
        Interval optimalInterval = API_Provider::GetOptimalIntervalForRange(currentRange);
        
        StockManager::AddStockDataRequest(stockData.symbol, currentRange, optimalInterval);
      }
      ImGui::SameLine();
    }
    
    // Plot type selector -------------------------------------------------------------------
    ImGui::SameLine();
    KanVasX::UI::ShiftCursor({20.0f, 5.0f});
    
    int32_t currentPlotType = (int32_t)s_plotType;
    static std::vector<std::string> options = {"Line", "Candle"};
    
    ImGui::SetNextItemWidth(100.0f);
    if (KanVasX::UI::DropMenu("##ChartType", options, &currentPlotType))
    {
      s_plotType = (PlotType)currentPlotType;
    }

    // Interval Controller -------------------------------------------------------------------
    ImGui::SameLine();

    const auto& possibleIntervals = API_Provider::GetValidIntervalsStringForRangeString(stockData.range);
    KanVasX::UI::ShiftCursorX(ImGui::GetContentRegionAvail().x - possibleIntervals.size() * 60.0f);

    for (const auto& interval : possibleIntervals)
    {
      auto buttonColor = interval == stockData.dataGranularity ? KanVasX::Color::BackgroundLight : KanVasX::Color::BackgroundDark;
      auto textColor = interval == stockData.dataGranularity ? KanVasX::Color::Text : KanVasX::Color::TextMuted;

      std::string uniqueLabel = interval + "##Interval";
      if (KanVasX::UI::DrawButton(uniqueLabel, nullptr, buttonColor, textColor, false, buttonRounding, buttonSize))
      {
        Range currentRange = API_Provider::GetRangeEnumFromString(stockData.range);
        Interval currentInterval = API_Provider::GetIntervalEnumFromString(interval);

        StockManager::AddStockDataRequest(stockData.symbol, currentRange, currentInterval);
      }
      ImGui::SameLine();
    }
    ImGui::NewLine();
  }
  
  void Chart::PLotChart(const StockData &stockData)
  {
    // Check valid data
    if (!stockData.IsValid())
    {
      return;
    }
    
    // Scoped button color
    KanVasX::ScopedColor ButtonColor(ImGuiCol_Button, Color::Null);
    KanVasX::ScopedColor ButtonHoveredColor(ImGuiCol_ButtonHovered, Color::Null);
    
    // Update if stock is changed
    s_stockChanged = s_lastSymbol != stockData.symbol || s_lastRange != stockData.range || s_lastInterval != stockData.dataGranularity;
    if (s_stockChanged)
    {
      s_lastSymbol   = stockData.symbol;
      s_lastRange    = stockData.range;
      s_lastInterval = stockData.dataGranularity;
    }

    // Get candle data
    const auto& candleHistory = stockData.candleHistory;
    std::vector<CandleData> filteredDaysCandles = Utils::FilterTradingDays(candleHistory);

    // Axis data vector
    std::vector<double> xs, opens, highs, lows, closes, volumes;

    // Limit range for price and volume
    double ymin = DBL_MAX;
    double ymax = -DBL_MAX;
    double maxVolume = 1.0;

    // Store data for each plot volume and price
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

    // Shift Y axis to cover previous price in chart in case of gap opening
    ymin = std::min(ymin, stockData.prevClose);
    ymax = std::max(ymax, stockData.prevClose);
    
    // Range for volume bar --------------------------------
    static double visibleYMin = 0.0f;
    static double visibleYMax = 0.0f;

    double volBottom = visibleYMin;
    double volTop = visibleYMin + (visibleYMax - visibleYMin) * 0.22;

    std::vector<double> volumeY;
    for (double v : volumes)
    {
      volumeY.push_back(volBottom + (v / maxVolume) * (volTop - volBottom));
    }

    // Label string vector
    std::vector<std::string> labelStrings;
    std::vector<const char*> labelPtrs;
    std::vector<double> labelPositions;
    
    // Label string limit
    const int targetLabels = 10;
    const size_t n = filteredDaysCandles.size();
    const size_t labelStep = std::max<size_t>(1, (n + targetLabels - 1) / targetLabels);
    
    for (size_t i = 0; i < n; i += labelStep)
    {
      char buf[64];
      GetTimeString(buf, 64, filteredDaysCandles[i].timestamp, stockData.range);
      labelStrings.emplace_back(buf);
      labelPositions.push_back((double)i);
    }
    
    // Label string as char*
    for (auto& s : labelStrings)
    {
      labelPtrs.push_back(s.c_str());
    }

    // Plot chart
    static const auto ChartFlag = ImPlotFlags_NoFrame | ImPlotFlags_NoMenus;
    if (ImPlot::BeginPlot("##StockPlot", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5f), ChartFlag))
    {
      const double xMin = 0.0;
      const double xMax = (double)xs.size() - 1.0;

      ImPlot::SetupAxes("", "", ImPlotAxisFlags_NoGridLines, ImPlotAxisFlags_NoGridLines);

      ImGuiCond cond = s_stockChanged ? ImGuiCond_Always : ImGuiCond_Once;
      
      ImPlot::SetupAxisLimits(ImAxis_X1, xMin, xMax, cond);
      ImPlot::SetupAxisLimits(ImAxis_Y1, ymin, ymax, ImGuiCond_Always);
      
      ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, xMin, xMax);
      ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, ymin, ymax);

      if (!labelPositions.empty())
      {
        ImPlot::SetupAxisTicks(ImAxis_X1, labelPositions.data(), (int)labelPositions.size(), labelPtrs.data());
      }

      // Compute candle width based on zoom size
      ComputeCandleWidth(xs);

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

      ImPlot::EndPlot();
    }
  }
  
  void Chart::ComputeCandleWidth(const std::vector<double>& xs)
  {
    if (xs.size() < 2)
    {
      s_candleWidth = 4.0f;
      return;
    }
    
    // Measure actual pixel spacing between two adjacent candles
    ImVec2 p0 = ImPlot::PlotToPixels(xs[0], 0.0);
    ImVec2 p1 = ImPlot::PlotToPixels(xs[1], 0.0);
    
    float pixelSpacing = fabsf(p1.x - p0.x);
    
    // Candle width = fraction of spacing
    s_candleWidth = pixelSpacing * 0.35f;
    
    // Hard safety clamp
    s_candleWidth = ImClamp(s_candleWidth, 1.0f, 20.0f);
  }
  
  void Chart::ShowLinePlot(const StockData& stockData, const std::vector<double>& xs, const std::vector<double>& closes)
  {
    double priceChange = stockData.livePrice - stockData.prevClose;
    ImU32 color = priceChange > 0 ? UI::Utils::StockProfitColor : UI::Utils::StockLossColor;
    
    ImVec4 col4 = ImGui::ColorConvertU32ToFloat4(color);
    ImPlot::SetNextLineStyle(col4, 2.0f);
    ImPlot::PlotLine("", xs.data(), closes.data(), static_cast<int>(xs.size()));
  }
  
  void Chart::ShowCandlePlot(const StockData&, const std::vector<double>& xs, const std::vector<double>& closes, const std::vector<double>& opens,
                             const std::vector<double>& highs, const std::vector<double>& lows)
  {
    ImVec4 col4 = ImGui::ColorConvertU32ToFloat4(Color::Null);
    ImPlot::SetNextLineStyle(col4, 2.0f);
    ImPlot::PlotLine("", xs.data(), closes.data(), (int)xs.size());
    
    ImDrawList* dl = ImPlot::GetPlotDrawList();

    ImPlotRect plot = ImPlot::GetPlotLimits();
    ImVec2 plotMin = ImPlot::PlotToPixels(plot.Min());
    ImVec2 plotMax = ImPlot::PlotToPixels(plot.Max());

    for (size_t i = 0; i < xs.size(); ++i)
    {
      ImU32 color = (closes[i] >= opens[i]) ? UI::Utils::StockProfitColor : UI::Utils::StockLossColor;
      
      ImVec2 pHigh  = ImPlot::PlotToPixels(xs[i], highs[i]);
      ImVec2 pLow   = ImPlot::PlotToPixels(xs[i], lows[i]);
      ImVec2 pOpen  = ImPlot::PlotToPixels(xs[i], opens[i]);
      ImVec2 pClose = ImPlot::PlotToPixels(xs[i], closes[i]);
      
      dl->AddLine(pLow, pHigh, IM_COL32(200,200,200,255));
      
      float left  = pOpen.x - s_candleWidth;
      float right = pOpen.x + s_candleWidth;
      
      if (right < plotMin.x || left > plotMax.x)
        continue;
      
      left  = std::max(left,  plotMin.x);
      right = std::min(right, plotMax.x);
      
      float top = std::min(pOpen.y, pClose.y);
      float bot = std::max(pOpen.y, pClose.y);
      
      dl->AddRectFilled(ImVec2(left, top), ImVec2(right, bot), color);
      dl->AddRect(ImVec2(left, top), ImVec2(right, bot), IM_COL32(40,40,40,255));
    }
  }
} // namespace KanVest
