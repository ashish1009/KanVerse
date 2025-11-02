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
      {
        *p = (char)toupper(*p);
      }
    }
    
    std::string FormatDoubleToString(double value)
    {
      char buf[32];
      std::snprintf(buf, sizeof(buf), "%.2f", value);
      return std::string(buf);
    };
    
    std::string FormatLargeNumber(double value)
    {
      const double billion = 1'000'000'000.0;
      const double million = 1'000'000.0;
      const double thousand = 1'000.0;
      
      std::ostringstream oss;
      oss << std::fixed << std::setprecision(2); // show 2 decimals max
      
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
      // IST = UTC + 5 hours 30 minutes = +19800 seconds
      return utcTimestamp + 5 * 3600 + 30 * 60;
    }
  } // namespace Utils
  
  void StockUI::Initialize(ImTextureID reloadIconID)
  {
    s_reloadIconID = reloadIconID;
  }
  
  void StockUI::DrawCandleChart(const std::vector<StockPoint>& history)
  {
    if (history.empty())
    {
      KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_22), "No Chart Available", KanVasX::UI::AlignX::Left);
      return;
    }
    
    // Filter out non-trading days (weekends/holidays)
    std::vector<StockPoint> filtered;
    filtered.reserve(history.size());
    
    for (const auto& h : history)
    {
      time_t t = (time_t)h.timestamp;
      struct tm* tm_info = localtime(&t);
      int wday = tm_info->tm_wday; // 0 = Sunday, 6 = Saturday
      if (wday == 0 || wday == 6) continue; // skip weekends
      // (optional) add your holiday skip logic here
      filtered.push_back(h);
    }
    
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
    
    double ymin = DBL_MAX, ymax = -DBL_MAX;
    for (auto& h : filtered)
    {
      xs.push_back((double)Utils::ToMarketTime(h.timestamp));
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
      ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time); // ⏰ real-time axis!
      ImPlot::SetupAxisLimits(ImAxis_Y1, ymin - 1, ymax + 1, ImGuiCond_Always);
      
      // Invisible plot for transform
      ImPlot::PlotLine("", xs.data(), closes.data(), (int)xs.size());
      
      ImDrawList* draw_list = ImPlot::GetPlotDrawList();
      
      for (size_t i = 0; i < xs.size(); ++i)
      {
        double x = xs[i];
        double o = opens[i];
        double c = closes[i];
        double h = highs[i];
        double l = lows[i];
        
        ImU32 color = (c >= o) ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 80, 80, 255);
        
        ImVec2 p_high  = ImPlot::PlotToPixels(ImPlotPoint(x, h));
        ImVec2 p_low   = ImPlot::PlotToPixels(ImPlotPoint(x, l));
        ImVec2 p_open  = ImPlot::PlotToPixels(ImPlotPoint(x, o));
        ImVec2 p_close = ImPlot::PlotToPixels(ImPlotPoint(x, c));
        
        // Wick
        draw_list->AddLine(p_low, p_high, IM_COL32(200, 200, 200, 255));
        
        // Candle body
        float top = std::min(p_open.y, p_close.y);
        float bottom = std::max(p_open.y, p_close.y);
        float cx = p_open.x;
        float w = 4.0f;
        
        if (s_showCandle)
        {
          draw_list->AddRectFilled(ImVec2(cx - w, top), ImVec2(cx + w, bottom), color);
          draw_list->AddRect(ImVec2(cx - w, top), ImVec2(cx + w, bottom), IM_COL32(40, 40, 40, 255));
        }
      }
      
      ImPlot::EndPlot();
    }
  }

  void StockUI::StockAnalyzer()
  {
    KanVasX::Panel::Begin("Stock Analyzer");
    {
      const float contentRegionAvailX = ImGui::GetContentRegionAvail().x;
      
      // Stock Data
      static StockData stockData{""};
      
      // History date range
      static KanVasX::Date startDate{2024, 1, 30};
      static KanVasX::Date endDate{2024, 1, 31};
      
      static const char* inteval[] =
      {
        "1m", "2m", "5m", "15m", "30m", "60m", "90m", "1h", "1d", "5d", "1wk", "1mo", "3mo"
      };
      static const char* currentInterval = "1m";

      static const char* range[] =
      {
        "1d", "5d", "1mo", "6mo", "ytd", "1y", "5y", "max"
      };
      static const char* currentRange = "1d";
      
      static std::unordered_map<std::string, std::vector<std::string>> rangeIntervalMap =
      {
        {"1d",  {"1m", "2m", "5m", "15m", "30m", "60m"}},
        {"5d",  {"1m", "2m", "5m", "15m", "30m", "60m"}},
        {"1mo", {"5m", "15m", "30m", "60m", "1d"}},
        {"6mo", {"1h", "1d", "1wk"}},
        {"ytd", {"1h", "1wk", "1mo"}},
        {"1y",  {"1d", "1wk", "1mo"}},
        {"5y",  {"1d", "1wk", "1mo"}},
        {"max", {"1d", "1wk", "1mo"}},
      };

      auto UpdateStockData = [](const std::string& symbol)
      {
        time_t startTime = StockParser::ParseDateYYYYMMDD(startDate.ToString());
        time_t endTime = StockParser::ParseDateYYYYMMDD(endDate.ToString());
        
        if (startTime == 0 || endTime == 0 || endTime <= startTime)
        {
          endTime = time(nullptr);
          startTime = endTime - 60 * 60 * 24 * 90;
        }
        
        // Use period1= start (UTC), period2 = end (UTC) + one day to include end date
        long period1 = static_cast<long>(startTime);
        long period2 = static_cast<long>(endTime + 60*60*24);
        
        stockData = StockController::UpdateStockData(symbol, std::to_string(period1), std::to_string(period2), currentInterval, currentRange);
      };

      // 🔒 Non-resizable, borderless, fixed-size table
      if (ImGui::BeginTable("StockAnalyzerTable", 2, ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingFixedFit))
      {
        // UI
        float topYArea = ImGui::GetContentRegionAvail().y * 0.45f;
        float totalWidth = ImGui::GetContentRegionAvail().x;
        float firstColWidth = totalWidth * 0.3f;
        float secondColWidth = totalWidth  - firstColWidth;
        
        ImGui::TableSetupColumn("Stock Info", ImGuiTableColumnFlags_WidthFixed, firstColWidth);
        ImGui::TableSetupColumn("Chart", ImGuiTableColumnFlags_WidthFixed, secondColWidth);
        
        ImGui::TableNextRow();
        
        // ============================================================
        // 🧭 COLUMN 1 : Stock Details
        // ============================================================
        {
          ImGui::TableSetColumnIndex(0);
          KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundDark, topYArea, 0.2985);
          KanVasX::UI::DrawFilledRect(KanVasX::Color::FrameBg, 40, 0.2985);
          
          static char searchedString[128] = "Nifty";
          
          static bool firstUpdate = true;
          if (firstUpdate)
          {
            UpdateStockData(searchedString);
            firstUpdate = false;
          }

          if (KanVasX::Widget::Search(searchedString, 128, KanVasX::Settings::FrameHeight, contentRegionAvailX * 0.278f, "Enter Symbol ...", UI::Font::Get(UI::FontType::Large), true))
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
          {
            using namespace UI;
            static const auto &textColor = KanVasX::Color::TextBright;
            static const auto &alignLeft = KanVasX::UI::AlignX::Left;
            
            // Stock Name and Value
            std::string name = stockData.shortName + " (" + stockData.currency + ")";
            KanVasX::UI::Text(Font::Get(FontType::Header_36), name.c_str(), alignLeft, {30.0f, 10.0f}, textColor);
            KanVasX::UI::Text(Font::Get(FontType::Header_24), stockData.longName .c_str(), alignLeft, {30.0f, 0.0f}, textColor);
            KanVasX::UI::Text(Font::Get(FontType::Header_48), Utils::FormatDoubleToString(stockData.livePrice), alignLeft, {30.0f, 10.0f}, textColor);
            
            // Change
            std::string change = stockData.change > 0 ? "+" : "";
            change += Utils::FormatDoubleToString(stockData.change);
            change += stockData.change > 0 ? " ( +" : " ( ";
            change += Utils::FormatDoubleToString(stockData.changePercent) + "%)";
            ImU32 changeColor = stockData.change > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;
            KanVasX::UI::Text(Font::Get(FontType::Header_30), change, alignLeft, {30.0f, 10.0f}, changeColor);
            
            // Under line
            KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, 0.24, {20.0, 5.0});
            KanVasX::UI::ShiftCursorY(10);
            
            // 52-Week
            std::string fiftytwoWeek = Utils::FormatDoubleToString(stockData.fiftyTwoLow) + " - " +
            Utils::FormatDoubleToString(stockData.fiftyTwoHigh);
            KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), "52-Week Range ", alignLeft, {30.0f, 10.0f}, textColor);
            ImGui::SameLine();
            KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), fiftytwoWeek, alignLeft, {50.0f, 0.0f}, textColor);
            
            // Day
            std::string dayRange = Utils::FormatDoubleToString(stockData.dayLow) + " - " +
            Utils::FormatDoubleToString(stockData.dayHigh);
            KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), "Day Range     ", alignLeft, {30.0f, 10.0f}, textColor);
            ImGui::SameLine();
            KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), dayRange, alignLeft, {50.0f, 0.0f}, textColor);
            
            // Volume
            KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), "Volume        ", alignLeft, {30.0f, 10.0f}, textColor);
            ImGui::SameLine();
            KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), Utils::FormatLargeNumber(stockData.volume), alignLeft, {50.0f, 0.0f}, textColor);
            
            KanVasX::UI::ShiftCursorY(10);
            KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, 0.2, {20.0, 5.0});
          }
        }
        
        // ============================================================
        // 📊 COLUMN 2 : Chart
        // ============================================================
        {
          ImGui::TableSetColumnIndex(1);
          KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundDark, topYArea, 0.688);
          
          if (stockData.IsValid())
          {            
            DrawCandleChart(stockData.history);
            
            bool modify = false;
            for (int i = 0; i < IM_ARRAYSIZE(range); ++i)
            {
              auto buttonColor = range[i] == currentRange ? KanVasX::Color::BackgroundLight : KanVasX::Color::BackgroundDark;
              if (KanVasX::UI::DrawButton(range[i], nullptr, buttonColor))
              {
                currentRange = range[i];
                currentInterval = rangeIntervalMap[currentRange][0].c_str();

                modify = true;
              }
              if (i < IM_ARRAYSIZE(range) - 1)
              {
                ImGui::SameLine();
              }
            }
            
            ImGui::SameLine();
            KanVasX::UI::ShiftCursorX(50);
            ImGui::Checkbox(" Show Candle", &s_showCandle);

            ImGui::SameLine();
            std::vector<std::string> intervalValues = rangeIntervalMap[currentRange];
            KanVasX::UI::ShiftCursorX(ImGui::GetContentRegionAvail().x - (intervalValues.size() * 40));
            for (int i = 0; i < intervalValues.size(); ++i)
            {
              auto buttonColor = intervalValues[i] == currentInterval ? KanVasX::Color::BackgroundLight : KanVasX::Color::BackgroundDark;
              if (KanVasX::UI::DrawButton(intervalValues[i], nullptr, buttonColor))
              {
                currentInterval = intervalValues[i].c_str();
                modify = true;
              }
              if (i < IM_ARRAYSIZE(inteval) - 1)
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
      }
      ImGui::EndTable();

      // 🔒 Non-resizable, borderless, fixed-size table
      KanVasX::UI::ShiftCursorY(5);
      if (ImGui::BeginTable("Portfolio Manager", 2, ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingFixedFit))
      {
        // UI
        float topYArea = ImGui::GetContentRegionAvail().y;
        float totalWidth = ImGui::GetContentRegionAvail().x;
        float firstColWidth = totalWidth * 0.3f;
        float secondColWidth = totalWidth  - firstColWidth;
        
        ImGui::TableSetupColumn(" DATA 1", ImGuiTableColumnFlags_WidthFixed, firstColWidth);
        ImGui::TableSetupColumn(" DATA 2", ImGuiTableColumnFlags_WidthFixed, secondColWidth);
        
        ImGui::TableNextRow();
        
        // ============================================================
        // 🧭 COLUMN 1 :
        // ============================================================
        {
          ImGui::TableSetColumnIndex(0);
          KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundDark, topYArea, 0.2985);
        }
        
        // ============================================================
        // 📊 COLUMN 2 :
        // ============================================================
        {
          ImGui::TableSetColumnIndex(1);
          KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundDark, topYArea, 0.688);
        }
      }
      ImGui::EndTable();
      KanVasX::Panel::End();
    }
  }
} // namespace KanVest
