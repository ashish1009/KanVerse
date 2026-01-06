//
//  UI_Chart.cpp
//  KanVest
//
//  Created by Ashish . on 09/12/25.
//

#include "UI_Chart.hpp"

#include "Stock/StockUtils.hpp"
#include "Stock/StockManager.hpp"

#include "Analyzer/StockAnalyzer.hpp"

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

    ShowController(stockData);
    PLotChart(stockData);
  }
  
  void Chart::PLotChart(const StockData &stockData)
  {
    IK_PERFORMANCE_FUNC("Chart::PLotChart");
    
    // Data validity check
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(Font(Header_24), "No Chart available for stock", Align::Left, {20.0f, 10.0f}, Color::Error);
      return;
    }

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

    // Range for volume bar
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
  
    static const auto ChartFlag = ImPlotFlags_NoFrame | ImPlotFlags_NoMenus;
    if (ImPlot::BeginPlot("##StockPlot", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), ChartFlag))
    {
      const double xMin = 0.0;
      const double xMax = (double)xs.size() - 1.0;

      ImPlot::SetupAxes("", "", ImPlotAxisFlags_NoGridLines, ImPlotAxisFlags_NoGridLines);

      ImGuiCond cond = s_stockChanged ? ImGuiCond_Always : ImGuiCond_Once;

      ImPlot::SetupAxisLimits(ImAxis_X1, xMin, xMax, cond);
      ImPlot::SetupAxisLimits(ImAxis_Y1, ymin, ymax, cond);

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

      // Show volume bars
      {
        ImPlotRect limits = ImPlot::GetPlotLimits();
        visibleYMin = limits.Y.Min;
        visibleYMax = limits.Y.Max;
        ShowVolumes(xs, volumeY, opens, closes, volBottom);
      }

      // Helpers
      ShowTooltip(stockData, filteredDaysCandles);
      ShowReferenceLine(stockData.prevClose, ymin, ymax, xs, Color::Text);
      ShowCrossHair(xs, ymin, ymax);

      // Show technicals
      auto ShowTechnical = [xs](const std::string& title, const std::map<int, std::vector<double>>& MA_Data, std::unordered_map<int /* Period */, Indicator_UI_Data>& MA_UI_data)
      {
        for (auto& [period, data] : MA_UI_data)
        {
          if (data.show)
          {
            ShowMAControler(title, data);
            ShowMA(data, MA_Data, xs);
            ImGui::SameLine();
          }
        }
      };
      
      const auto& cursorPos = ImPlot::GetPlotPos();

      ImGui::SetCursorScreenPos({cursorPos.x + 10.0f, cursorPos.y + 10.0f});
      ShowTechnical("DMA", Analyzer::GetDMAValues(), s_DMA_UI_Data);
      ImGui::NewLine();

      ImGui::SetCursorScreenPos({cursorPos.x + 10.0f, cursorPos.y + 40.0f});
      ShowTechnical("EMA", Analyzer::GetEMAValues(), s_EMA_UI_Data);
      
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
  
  void Chart::ShowVolumes(const std::vector<double>& xs, const std::vector<double>& volumeY, const std::vector<double>& opens, const std::vector<double>& closes, double volBottom)
  {
    ImDrawList* dl = ImPlot::GetPlotDrawList();
    for (size_t i = 0; i < xs.size(); i++)
    {
      // Volume color = candle color
      ImU32 color = (closes[i] >= opens[i]) ? Color::Alpha(UI::Utils::StockProfitColor, 0.5f) : Color::Alpha(UI::Utils::StockLossColor, 0.5f);

      // Convert center X to pixels
      ImVec2 pBase   = ImPlot::PlotToPixels(xs[i], volBottom);
      ImVec2 pVolume = ImPlot::PlotToPixels(xs[i], volumeY[i]);

      // Rectangle pixel coords
      ImVec2 a(pBase.x - s_candleWidth, pVolume.y);
      ImVec2 b(pBase.x + s_candleWidth, pBase.y);

      dl->AddRectFilled(a, b, color);
      dl->AddRect(a, b, IM_COL32(40, 40, 40, 255));
    }
  }
  
  void Chart::ShowController(const StockData& stockData)
  {
    if (!stockData.IsValid())
    {
      return;
    }

    // Range controller
    {
      for (const auto& range : API_Provider::GetValidRanges())
      {
        auto buttonColor = range == stockData.range ? KanVasX::Color::BackgroundLight : KanVasX::Color::BackgroundDark;
        std::string uniqueLabel = range + "##Range";

        ImGui::SameLine();
        if (KanVasX::UI::DrawButton(uniqueLabel, nullptr, buttonColor))
        {
          const auto& validInterval = API_Provider::GetValidIntervalForRange(API_Provider::GetRangeFromString(range));
          StockManager::AddStockDataRequest(stockData.symbol, API_Provider::GetRangeFromString(range), validInterval);
        }
      }
    } // Range scope
    
    // Plot type selector
    {
      ImGui::SameLine();
      KanVasX::UI::ShiftCursorX(10.0f);

      int32_t currentPlotType = (int32_t)s_plotType;
      static std::vector<std::string> options = {"Line", "Candle"};

      ImGui::SetNextItemWidth(100.0f);
      if (KanVasX::UI::DropMenu("##ChartType", options, &currentPlotType))
      {
        s_plotType = (PlotType)currentPlotType;
      }
    } // Plot type scope

    // Technicals
    {
      enum class Indicators {None, DMA, EMA};
      
      ImGui::SameLine();
      KanVasX::UI::ShiftCursorX(10.0f);
  
      int32_t currentIndicator = 0; // No need to set the drop menu since we support multiple Indicators
      static std::vector<std::string> options = {"Indicator", "DMA", "EMA"};

      // Get Moving average which are available
      auto GetMovingAveragePeriodIdx = [](const std::unordered_map<int /* Period */, Indicator_UI_Data>& movingAverageUIData)
      {
        // Return detault first period
        if (movingAverageUIData.empty())
        {
          return 0;
        }
        
        // Return next available period
        int idx = 0;
        for (auto period : ValidPeriods)
        {
          auto UI_Data_Itr = movingAverageUIData.find(period);
          if (UI_Data_Itr == movingAverageUIData.end())
          {
            return idx;
          }
          else
          {
            if (UI_Data_Itr->second.show == false)
            {
              return idx;
            }
          }
          idx++;
        }
        
        // Invalid period : All periods exists
        return -1;
      };
      
      ImGui::SetNextItemWidth(100.0f);
      if (KanVasX::UI::DropMenu("##Indicator", options, &currentIndicator))
      {
        auto Fill_MA_UI_Data = [GetMovingAveragePeriodIdx](std::unordered_map<int /* Period */, Indicator_UI_Data>& MA_UI_Data, bool isDMA)
        {
          int periodIdx = GetMovingAveragePeriodIdx(MA_UI_Data);
          if (periodIdx == -1)
          {
            return;
          }
          int period = ValidPeriods.at(periodIdx);
          Indicator_UI_Data& UI_Data = MA_UI_Data[period];
          
          UI_Data.show = true;
          UI_Data.periodIdx = periodIdx;
          UI_Data.period = period;
          
          float typeOffset = isDMA * 0.37f;   // isDMA = 0 (EMA), 1 (DMA)
          UI_Data.color = {
            0.25f + 0.75f * fmod(period * 0.37f + typeOffset, 1.0f),
            0.25f + 0.75f * fmod(period * 0.61f + typeOffset, 1.0f),
            0.25f + 0.75f * fmod(period * 0.83f + typeOffset, 1.0f),
            1.0f
          };
        };
        
        switch ((Indicators)currentIndicator)
        {
          case Indicators::DMA:
          {
            Fill_MA_UI_Data(s_DMA_UI_Data, true);
            break;
          }
          case Indicators::EMA:
          {
            Fill_MA_UI_Data(s_EMA_UI_Data, false);
            break;
          }

          default:
            break;
        }
      }
    } // Technical Scope
    
    // Interval Controller
    {
      ImGui::SameLine();
      float availX = ImGui::GetContentRegionAvail().x;
      const auto& possibleIntervals = API_Provider::GetValidIntervalsStringForRange(stockData.range);
      KanVasX::UI::ShiftCursorX(availX - possibleIntervals.size() * 40.0f);
      {
        for (const auto& interval : possibleIntervals)
        {
          auto buttonColor = interval == stockData.dataGranularity ? KanVasX::Color::BackgroundLight : KanVasX::Color::BackgroundDark;
          std::string uniqueLabel = interval + "##Interval";
          
          if (KanVasX::UI::DrawButton(uniqueLabel, nullptr, buttonColor))
          {
            StockManager::AddStockDataRequest(stockData.symbol, API_Provider::GetRangeFromString(stockData.range), API_Provider::GetIntervalFromString(interval));
          }
          ImGui::SameLine();
        }
        ImGui::NewLine();
      }
    } // Interval Scope
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

  void Chart::ShowCrossHair(const std::vector<double>& xs, double ymin, double ymax)
  {
    if (!ImPlot::IsPlotHovered())
      return;

    ImDrawList* drawList = ImPlot::GetPlotDrawList();
    ImPlotPoint mouse = ImPlot::GetPlotMousePos();

    // Snap X to nearest candle index
    int idx = (int)std::round(mouse.x);
    idx = std::clamp(idx, 0, (int)xs.size() - 1);
    double snapX = xs[idx];

    // Convert plot coords -> pixels
    ImVec2 pMin = ImPlot::PlotToPixels(snapX, ymin);
    ImVec2 pMax = ImPlot::PlotToPixels(snapX, ymax);
    ImVec2 hMin = ImPlot::PlotToPixels(xs.front(), mouse.y);
    ImVec2 hMax = ImPlot::PlotToPixels(xs.back(),  mouse.y);

    ImU32 color = IM_COL32(200, 200, 200, 120);

    // Vertical line
    drawList->AddLine(pMin, pMax, color, 1.0f);

    // Horizontal line
    drawList->AddLine(hMin, hMax, color, 1.0f);
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
        ImU32 color = (candle.close >= candle.open) ? UI::Utils::StockProfitColor : UI::Utils::StockLossColor;
        KanVasX::ScopedFont formattedText(Font(FixedWidthHeader_12));

        ImGui::BeginTooltip();
        ImGui::TextColored(ImVec4(1, 0.8f, 0, 1), "%s", dateTimeBuf);
        ImGui::Separator();

        auto showOCHL = [color](const std::string& value)
        {
          KanVasX::ScopedColor textColor(ImGuiCol_Text, color);
          ImGui::SameLine(); ImGui::Text("%s", value.c_str());
        };

        ImGui::Text("Open   : "); showOCHL(UI::Utils::FormatDoubleToString(candle.open));
        ImGui::Text("Close  : "); showOCHL(UI::Utils::FormatDoubleToString(candle.close));
        ImGui::Text("High   : "); showOCHL(UI::Utils::FormatDoubleToString(candle.high));
        ImGui::Text("Low    : "); showOCHL(UI::Utils::FormatDoubleToString(candle.low));

        ImGui::Separator();
        ImGui::TextColored(ImVec4(1, 0.8f, 0, 1), "Volume : %s", UI::Utils::FormatLargeNumber(candle.volume).c_str());

        ImGui::EndTooltip();
      }
    }
  }
  
  void Chart::ShowMAControler(const std::string& title, Indicator_UI_Data& data)
  {
    std::string id = title + " " + std::to_string(data.period);
    ImGui::PushID(id.c_str());
    
    // Rectangle
    {
      KanVasX::UI::DrawFilledRect(Color::BackgroundLight, 25.0f, 0.082);
    }
  
    // Cross Button
    {
      if (KanVasX::UI::DrawButton("X", Font(Bold), Color::BackgroundLight, Color::DarkRed, false, 0.0f, {25.0f, 25.0f}))
      {
        data.show = false;
      }
    }

    // Title
    {
      ImGui::SameLine();
      KanVasX::UI::Text(Font(FixedWidthHeader_12), title, Align::Left, {0.0f, 4.0f});
    }
    
    // Period
    {
      static std::vector<std::string> possibleMAPeriods = {"5", "10", "20", "30", "50", "100", "150", "200"};

      ImGui::SameLine();
      ImGui::SetNextItemWidth(50.0f);

      std::string periodID = "##DMAPeriod" + title;
      if (KanVasX::UI::DropMenu(periodID.c_str(), possibleMAPeriods, &data.periodIdx))
      {
//        if (title == "DMA")       s_DMAPeriod = std::stoi(possibleMAPeriods[s_DMAPeriodIdx]);
//        else if (title == "EMA")  s_EMAPeriod = std::stoi(possibleMAPeriods[s_EMAPeriodIdx]);
      }
    }
  
    // Color
    {
      ImGui::SameLine();
      KanVasX::UI::ShiftCursorY(4.0f);

      ImVec4 col = { data.color.r, data.color.g, data.color.b, data.color.a };
      if (ImGui::ColorButton("##MAColor", col, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(14, 14)))
      {
        ImGui::OpenPopup("MA_Color_Popup");
      }

      if (ImGui::BeginPopup("MA_Color_Popup"))
      {
        ImGui::ColorPicker4("MA Color", &data.color.r, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
        ImGui::EndPopup();
      }
    }
    ImGui::PopID();
  }
  
  void Chart::ShowMA(const Indicator_UI_Data& MA_UI_Data, const std::map<int, std::vector<double>>& MA_Data, const std::vector<double> &xs)
  {
    if (auto itr = MA_Data.find(MA_UI_Data.period); itr != MA_Data.end())
    {
      ImVec4 col4 = {MA_UI_Data.color.r, MA_UI_Data.color.g, MA_UI_Data.color.b, MA_UI_Data.color.a};
      ImPlot::SetNextLineStyle(col4, 2.0f);
      ImPlot::PlotLine("", xs.data(), itr->second.data(), static_cast<int>(xs.size()));
    }
  }
} // namespace KanVest
