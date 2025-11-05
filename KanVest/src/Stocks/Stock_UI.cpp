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

#include "Portfolio/Portfolio.hpp"
#include "Portfolio/UserManager.hpp"

namespace KanVest
{
#define KanVest_Text(font, string, offset, textColor) \
KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::font), string, KanVasX::UI::AlignX::Left, offset, textColor);
  
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

    [[maybe_unused]] static inline time_t ToMarketTime(time_t utcTimestamp)
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

    ImU32 GetTrendColor(const std::string& trend)
    {
      if (trend == "Uptrend") return KanVasX::Color::Cyan;
      if (trend == "Downtrend") return KanVasX::Color::Red;
      if (trend == "Sideways") return KanVasX::Color::White;
      if (trend == "Mixed") return KanVasX::Color::Yellow;
      
      return KanVasX::Color::White;
    }

    ImU32 GetMomentumColor(const std::string& momentum)
    {
      if (momentum == "Strong") return KanVasX::Color::Cyan;
      if (momentum == "Weak") return KanVasX::Color::Red;
      if (momentum == "Moderate") return KanVasX::Color::White;
      
      return KanVasX::Color::White;
    }

    ImU32 GetVoltalityColor(const std::string& voltality)
    {
      if (voltality == "High") return KanVasX::Color::Red;
      if (voltality == "Low") return KanVasX::Color::Cyan;
      if (voltality == "Medium") return KanVasX::Color::White;
      
      return KanVasX::Color::White;
    }
    
    ImU32 GetVolumeColor(const std::string& volume)
    {
      if (volume == "High") return KanVasX::Color::Cyan;
      if (volume == "Low") return KanVasX::Color::Red;
      if (volume == "Normal") return KanVasX::Color::White;
      
      return KanVasX::Color::White;
    }

    ImU32 GetVWAPColor(const std::string& vwap)
    {
      if (vwap == "Above VWAP") return KanVasX::Color::Cyan;
      if (vwap == "Below VWAP") return KanVasX::Color::Red;
      if (vwap == "Near VWAP") return KanVasX::Color::White;
      
      return KanVasX::Color::White;
    }

    ImU32 GetValuationColor(const std::string& valuation)
    {
      if (valuation == "Oversold") return KanVasX::Color::Cyan;
      if (valuation == "Overbought") return KanVasX::Color::Red;
      if (valuation == "Fair") return KanVasX::Color::White;
      
      return KanVasX::Color::White;
    }

    ImU32 GetActionColor(const std::string& actiom)
    {
      if (actiom == "Buy") return KanVasX::Color::Cyan;
      if (actiom == "Sell") return KanVasX::Color::Red;
      if (actiom == "Hold") return KanVasX::Color::White;
      
      return KanVasX::Color::White;
    }

    ImU32 GetConfidenceColor(double value)
    {
      if (value < 0.25) return KanVasX::Color::Red;
      if (value < 0.5)  return KanVasX::Color::Yellow;
      if (value < 0.75)  return KanVasX::Color::Blue;
      if (value < 1.0)  return KanVasX::Color::Cyan;

      return KanVasX::Color::White;
    }

    ImU32 GetSummaryColor(const std::string& summary)
    {
      if (summary.find("Buy") != std::string::npos)
        return KanVasX::Color::Cyan;
      if (summary.find("Sell") != std::string::npos)
        return KanVasX::Color::Red;
      if (summary.find("Hold") != std::string::npos)
        return KanVasX::Color::White;
      
      return KanVasX::Color::White; // fallback
    }

    std::pair<std::string, std::string> SplitSuggestion(const std::string& suggestion)
    {
      size_t pos = suggestion.find(':');
      if (pos != std::string::npos)
      {
        // Trim spaces around the colon
        std::string action  = suggestion.substr(0, pos);
        std::string message = suggestion.substr(pos + 1);
        
        // Trim extra spaces
        auto trim = [](std::string& s)
        {
          size_t start = s.find_first_not_of(" \t");
          size_t end   = s.find_last_not_of(" \t");
          if (start == std::string::npos || end == std::string::npos)
            s.clear();
          else
            s = s.substr(start, end - start + 1);
        };
        
        trim(action);
        trim(message);
        
        return { action, message };
      }
      
      // If no colon found, treat everything as message
      return { "Unknown", suggestion };
    }

  } // namespace Utils
  
  static float FirstColumnSize = 0.2685f;
  
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
    
    KanVasX::Panel::End();
  }
  
  void StockUI::SearchBar()
  {
    KanVasX::UI::DrawFilledRect(KanVasX::Color::FrameBg, 40, FirstColumnSize);
    const float contentRegionAvailX = ImGui::GetContentRegionAvail().x;
    if (KanVasX::Widget::Search(s_searchedString, 128, KanVasX::Settings::FrameHeight, contentRegionAvailX * 0.9f, "Enter Symbol ...", UI::Font::Get(UI::FontType::Large), true))
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

    // Name & price
    std::string name = stockData.shortName + " (" + stockData.currency + ")";

    KanVest_Text(Header_36, name.c_str(), glm::vec2(30.0f, 10.0f), textColor);
    KanVest_Text(Header_24, stockData.longName.c_str(), glm::vec2(30.0f, 10.0f), textColor);
    KanVest_Text(Header_48, Utils::FormatDoubleToString(stockData.livePrice), glm::vec2(30.0f, 10.0f), textColor);
    
    // Change
    std::string change = (stockData.change > 0 ? "+" : "") +
    Utils::FormatDoubleToString(stockData.change) +
    (stockData.change > 0 ? " ( +" : " ( ") +
    Utils::FormatDoubleToString(stockData.changePercent) + "%)";
    
    ImU32 changeColor = stockData.change > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;
    KanVest_Text(Header_30, change, glm::vec2(30.0f, 10.0f), changeColor);

    static const float UnderLineSize = 0.28;
    KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, UnderLineSize, {20.0f, 5.0f});
    KanVasX::UI::ShiftCursorY(20);

    // 52-Week Range
    std::string fiftyTwoWeek = Utils::FormatDoubleToString(stockData.fiftyTwoLow) + " - " + Utils::FormatDoubleToString(stockData.fiftyTwoHigh);
    KanVest_Text(FixedWidthHeader_18, "52-Week Range ", glm::vec2(30.0f, 5.0f), textColor);
    ImGui::SameLine();
    KanVest_Text(FixedWidthHeader_18, fiftyTwoWeek, glm::vec2(50.0f, 0.0f), textColor);

    // Day Range
    std::string dayRange = Utils::FormatDoubleToString(stockData.dayLow) + " - " + Utils::FormatDoubleToString(stockData.dayHigh);
    KanVest_Text(FixedWidthHeader_18, "Day Range     ", glm::vec2(30.0f, 5.0f), textColor);
    ImGui::SameLine();
    KanVest_Text(FixedWidthHeader_18, dayRange, glm::vec2(50.0f, 0.0f), textColor);

    // Volume
    KanVest_Text(FixedWidthHeader_18, "Volume        ", glm::vec2(30.0f, 5.0f), textColor);
    ImGui::SameLine();
    KanVest_Text(FixedWidthHeader_18, Utils::FormatLargeNumber(stockData.volume), glm::vec2(50.0f, 0.0f), textColor);

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
    
    // Keep only trading days (you already had this).
    std::vector<StockPoint> filtered = Utils::FilterTradingDays(history);
    if (filtered.empty())
    {
      ImGui::Text("No trading days in this range");
      return;
    }
    
    // We'll use sequential x indices so weekends/holidays are skipped visually.
    std::vector<double> xs, opens, highs, lows, closes;
    xs.reserve(filtered.size());
    opens.reserve(filtered.size());
    highs.reserve(filtered.size());
    lows.reserve(filtered.size());
    closes.reserve(filtered.size());
    
    double ymin = DBL_MAX;
    double ymax = -DBL_MAX;
    
    for (size_t i = 0; i < filtered.size(); ++i)
    {
      const auto& h = filtered[i];
      xs.push_back(static_cast<double>(i)); // sequential index
      opens.push_back(h.open);
      highs.push_back(h.high);
      lows.push_back(h.low);
      closes.push_back(h.close);
      ymin = std::min(ymin, h.low);
      ymax = std::max(ymax, h.high);
    }
    
    // Build human-readable labels locally (e.g., "2025-11-03").
    // To avoid too many labels, we'll place a label every few points.
    std::vector<std::string> labelStrings;
    std::vector<const char*> labelPtrs;
    std::vector<double> labelPositions;
    
    // Decide label step based on number of points: aim for ~8-12 labels max.
    int targetLabels = 10;
    size_t n = filtered.size();
    size_t labelStep = 1;
    if (n > 0)
    {
      labelStep = std::max<size_t>(1, (n + targetLabels - 1) / targetLabels);
    }
    
    labelStrings.reserve((n + labelStep - 1) / labelStep);
    labelPositions.reserve(labelStrings.capacity());
    labelPtrs.reserve(labelStrings.capacity());
    
    for (size_t i = 0; i < n; i += labelStep)
    {
      // Convert timestamp to UTC date string
      time_t t = static_cast<time_t>(filtered[i].timestamp);
      struct tm tm{};
#if defined(_WIN32)
      gmtime_s(&tm, &t);
#else
      gmtime_r(&t, &tm);
#endif
      char buf[64];
      // Format: YYYY-MM-DD (change format if you want time too)
      std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
      labelStrings.emplace_back(buf);
      labelPositions.push_back(static_cast<double>(i));
    }
    
    // Convert label strings to char* for ImPlot
    for (auto &s : labelStrings)
      labelPtrs.push_back(s.c_str());
    
    // Start plotting
    if (ImPlot::BeginPlot("", ImVec2(-1, 300)))
    {
      // We use AutoFit for X and set Y limits explicitly
      ImPlot::SetupAxes("", "", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
      ImPlot::SetupAxisLimits(ImAxis_Y1, ymin - 1.0, ymax + 1.0, ImGuiCond_Always);
      
      // Setup X axis ticks with our custom positions and labels (compressed timeline)
      if (!labelPositions.empty())
      {
        // ImPlot::SetupAxisTicks accepts arrays of positions and labels.
        // Provide positions (double*) and labels (const char*[])
        ImPlot::SetupAxisTicks(ImAxis_X1, labelPositions.data(), static_cast<int>(labelPositions.size()), labelPtrs.data());
      }
      
      // Plot close line (optional)
      ImPlot::PlotLine("", xs.data(), closes.data(), static_cast<int>(xs.size()));
      
      ImDrawList* drawList = ImPlot::GetPlotDrawList();
      
      if (s_showCandle)
      {
        for (size_t i = 0; i < xs.size(); ++i)
        {
          ImU32 color = (closes[i] >= opens[i]) ? KanVasX::Color::Cyan : KanVasX::Color::Red;
          
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
      
      // --- Tooltip (hover info with date + time) ---
      if (ImPlot::IsPlotHovered() && !filtered.empty())
      {
        ImPlotPoint mouse = ImPlot::GetPlotMousePos();
        
        // Find nearest candle index
        int idx = (int)std::round(mouse.x);
        idx = std::clamp(idx, 0, (int)filtered.size() - 1);
        
        const StockPoint& p = filtered[idx];
        
        // Convert timestamp → readable date + time
        time_t t = static_cast<time_t>(p.timestamp);
        struct tm tm {};
#if defined(_WIN32)
        localtime_s(&tm, &t); // Use local time for readability
#else
        localtime_r(&t, &tm);
#endif
        char dateTimeBuf[64];
        std::strftime(dateTimeBuf, sizeof(dateTimeBuf), "%Y-%m-%d %H:%M", &tm);
        
        // Draw tooltip near the cursor
        ImGui::BeginTooltip();
        ImGui::TextColored(ImVec4(1, 0.8f, 0, 1), "%s", dateTimeBuf);
        ImGui::Separator();
        ImGui::Text("Open : %.2f", p.open);
        ImGui::Text("High : %.2f", p.high);
        ImGui::Text("Low  : %.2f", p.low);
        ImGui::Text("Close: %.2f", p.close);
        ImGui::EndTooltip();
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

        std::string displayText = StockController::ValidRange[i];
        std::string uniqueLabel = displayText + "##Range" + std::to_string(i);
        if (KanVasX::UI::DrawButton(uniqueLabel, nullptr, buttonColor))
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
      KanVasX::UI::ShiftCursorX(10);
      ImGui::Text(" %.1f s ", StockController::GetRefreshInterval());
      ImGui::SameLine();
      KanVasX::UI::ShiftCursorX(10);

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
        
        std::string displayText = intervalValues[i];
        std::string uniqueLabel = displayText + "##Interval" + std::to_string(i);

        if (KanVasX::UI::DrawButton(uniqueLabel, nullptr, buttonColor))
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

    if (ImGui::BeginTable("StockAnalyzerTable", 3, ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingFixedFit))
    {
      float topYArea = ImGui::GetContentRegionAvail().y * 0.99f;
      float totalWidth = ImGui::GetContentRegionAvail().x;
      float firstColWidth = totalWidth * FirstColumnSize;
      float thirdColWidth = totalWidth * FirstColumnSize;
      float secondColWidth = totalWidth - firstColWidth - thirdColWidth;
      
      ImGui::TableSetupColumn("Stock Info", ImGuiTableColumnFlags_WidthFixed, firstColWidth);
      ImGui::TableSetupColumn("Chart", ImGuiTableColumnFlags_WidthFixed, secondColWidth);
      ImGui::TableSetupColumn("Portfolio", ImGuiTableColumnFlags_WidthFixed, thirdColWidth);

      ImGui::TableNextRow(ImGuiTableRowFlags_None, topYArea); // <-- fixed row height

      const StockData& stockData = StockController::GetActiveStockData();
      // Column 1 Stock Details
      {
        ImGui::TableSetColumnIndex(0);
        if (ImGui::BeginChild("StockDetailsCell", ImVec2(firstColWidth, topYArea))) // fixed height
        {
          SearchBar();
          ShowStcokBasicData(stockData);
          
          ShowStockDetails();
        }
        ImGui::EndChild();
      }
      
      // Column 2 Chart
      {
        ImGui::TableSetColumnIndex(1);
        if (ImGui::BeginChild("Portfolio", ImVec2(secondColWidth, topYArea))) // fixed height
        {
//        DrawCandleChart(stockData);
//        DrawChartController(stockData);
//        ImGui::NewLine();
//        KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, secondColWidth);
//        KanVasX::UI::ShiftCursorY(5.0f);

          KanVasX::UI::DrawFilledRect(KanVasX::Color::FrameBg, 40, 0.46);
          KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_26), "Portfolio", KanVasX::UI::AlignX::Center, {0.0f, 10.0f});
          
          ShowPortfolio();
        }
        ImGui::EndChild();
      }
      
      // Column 3 Chart
      {
        ImGui::TableSetColumnIndex(2);
        if (ImGui::BeginChild("Oth", ImVec2(thirdColWidth, topYArea))) // fixed height
        {

        }
        ImGui::EndChild();
      }
      ImGui::EndTable();
    }
  }
  
  void StockUI::ShowPortfolio()
  {
    KanViz::Ref<Portfolio> portfolio = UserManager::GetCurrentUser().portfolio;
    if (!portfolio)
    {
      ImGui::TextDisabled("No portfolio loaded.");
      return;
    }
    
    auto& holdings = portfolio->GetHoldings();
    
    ImGui::Spacing();
    
    // ---- Add New Holding Row ----
    static char symbolBuf[16] = "";
    static float avgPrice = 0.0f;
    static int quantity = 0;
    static int selectedVision = 0;
    static const char* visions[] = {"Long Term", "Mid Term", "Short Term"};
    
    float availX = ImGui::GetContentRegionAvail().x;
    ImGui::PushItemWidth(availX * 0.22); ImGui::InputTextWithHint("##symbol", "Symbol", symbolBuf, IM_ARRAYSIZE(symbolBuf)); ImGui::SameLine();
    ImGui::PushItemWidth(availX * 0.22); ImGui::InputFloat("##avg", &avgPrice, 0, 0, "%.2f"); KanVasX::UI::Tooltip("Average price");  ImGui::SameLine();
    ImGui::PushItemWidth(availX * 0.22); ImGui::InputInt("##qty", &quantity); KanVasX::UI::Tooltip("Quantity");  ImGui::SameLine();
    ImGui::PushItemWidth(availX * 0.22); ImGui::Combo("##vision", &selectedVision, visions, IM_ARRAYSIZE(visions)); ImGui::SameLine();
    
    if (ImGui::Button("Add", ImVec2(60, 0)))
    {
      std::string symbol = symbolBuf;
      if (!symbol.empty() && avgPrice > 0 && quantity > 0)
      {
        InvestmentHorizon h;
        h.symbol = symbol;
        h.averagePrice = avgPrice;
        h.quantity = quantity;
        h.vision = static_cast<InvestmentHorizon::Vision>(selectedVision);
        portfolio->AddHolding(h);
        UserManager::GetCurrentUser().SavePortfolio();
        
        symbolBuf[0] = '\0';
        avgPrice = 0.0f;
        quantity = 0;
        selectedVision = 0;
      }
      else ImGui::OpenPopup("InvalidInput");
    }
    
    if (ImGui::BeginPopup("InvalidInput"))
    {
      ImGui::Text("Please fill all fields correctly!");
      if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
      ImGui::EndPopup();
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // ---- Table ----
    ImGuiTableFlags flags =
    ImGuiTableFlags_Borders |
    ImGuiTableFlags_RowBg |
    ImGuiTableFlags_Sortable |
    ImGuiTableFlags_Resizable |
    ImGuiTableFlags_Reorderable |
    ImGuiTableFlags_ScrollY;
    
    static int editIndex = -1;
    
    if (ImGui::BeginTable("PortfolioTable", 4, flags, ImVec2(-FLT_MIN, 300.0f)))
    {
      ImGui::TableSetupColumn("Symbol");
      ImGui::TableSetupColumn("Average Price");
      ImGui::TableSetupColumn("Quantity");
      ImGui::TableSetupColumn("Vision");
      ImGui::TableHeadersRow();
      
      // ---- Sorting ----
      if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs())
      {
        if (sortSpecs->SpecsDirty && sortSpecs->SpecsCount > 0)
        {
          const ImGuiTableColumnSortSpecs& spec = sortSpecs->Specs[0];
          auto comparator = [&](const InvestmentHorizon& a, const InvestmentHorizon& b)
          {
            switch (spec.ColumnIndex)
            {
              case 0: return spec.SortDirection == ImGuiSortDirection_Ascending ? (a.symbol < b.symbol) : (a.symbol > b.symbol);
              case 1: return spec.SortDirection == ImGuiSortDirection_Ascending ? (a.averagePrice < b.averagePrice) : (a.averagePrice > b.averagePrice);
              case 2: return spec.SortDirection == ImGuiSortDirection_Ascending ? (a.quantity < b.quantity) : (a.quantity > b.quantity);
              case 3: return spec.SortDirection == ImGuiSortDirection_Ascending
                ? (static_cast<int>(a.vision) < static_cast<int>(b.vision))
                : (static_cast<int>(a.vision) > static_cast<int>(b.vision));
              default: return false;
            }
          };
          std::sort(holdings.begin(), holdings.end(), comparator);
          sortSpecs->SpecsDirty = false;
        }
      }
      
      // ---- Table Rows ----
      for (int i = 0; i < holdings.size(); ++i)
      {
        const auto& h = holdings[i];
        ImGui::TableNextRow();
        
        // Mark the start of the row so we can cover all columns with a selectable region
        ImGui::TableNextColumn();
        ImGui::PushID(i);
        bool rowSelected = false;
        
        // Create an invisible selectable covering the whole row
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(255,255,255,0));
        ImGui::Selectable(("##row_select_" + std::to_string(i)).c_str(), &rowSelected,
                          ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap);
        
        // Detect right-click on the *entire row selectable*, not just a cell
        if (ImGui::BeginPopupContextItem(("RightClickMenu_" + std::to_string(i)).c_str()))
        {
          if (ImGui::MenuItem("Edit")) editIndex = i;
          if (ImGui::MenuItem("Delete"))
          {
            holdings.erase(holdings.begin() + i);
            UserManager::GetCurrentUser().SavePortfolio();
          }
          ImGui::EndPopup();
        }
        
        // Draw actual text cells AFTER creating the selectable
        ImGui::SameLine(); ImGui::Text("%s", h.symbol.c_str());
        ImGui::TableNextColumn(); ImGui::Text("%.2f", h.averagePrice);
        ImGui::TableNextColumn(); ImGui::Text("%d", h.quantity);
        ImGui::TableNextColumn(); ImGui::Text("%s", PortfolioUtils::VisionToString(h.vision).c_str());
        
        ImGui::PopID();
      }

      ImGui::EndTable();
    }
    
    // ---- Edit Popup ----
    if (editIndex >= 0 && editIndex < holdings.size())
    {
      ImGui::OpenPopup("Edit Holding");
      
      static char editSymbol[16];
      static float editAvgPrice;
      static int editQty;
      static int editVision;
      static bool init = true;
      
      if (init)
      {
        strcpy(editSymbol, holdings[editIndex].symbol.c_str());
        editAvgPrice = holdings[editIndex].averagePrice;
        editQty = holdings[editIndex].quantity;
        editVision = static_cast<int>(holdings[editIndex].vision);
        init = false;
      }
      
      if (ImGui::BeginPopupModal("Edit Holding", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
      {
        KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 22.0f, 0.96);
        KanVasX::UI::Text(UI::Font::Get(UI::FontType::Large), "Edit Holding", KanVasX::UI::AlignX::Center);
        
        ImGui::InputText("Symbol", editSymbol, IM_ARRAYSIZE(editSymbol));
        ImGui::InputFloat("Average Price", &editAvgPrice, 0, 0, "%.2f");
        ImGui::InputInt("Quantity", &editQty);
        ImGui::Combo("Vision", &editVision, visions, IM_ARRAYSIZE(visions));
        
        ImGui::Spacing();
        if (ImGui::Button("Save", ImVec2(80, 0)))
        {
          holdings[editIndex].symbol = editSymbol;
          holdings[editIndex].averagePrice = editAvgPrice;
          holdings[editIndex].quantity = editQty;
          holdings[editIndex].vision = static_cast<InvestmentHorizon::Vision>(editVision);
          
          UserManager::GetCurrentUser().SavePortfolio();
          editIndex = -1;
          init = true;
          ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(80, 0)))
        {
          editIndex = -1;
          init = true;
          ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
      }
    }
  }
  
  void StockUI::ShowStockDetails()
  {
    if (ImGui::BeginTabBar("Stock Details"))
    {
      if (ImGui::BeginTabItem("Insights"))
      {
        ShowStockInsights();
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Technicals"))
      {
        ShowStockTechnicals();
        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }
  }

  void StockUI::ShowStockTechnicals()
  {
    StockTechnicals technicals = StockController::GetStockTechnicals();
    
    KanVest_Text(Header_30, "Technicals ", glm::vec2(30.0f, 10.0f), KanVasX::Color::TextBright);

    KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, 0.10, {20.0f, 5.0f});
    KanVasX::UI::ShiftCursorY(5.0f);
    
    static const float secondC0lumnShift = 80.0f;

    // close
    auto ShowTechnicels = [](const std::string& title, double value) {
      KanVest_Text(FixedWidthHeader_14, title.c_str(), glm::vec2(30.0f, 10.0f), KanVasX::Color::TextBright);
      ImGui::SameLine();
      KanVest_Text(FixedWidthHeader_14, Utils::FormatDoubleToString(value), glm::vec2(secondC0lumnShift, 0.0f), KanVasX::Color::TextBright);
    };
    
    ShowTechnicels("Close          ", technicals.close);
    ShowTechnicels("SMA            ", technicals.sma);
    ShowTechnicels("EMA            ", technicals.ema);
    ShowTechnicels("RSI            ", technicals.rsi);
    ShowTechnicels("ATR            ", technicals.atr);
    ShowTechnicels("VWAM           ", technicals.vwap);
    ShowTechnicels("MACD           ", technicals.macd);
    ShowTechnicels("Average Volume ", technicals.avgVol);
    ShowTechnicels("Latest Volume  ", technicals.latestVol);
  
    KanVasX::UI::ShiftCursorY(10.0f);
    KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, 0.28, {20.0f, 5.0f});
  }

  void StockUI::ShowStockInsights()
  {
    StockSummary summary = StockController::GetStockSummary();
    
    const ImU32 textColor = KanVasX::Color::TextBright;

    KanVest_Text(Header_30, "Stock Insights ", glm::vec2(30.0f, 10.0f), textColor);
    KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, 0.15, {20.0f, 5.0f});
    KanVasX::UI::ShiftCursorY(10.0f);

    static const float secondC0lumnShift = 80.0f;
    
    // Trend
    const auto& [action, reason] = Utils::SplitSuggestion(summary.suggestion);
    KanVest_Text(FixedWidthHeader_18, "Action     ", glm::vec2(30.0f, 5.0f), textColor);
    ImGui::SameLine();
    KanVest_Text(FixedWidthHeader_18, action.c_str(), glm::vec2(secondC0lumnShift, 0.0f), Utils::GetActionColor(action));
    KanVasX::UI::Tooltip(reason);

    KanVest_Text(FixedWidthHeader_18, "Trend      ", glm::vec2(30.0f, 5.0f), textColor);
    ImGui::SameLine();
    KanVest_Text(FixedWidthHeader_18, summary.trend.value.c_str(), glm::vec2(secondC0lumnShift, 0.0f), Utils::GetTrendColor(summary.trend.value));
    KanVasX::UI::Tooltip(summary.trend.reason);

    // Momentum
    KanVest_Text(FixedWidthHeader_18, "Momentum   ", glm::vec2(30.0f, 5.0f), textColor);
    ImGui::SameLine();
    KanVest_Text(FixedWidthHeader_18, summary.momentum.value.c_str(), glm::vec2(secondC0lumnShift, 0.0f), Utils::GetMomentumColor(summary.momentum.value));
    KanVasX::UI::Tooltip(summary.momentum.reason);

    // Voltality
    KanVest_Text(FixedWidthHeader_18, "Voltality  ", glm::vec2(30.0f, 5.0f), textColor);
    ImGui::SameLine();
    KanVest_Text(FixedWidthHeader_18, summary.volatility.value.c_str(), glm::vec2(secondC0lumnShift, 0.0f), Utils::GetVoltalityColor(summary.volatility.value));
    KanVasX::UI::Tooltip(summary.volatility.reason);

    // Volume
    KanVest_Text(FixedWidthHeader_18, "Volume     ", glm::vec2(30.0f, 5.0f), textColor);
    ImGui::SameLine();
    KanVest_Text(FixedWidthHeader_18, summary.volume.value.c_str(), glm::vec2(secondC0lumnShift, 0.0f), Utils::GetVolumeColor(summary.volume.value));
    KanVasX::UI::Tooltip(summary.volume.reason);

    // VWAM
    KanVest_Text(FixedWidthHeader_18, "VWAP       ", glm::vec2(30.0f, 5.0f), textColor);
    ImGui::SameLine();
    KanVest_Text(FixedWidthHeader_18, summary.vwapBias.value.c_str(), glm::vec2(secondC0lumnShift, 0.0f), Utils::GetVWAPColor(summary.vwapBias.value));
    KanVasX::UI::Tooltip(summary.vwapBias.reason);

    // Voltality
    KanVest_Text(FixedWidthHeader_18, "Valuation  ", glm::vec2(30.0f, 5.0f), textColor);
    ImGui::SameLine();
    KanVest_Text(FixedWidthHeader_18, summary.valuation.value.c_str(), glm::vec2(secondC0lumnShift, 0.0f), Utils::GetValuationColor(summary.valuation.value));
    KanVasX::UI::Tooltip(summary.valuation.reason);

    // Voltality
    KanVest_Text(FixedWidthHeader_18, "Confidence ", glm::vec2(30.0f, 5.0f), textColor);
    ImGui::SameLine();
    std::string confidenceString = Utils::FormatDoubleToString(summary.score * 100.0f) + "%";
    KanVest_Text(FixedWidthHeader_18, confidenceString.c_str(), glm::vec2(secondC0lumnShift, 0.0f), Utils::GetConfidenceColor(summary.score));

    KanVasX::UI::ShiftCursorY(10.0f);
    KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, 0.28, {20.0f, 5.0f});
  }
} // namespace KanVest
