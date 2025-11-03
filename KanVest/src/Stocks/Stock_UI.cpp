//
//  Stock_UI.cpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#include "Stock_UI.hpp"

#include "Stocks/StockController.hpp"
#include "Stocks/StockAPI.hpp"
#include "Stocks/StockParser.hpp"

namespace KanVest
{
  static char s_searchedString[128] = "Nifty";

#if 0
  static KanVasX::Date startDate{2024, 1, 30};
  static KanVasX::Date endDate{2024, 1, 31};
#endif
  
  namespace Utils
  {
    void ConvertUpper(char* string)
    {
      for (char* p = string; *p; ++p)
        *p = static_cast<char>(toupper(*p));
    }
    
    std::string FormatDoubleToString(double value)
    {
      char buf[32];
      std::snprintf(buf, sizeof(buf), "%.2f", value);
      return std::string(buf);
    }
    
    std::string FormatLargeNumber(double value)
    {
      const double billion = 1e9, million = 1e6, thousand = 1e3;
      std::ostringstream oss;
      oss << std::fixed << std::setprecision(2);

      if (value >= billion)
        oss << (value / billion) << "B";
      else if (value >= million)
        oss << (value / million) << "M";
      else if (value >= thousand)
        oss << (value / thousand) << "K";
      else
        oss << value;

      return oss.str();
    }

    static inline time_t ToMarketTime(time_t utcTimestamp)
    {
      return utcTimestamp + 5 * 3600 + 30 * 60;
    }
    
    static std::vector<StockPoint> FilterTradingDays(const std::vector<StockPoint>& history)
    {
      std::vector<StockPoint> filtered;
      filtered.reserve(history.size());

      for (const auto& h : history)
      {
        time_t t = static_cast<time_t>(h.timestamp);
        struct tm* tm_info = localtime(&t);
        int wday = tm_info->tm_wday; // 0 = Sunday, 6 = Saturday
        if (wday != 0 && wday != 6)
          filtered.push_back(h);
      }

      return filtered;
    }

  } // namespace Utils
  
  void StockUI::Initialize(ImTextureID reloadIconID)
  {
    s_reloadIconID = reloadIconID;    
    UpdateStockData(s_searchedString);
  }
  
  void StockUI::UpdateStockData(const std::string& symbol)
  {
#if 0
    time_t startTime = StockParser::ParseDateYYYYMMDD(startDate.ToString());
    time_t endTime = StockParser::ParseDateYYYYMMDD(endDate.ToString());
    if (startTime == 0 || endTime == 0 || endTime <= startTime)
    {
      endTime = time(nullptr);
      startTime = endTime - 60 * 60 * 24 * 90;
    }

    long period1 = static_cast<long>(startTime);
    long period2 = static_cast<long>(endTime + 60 * 60 * 24);
#endif
    // Helper to convert & format timestamp -> readable
    auto CurrentTimeString = []() -> std::string
    {
      time_t now = time(nullptr);
      char buf[128];
      strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S", localtime(&now));
      return std::string(buf);
    };

    s_lastUpdatedString = CurrentTimeString();
    s_lastUpdateTime = ImGui::GetTime();

    // Update Stock Data
    StockController::UpdateStockData(symbol);
  };

  void StockUI::StockPanel()
  {
    KanVasX::Panel::Begin("Stock Analyzer");
    
    ShowStockData();
    
    KanVasX::UI::ShiftCursorY(4.0f);
    ShowStockAnalyzeData();
    
    KanVasX::Panel::End();
  }
  
  void StockUI::SearchBar()
  {
    KanVasX::UI::DrawFilledRect(KanVasX::Color::FrameBg, 40, 0.2985);
    const float contentRegionAvailX = ImGui::GetContentRegionAvail().x;
    if (KanVasX::Widget::Search(s_searchedString, 128, KanVasX::Settings::FrameHeight, contentRegionAvailX * 0.92f, "Enter Symbol ...", UI::Font::Get(UI::FontType::Large), true))
    {
      Utils::ConvertUpper(s_searchedString);
    }
    
    ImGui::SameLine();
    float iconSize = ImGui::GetItemRectSize().y - 12;
    if (KanVasX::UI::DrawButtonImage("Refresh", s_reloadIconID, false, {iconSize, iconSize}, {0.0, 6.0}) || ImGui::IsKeyDown(ImGuiKey_Enter))
    {
      UpdateStockData(s_searchedString);
    }
  }
  
  void StockUI::ShowStcokBasicData(const StockData& stockData)
  {
    const ImU32 textColor = KanVasX::Color::TextBright;
    const auto alignLeft = KanVasX::UI::AlignX::Left;

    // Name & price
    std::string name = stockData.shortName + " (" + stockData.currency + ")";
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_36), name.c_str(), alignLeft, {30.0f, 10.0f}, textColor);
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_24), stockData.longName.c_str(), alignLeft, {30.0f, 0.0f}, textColor);
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_48), Utils::FormatDoubleToString(stockData.livePrice), alignLeft, {30.0f, 10.0f}, textColor);
    
    // Change
    std::string change = (stockData.change > 0 ? "+" : "") +
    Utils::FormatDoubleToString(stockData.change) +
    (stockData.change > 0 ? " ( +" : " ( ") +
    Utils::FormatDoubleToString(stockData.changePercent) + "%)";
    
    ImU32 changeColor = stockData.change > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_30), change, alignLeft, {30.0f, 10.0f}, changeColor);

    static const float UnderLineSize = 0.28;
    KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, UnderLineSize, {20.0f, 5.0f});
    KanVasX::UI::ShiftCursorY(10);

    // 52-Week Range
    std::string fiftyTwoWeek = Utils::FormatDoubleToString(stockData.fiftyTwoLow) + " - " + Utils::FormatDoubleToString(stockData.fiftyTwoHigh);
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::FixedWidthHeader_20), "52-Week Range ", alignLeft, {30.0f, 10.0f}, textColor);
    ImGui::SameLine();
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::FixedWidthHeader_20), fiftyTwoWeek, alignLeft, {50.0f, 0.0f}, textColor);

    // Day Range
    std::string dayRange = Utils::FormatDoubleToString(stockData.dayLow) + " - " + Utils::FormatDoubleToString(stockData.dayHigh);
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::FixedWidthHeader_20), "Day Range     ", alignLeft, {30.0f, 10.0f}, textColor);
    ImGui::SameLine();
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::FixedWidthHeader_20), dayRange, alignLeft, {50.0f, 0.0f}, textColor);

    // Volume
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::FixedWidthHeader_20), "Volume        ", alignLeft, {30.0f, 10.0f}, textColor);
    ImGui::SameLine();
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::FixedWidthHeader_20), Utils::FormatLargeNumber(stockData.volume), alignLeft, {50.0f, 0.0f}, textColor);

    KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, UnderLineSize, {20.0f, 5.0f});
    KanVasX::UI::ShiftCursorY(10);
  }
  
  void StockUI::DrawCandleChart(const StockData& stockData)
  {
    const auto& history = stockData.history;
    
    if (history.empty())
    {
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_22), "No Chart Available", KanVasX::UI::AlignX::Left);
      return;
    }

    std::vector<StockPoint> filtered = Utils::FilterTradingDays(history);
    if (filtered.empty())
    {
      ImGui::Text("No trading days in this range");
      return;
    }

    std::vector<double> xs, opens, highs, lows, closes;
    xs.reserve(filtered.size());
    opens.reserve(filtered.size());
    highs.reserve(filtered.size());
    lows.reserve(filtered.size());
    closes.reserve(filtered.size());

    double ymin = DBL_MAX;
    double ymax = -DBL_MAX;

    for (const auto& h : filtered)
    {
      xs.push_back(static_cast<double>(Utils::ToMarketTime(h.timestamp)));
      opens.push_back(h.open);
      highs.push_back(h.high);
      lows.push_back(h.low);
      closes.push_back(h.close);
      ymin = std::min(ymin, h.low);
      ymax = std::max(ymax, h.high);
    }

    if (ImPlot::BeginPlot("", ImVec2(-1, 400)))
    {
      ImPlot::SetupAxes("", "", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
      ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
      ImPlot::SetupAxisLimits(ImAxis_Y1, ymin - 1, ymax + 1, ImGuiCond_Always);

      ImPlot::PlotLine("", xs.data(), closes.data(), static_cast<int>(xs.size()));
      ImDrawList* drawList = ImPlot::GetPlotDrawList();

      if (s_showCandle)
      {
        for (size_t i = 0; i < xs.size(); ++i)
        {
          ImU32 color = (closes[i] >= opens[i])
          ? IM_COL32(0, 255, 0, 255)
          : IM_COL32(255, 80, 80, 255);
          
          ImVec2 pHigh  = ImPlot::PlotToPixels(ImPlotPoint(xs[i], highs[i]));
          ImVec2 pLow   = ImPlot::PlotToPixels(ImPlotPoint(xs[i], lows[i]));
          ImVec2 pOpen  = ImPlot::PlotToPixels(ImPlotPoint(xs[i], opens[i]));
          ImVec2 pClose = ImPlot::PlotToPixels(ImPlotPoint(xs[i], closes[i]));
          
          drawList->AddLine(pLow, pHigh, IM_COL32(200, 200, 200, 255));
          
          float top = std::min(pOpen.y, pClose.y);
          float bottom = std::max(pOpen.y, pClose.y);
          float cx = pOpen.x;
          float width = 4.0f;
          
          drawList->AddRectFilled(ImVec2(cx - width, top), ImVec2(cx + width, bottom), color);
          drawList->AddRect(ImVec2(cx - width, top), ImVec2(cx + width, bottom), IM_COL32(40, 40, 40, 255));
        }
      }

      ImPlot::EndPlot();
    }
  }
  
  void StockUI::DrawChartController(const StockData &stockData)
  {
    if (stockData.IsValid())
    {
      bool modify = false;
      for (int i = 0; i < IM_ARRAYSIZE(StockController::ValidRange); ++i)
      {
        auto buttonColor = StockController::ValidRange[i] == StockController::GetCurrentRange() ? KanVasX::Color::BackgroundLight : KanVasX::Color::BackgroundDark;
        if (KanVasX::UI::DrawButton(StockController::ValidRange[i], nullptr, buttonColor))
        {
          StockController::SetCurrentRange(StockController::ValidRange[i]);
          StockController::SetCurrentInterval(StockController::RangeIntervalMap[StockController::GetCurrentRange()][0].c_str());
          
          modify = true;
        }
        if (i < IM_ARRAYSIZE(StockController::ValidRange) - 1)
        {
          ImGui::SameLine();
        }
      }

      ImGui::SameLine();
      KanVasX::UI::ShiftCursorX(20);
      ImGui::Checkbox(" Show Candle", &s_showCandle);
      
      ImGui::SameLine();
      KanVasX::UI::ShiftCursorX(20);

      float refreshInterval = StockController::GetRefreshInterval();
      if (ImGui::SmallButton("-"))
      {
        refreshInterval = std::max(1.0f, refreshInterval - 5.0f);
        StockController::SetRefreshInterval(refreshInterval);
      }

      ImGui::SameLine();
      KanVasX::UI::ShiftCursorX(20);
      ImGui::Text("Refresh Time Interval %.1f s", StockController::GetRefreshInterval());
      ImGui::SameLine();
      KanVasX::UI::ShiftCursorX(20);

      if (ImGui::SmallButton("+"))
      {
        refreshInterval += 5.0f;
        StockController::SetRefreshInterval(refreshInterval);
      }

      ImGui::SameLine();
      std::vector<std::string> intervalValues = StockController::RangeIntervalMap[StockController::GetCurrentRange()];
      KanVasX::UI::ShiftCursorX(ImGui::GetContentRegionAvail().x - (intervalValues.size() * 40));
      for (int i = 0; i < intervalValues.size(); ++i)
      {
        auto buttonColor = intervalValues[i] == StockController::GetCurrentInterval() ? KanVasX::Color::BackgroundLight : KanVasX::Color::BackgroundDark;
        if (KanVasX::UI::DrawButton(intervalValues[i], nullptr, buttonColor))
        {
          StockController::SetCurrentInterval(intervalValues[i].c_str());
          modify = true;
        }
        if (i < intervalValues.size())
        {
          ImGui::SameLine();
        }
      }
      
      if (modify)
      {
        UpdateStockData(stockData.symbol);
      }
    }
  }
  
  void StockUI::ShowStockData()
  {
    // Auto Update
    double currentTime = ImGui::GetTime();
    float timeSinceUpdate = static_cast<float>(currentTime - s_lastUpdateTime);
    if (timeSinceUpdate >= StockController::GetRefreshInterval())
    {
      UpdateStockData(StockController::GetActiveStockData().symbol);
    }

    if (ImGui::BeginTable("StockAnalyzerTable", 2, ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingFixedFit))
    {
      float topYArea = ImGui::GetContentRegionAvail().y * 0.45f;
      float totalWidth = ImGui::GetContentRegionAvail().x;
      float firstColWidth = totalWidth * 0.3f;
      float secondColWidth = totalWidth  - firstColWidth;
      
      ImGui::TableSetupColumn("Stock Info", ImGuiTableColumnFlags_WidthFixed, firstColWidth);
      ImGui::TableSetupColumn("Chart", ImGuiTableColumnFlags_WidthFixed, secondColWidth);
      
      ImGui::TableNextRow();
      
      const StockData& stockData = StockController::GetActiveStockData();
      // Column 1 Stock Details
      {
        ImGui::TableSetColumnIndex(0);
        KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundDark, topYArea, 0.2985);
      
        SearchBar();
        ShowStcokBasicData(stockData);
      }
      
      // Column 2 Chart
      {
        ImGui::TableSetColumnIndex(1);
        KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundDark, topYArea, 0.688);
        
        DrawCandleChart(stockData);
        DrawChartController(stockData);
      }
    }
    ImGui::EndTable();
  }
  
  void StockUI::ShowStockAnalyzeData()
  {
    if (ImGui::BeginTable("Portfolio Manager", 2, ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingFixedFit))
    {
      // UI
      float topYArea = ImGui::GetContentRegionAvail().y;
      float totalWidth = ImGui::GetContentRegionAvail().x;
      float firstColWidth = totalWidth * 0.3f;
      float secondColWidth = totalWidth  - firstColWidth;
      
      ImGui::TableSetupColumn(" Stock Analyzer", ImGuiTableColumnFlags_WidthFixed, firstColWidth);
      ImGui::TableSetupColumn(" DATA 2", ImGuiTableColumnFlags_WidthFixed, secondColWidth);
      
      ImGui::TableNextRow();
      
      {
        ImGui::TableSetColumnIndex(0);
        KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundDark, topYArea, 0.2985);
        
        StockSummary summary = StockController::GetStockSummary();

        ImGui::Text("trend : %s", summary.trend.c_str());
        ImGui::Text("momentum : %s", summary.momentum.c_str() );
        ImGui::Text("volatility : %s", summary.volatility.c_str() );
        ImGui::Text("volume : %s", summary.volume.c_str() );
        ImGui::Text("valuation : %s", summary.valuation.c_str() );
        ImGui::Text("vwapBias : %s", summary.vwapBias.c_str() );
        ImGui::Text("conclusion : %s", summary.conclusion.c_str());
        ImGui::Text("score : %f", summary.score);
        ImGui::Text("suggestion : %s", summary.suggestion.c_str());

      }
      
      ImGui::TableSetColumnIndex(1);
      KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundDark, topYArea, 0.688);
    }
    ImGui::EndTable();
  }
} // namespace KanVest
