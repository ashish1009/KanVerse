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
      // UTC + 5 hours 30 minutes (IST)
      return utcTimestamp + 5 * 3600 + 30 * 60;
    }
  } // namespace Utils
  
  // ============================================================
  // 🔧 INITIALIZATION
  // ============================================================
  void StockUI::Initialize(ImTextureID reloadIconID)
  {
    s_reloadIconID = reloadIconID;
  }
  
  // ============================================================
  // 🧩 HELPER — Filter Trading Days
  // ============================================================
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
  
  // ============================================================
  // 📈 DRAW CANDLE CHART
  // ============================================================
  void StockUI::DrawCandleChart(const std::vector<StockPoint>& history)
  {
    if (history.empty())
    {
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_22), "No Chart Available", KanVasX::UI::AlignX::Left);
      return;
    }
    
    std::vector<StockPoint> filtered = FilterTradingDays(history);
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
        
        if (s_showCandle)
        {
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
  
  // ============================================================
  // 📋 DISPLAY STOCK DETAILS (LEFT PANEL)
  // ============================================================
  static void DisplayStockDetails(const StockData& stockData)
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
    
    KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, 0.24, {20.0f, 5.0f});
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
    
    KanVasX::UI::ShiftCursorY(10);
    KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, 0.2, {20.0f, 5.0f});
  }
  
  // ============================================================
  // 🧭 MAIN PANEL — STOCK ANALYZER
  // ============================================================
  void StockUI::StockAnalyzer()
  {
    KanVasX::Panel::Begin("Stock Analyzer");
    
    const float contentRegionAvailX = ImGui::GetContentRegionAvail().x;
    
    static StockData stockData{""};
    static KanVasX::Date startDate{2024, 1, 30};
    static KanVasX::Date endDate{2024, 1, 31};
    static const char* currentRange = "1d";
    static const char* currentInterval = "1m";
    
    static std::unordered_map<std::string, std::vector<std::string>> rangeIntervalMap = {
      {"1d",  {"1m", "2m", "5m", "15m", "30m", "60m"}},
      {"5d",  {"1m", "2m", "5m", "15m", "30m", "60m"}},
      {"1mo", {"5m", "15m", "30m", "60m", "1d"}},
      {"6mo", {"1h", "1d", "1wk"}},
      {"ytd", {"1h", "1wk", "1mo"}},
      {"1y",  {"1d", "1wk", "1mo"}},
      {"5y",  {"1d", "1wk", "1mo"}},
      {"max", {"1d", "1wk", "1mo"}},
    };
    
    auto UpdateStockData = [&](const std::string& symbol)
    {
      time_t startTime = StockParser::ParseDateYYYYMMDD(startDate.ToString());
      time_t endTime = StockParser::ParseDateYYYYMMDD(endDate.ToString());
      if (startTime == 0 || endTime == 0 || endTime <= startTime)
      {
        endTime = time(nullptr);
        startTime = endTime - 60 * 60 * 24 * 90;
      }
      
      long period1 = static_cast<long>(startTime);
      long period2 = static_cast<long>(endTime + 60 * 60 * 24);
      stockData = StockController::UpdateStockData(symbol, std::to_string(period1), std::to_string(period2), currentInterval, currentRange);
    };
    
    // Layout table
    if (ImGui::BeginTable("StockAnalyzerTable", 2, ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingFixedFit))
    {
      float totalWidth = ImGui::GetContentRegionAvail().x;
      float firstColWidth = totalWidth * 0.3f;
      float secondColWidth = totalWidth - firstColWidth;
      
      ImGui::TableSetupColumn("Stock Info", ImGuiTableColumnFlags_WidthFixed, firstColWidth);
      ImGui::TableSetupColumn("Chart", ImGuiTableColumnFlags_WidthFixed, secondColWidth);
      ImGui::TableNextRow();
      
      // COLUMN 1 — Stock Info
      ImGui::TableSetColumnIndex(0);
      static char searchedString[128] = "Nifty";
      static bool firstUpdate = true;
      
      if (firstUpdate)
      {
        UpdateStockData(searchedString);
        firstUpdate = false;
      }
      
      if (KanVasX::Widget::Search(searchedString, 128, KanVasX::Settings::FrameHeight, contentRegionAvailX * 0.278f, "Enter Symbol ...", KanVest::UI::Font::Get(KanVest::UI::FontType::Large), true))
      {
        Utils::ConvertUpper(searchedString);
      }
      
      ImGui::SameLine();
      float iconSize = ImGui::GetItemRectSize().y - 12;
      if (KanVasX::UI::DrawButtonImage("Refresh", s_reloadIconID, false, {iconSize, iconSize}, {-8.0, 6.0}) || ImGui::IsKeyDown(ImGuiKey_Enter))
      {
        UpdateStockData(searchedString);
      }
      
      KanVasX::UI::ShiftCursorY(8.0f);
      if (stockData.IsValid())
        DisplayStockDetails(stockData);
      
      // COLUMN 2 — Chart
      ImGui::TableSetColumnIndex(1);
      if (stockData.IsValid())
      {
        DrawCandleChart(stockData.history);
        
        bool modify = false;
        
        for (auto& rangePair : rangeIntervalMap)
        {
          const std::string& rangeName = rangePair.first;
          ImU32 buttonColor = (rangeName == currentRange) ? KanVasX::Color::BackgroundLight : KanVasX::Color::BackgroundDark;
          if (KanVasX::UI::DrawButton(rangeName.c_str(), nullptr, buttonColor))
          {
            currentRange = rangeName.c_str();
            currentInterval = rangeIntervalMap[currentRange][0].c_str();
            modify = true;
          }
          ImGui::SameLine();
        }
        
        ImGui::Checkbox(" Show Candle", &s_showCandle);
        
        const std::vector<std::string>& intervals = rangeIntervalMap[currentRange];
        for (const std::string& interval : intervals)
        {
          ImU32 buttonColor = (interval == currentInterval) ? KanVasX::Color::BackgroundLight : KanVasX::Color::BackgroundDark;
          if (KanVasX::UI::DrawButton(interval, nullptr, buttonColor))
          {
            currentInterval = interval.c_str();
            modify = true;
          }
          ImGui::SameLine();
        }
        
        if (modify)
          UpdateStockData(stockData.symbol);
      }
      
      ImGui::EndTable();
    }
    
    KanVasX::Panel::End();
  }
  
} // namespace KanVest
