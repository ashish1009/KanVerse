//
//  StockUI.cpp
//  KanVest
//
//  Created by Ashish . on 05/11/25.
//

#include "StockUI.hpp"

#include "Stocks/StockManager.hpp"
#include "Stocks/Analyzer/Analyzer.hpp"

#include "User/UserManager.hpp"

#include "Portfolio/PortfolioController.hpp"

namespace KanVest
{
#define KanVest_Text(font, string, offset, textColor) \
KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::font), string, KanVasX::UI::AlignX::Left, offset, textColor);

  static int g_sortColumn = 0;
  static int g_selectedRow = -1;
  static int g_editIndex = -1;

  static bool g_sortAscending = true;
  static bool g_showEditModal = false;
 
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
        {
          filtered.push_back(h);
        }
      }
      
      return filtered;
    }
  } // namespace Utils
  
  void StockUI::Initialize(ImTextureID reloadIconID)
  {
    s_reloadIconID = reloadIconID;
  }

  void StockUI::StockPanel()
  {
    KanVasX::Panel::Begin("Stock Analyzer");
    
    if (ImGui::BeginTable("StockAnalyzerTable", 3, ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingFixedFit))
    {
      float topYArea = ImGui::GetContentRegionAvail().y * 0.99f;
      float totalWidth = ImGui::GetContentRegionAvail().x;
      
      float firstColWidth = totalWidth * 0.26;
      float thirdColWidth = totalWidth * 0.26;
      float secondColWidth = totalWidth - firstColWidth - thirdColWidth;

      ImGui::TableSetupColumn("Stock Info", ImGuiTableColumnFlags_WidthFixed, firstColWidth);
      ImGui::TableSetupColumn("Chart", ImGuiTableColumnFlags_WidthFixed, secondColWidth);
      ImGui::TableSetupColumn("Portfolio", ImGuiTableColumnFlags_WidthFixed, thirdColWidth);

      ImGui::TableNextRow(ImGuiTableRowFlags_None, topYArea); // <-- fixed row height

      // Column 1 Stock Details
      {
        ImGui::TableSetColumnIndex(0);
        if (ImGui::BeginChild("StockDetailsCell", ImVec2(firstColWidth, topYArea))) // fixed height
        {
          ShowStockDetails();
        }
        ImGui::EndChild();
      }

      // Column 2 Portfolio
      {
        ImGui::TableSetColumnIndex(1);
        if (ImGui::BeginChild("PortfolioCell", ImVec2(secondColWidth, topYArea))) // fixed height
        {
          ShowPortfolio();
        }
        ImGui::EndChild();
      }

      // Column 3 Watch list
      {
        ImGui::TableSetColumnIndex(2);
        if (ImGui::BeginChild("WatchlistCell", ImVec2(thirdColWidth, topYArea))) // fixed height
        {
          DrawAnalysisPanel();
        }
        ImGui::EndChild();
      }
      ImGui::EndTable();
    }
    
    KanVasX::Panel::End();
  }
  
  void StockUI::ShowStockDetails()
  {
    SearchBar();
    ShowStcokBasicData();
    ShowAnalyzerData();
  }
  
  void StockUI::ShowPortfolio()
  {
    if (ImGui::BeginChild("StockPortfolioCell", ImVec2(-1, ImGui::GetContentRegionAvail().y * 0.59))) // fixed height
    {
      KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundLight, 40);
      KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_26), "Portfolio", KanVasX::UI::AlignX::Center, {0, 5});

      DrawPortfolioTable(UserManager::GetCurrentUser().portfolio.get());
    }
    ImGui::EndChild();

    KanVasX::UI::ShiftCursorY(10.0f);
    if (ImGui::BeginChild("ChartCell", ImVec2(-1, ImGui::GetContentRegionAvail().y))) // fixed height
    {
      DrawCandleChart();
      DrawChartController();
    }
    ImGui::EndChild();
  }
  
  void StockUI::DrawCandleChart()
  {
    StockData stockData = StockManager::GetSelectedStockData();
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
    if (ImPlot::BeginPlot("##", ImVec2(-1, ImGui::GetContentRegionAvail().y * 0.9)))
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
  
  void StockUI::DrawChartController()
  {
    StockData stockData = StockManager::GetSelectedStockData();
    if (!stockData.IsValid())
    {
      return;
    }
    bool modify = false;
    for (int i = 0; i < IM_ARRAYSIZE(StockManager::ValidRange); ++i)
    {
      auto buttonColor = StockManager::ValidRange[i] == StockManager::GetCurrentRange() ? KanVasX::Color::BackgroundLight : KanVasX::Color::BackgroundDark;
      
      std::string displayText = StockManager::ValidRange[i];
      std::string uniqueLabel = displayText + "##Range" + std::to_string(i);
      if (KanVasX::UI::DrawButton(uniqueLabel, nullptr, buttonColor))
      {
        StockManager::SetCurrentRange(StockManager::ValidRange[i]);
        StockManager::SetCurrentInterval(StockManager::RangeIntervalMap[StockManager::GetCurrentRange()][0].c_str());
        
        modify = true;
      }
      if (i < IM_ARRAYSIZE(StockManager::ValidRange) - 1)
      {
        ImGui::SameLine();
      }
    }
    
    ImGui::SameLine();
    KanVasX::UI::ShiftCursorX(20);
    ImGui::Checkbox(" Show Candle", &s_showCandle);
        
    ImGui::SameLine();
    std::vector<std::string> intervalValues = StockManager::RangeIntervalMap[StockManager::GetCurrentRange()];
    KanVasX::UI::ShiftCursorX(ImGui::GetContentRegionAvail().x - (intervalValues.size() * 40));
    for (int i = 0; i < intervalValues.size(); ++i)
    {
      auto buttonColor = intervalValues[i] == StockManager::GetCurrentInterval() ? KanVasX::Color::BackgroundLight : KanVasX::Color::BackgroundDark;
      
      std::string displayText = intervalValues[i];
      std::string uniqueLabel = displayText + "##Interval" + std::to_string(i);
      
      if (KanVasX::UI::DrawButton(uniqueLabel, nullptr, buttonColor))
      {
        StockManager::SetCurrentInterval(intervalValues[i].c_str());
        modify = true;
      }
      if (i < intervalValues.size())
      {
        ImGui::SameLine();
      }
    }
  }
  
  void StockUI::ShowAnalyzerData()
  {
    KanVasX::UI::ShiftCursorY(0.0f);
    
    Analysis::AnalysisReport r = StockManager::AnalyzeSelectedStock();
    
    // --- Recommendation ---
    const char* recommendationText = "";
    ImU32 recColor;
    switch (r.recommendation)
    {
      case Analysis::Recommendation::StrongBuy: recommendationText = "Strong Buy"; recColor = KanVasX::Color::Green; break;
      case Analysis::Recommendation::Buy:       recommendationText = "Buy";        recColor = KanVasX::Color::Blue; break;
      case Analysis::Recommendation::Hold:      recommendationText = "Hold";       recColor = KanVasX::Color::Yellow; break;
      case Analysis::Recommendation::Sell:      recommendationText = "Sell";       recColor = KanVasX::Color::Orange; break;
      case Analysis::Recommendation::StrongSell:recommendationText = "Strong Sell";recColor = KanVasX::Color::Red; break;
      default: recommendationText = "Unknown";   recColor = KanVasX::Color::White; break;
    }

    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_32), "Recommendation", KanVasX::UI::AlignX::Left, {30.0f, 0}, KanVasX::Color::White);
    ImGui::SameLine();
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_32), recommendationText, KanVasX::UI::AlignX::Right, {-30, 0}, recColor);

    // --- Score Bar ---
    float score = static_cast<float>(r.score);
    
    KanVasX::UI::ShiftCursorX(20.0f);
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.9);
    ImGui::ProgressBar((score + 1.0f) / 2.0f, ImVec2(-1, 0), ("Score: " + std::to_string(score)).c_str());

    // --- Suggested Quantity ---
    KanVasX::UI::ShiftCursorX(20.0f);
    ImGui::TextWrapped("%s", r.actionReason.c_str());
    if (r.suggestedActionQty != 0.0)
    {
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Medium), "Suggested Quantity", KanVasX::UI::AlignX::Left, {20.0f, 0}, KanVasX::Color::White);
      ImGui::SameLine();
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Medium), std::to_string(int(r.suggestedActionQty)), KanVasX::UI::AlignX::Right, {-30, 0}, KanVasX::Color::White);
    }

    KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, 0.9, {20.0f, 5.0f});

    KanVasX::UI::ShiftCursorY(15.0f);
    {
      if (ImGui::BeginTabBar("Technicals"))
      {
        if (ImGui::BeginTabItem("Key Indicators"))
        {
          auto Indicator = [&](const char* label, double value)
          {
            KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Medium), label, KanVasX::UI::AlignX::Left, {20.0f, 0}, KanVasX::Color::White);
            ImGui::SameLine();
            KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Medium), std::to_string(value), KanVasX::UI::AlignX::Right, {-20.0f, 0}, KanVasX::Color::White);
            if (ImGui::IsItemHovered())
            {
              ImGui::SetTooltip("%s", r.tooltips.count(label) ? r.tooltips.at(label).c_str() : "No tooltip");
            }
          };
          
          Indicator("SMA Short", r.smaShort);
          Indicator("SMA Long", r.smaLong);
          Indicator("RSI", r.rsi);
          Indicator("MACD", r.macd);
          Indicator("MACD Signal", r.macdSignal);
          Indicator("ATR", r.atr);
          Indicator("VWAP", r.vwap);
          Indicator("OBV", r.obv);
          Indicator("ADX", r.adx);
          Indicator("MFI", r.mfi);
          Indicator("ROC", r.roc);
          Indicator("CCI", r.cci);
          Indicator("StochasticK", r.stochasticK);
          Indicator("StochasticD", r.stochasticD);

          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Candlestick Patterns"))
        {
          if (r.candlePatterns.empty())
          {
            ImGui::TextDisabled("No candlestick patterns detected.");
          }
          else
          {
            for (auto& p : r.candlePatterns)
            {
              KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Medium), p.name, KanVasX::UI::AlignX::Left, {20.0f, 0}, KanVasX::Color::White);
              ImGui::SameLine();
              KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Medium), Utils::FormatDoubleToString(p.strength), KanVasX::UI::AlignX::Right, {-20.0f, 0}, KanVasX::Color::White);
              
              if (ImGui::IsItemHovered() && !p.rationale.empty())
              {
                ImGui::SetTooltip("%s", p.rationale.c_str());
              }
            }
          }
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Chart Patterns"))
        {
          if (r.chartPatterns.empty())
          {
            ImGui::TextDisabled("No chart patterns detected.");
          }
          else
          {
            for (auto& p : r.chartPatterns)
            {
              KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Medium), p.name, KanVasX::UI::AlignX::Left, {20.0f, 0}, KanVasX::Color::White);
              ImGui::SameLine();
              KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Medium), Utils::FormatDoubleToString(p.strength), KanVasX::UI::AlignX::Right, {-20.0f, 0}, KanVasX::Color::White);
              if (ImGui::IsItemHovered() && !p.rationale.empty())
              {
                ImGui::SetTooltip("%s", p.rationale.c_str());
              }
            }
          }
          ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
      }
    } // Tabs
    
    // --- Explanation ---
    if (ImGui::CollapsingHeader("Detailed Explanation", ImGuiTreeNodeFlags_DefaultOpen))
    {
      ImGui::BeginChild("ExplanationScroll", ImVec2(0, 150), true);
      ImGui::TextWrapped("%s", r.detailedExplanation.c_str());
      ImGui::EndChild();
    }
  }
  
  void StockUI::DrawAnalysisPanel()
  {
    {
      KanVasX::ScopedColor textColor(ImGuiCol_Text, KanVasX::Color::TextMuted);
      KanVasX::UI::ShiftCursor({ImGui::GetContentRegionAvail().x - 90.0f, ImGui::GetContentRegionAvail().y - 20.0f});
      ImGui::Text("FPS : %.1f", ImGui::GetIO().Framerate);
    }
  }
  
  void StockUI::AddStockInManager(const std::string& symbol)
  {
    StockManager::AddStock(symbol);
    StockManager::SetSelectedStockSymbol(symbol);
    StockManager::SetSelectedStockHoldingData(0.0, 0, "");
  }
  
  void StockUI::SearchBar()
  {
    const float contentRegionAvailX = ImGui::GetContentRegionAvail().x;
    if (KanVasX::Widget::Search(s_searchedString, 128, KanVasX::Settings::FrameHeight, contentRegionAvailX * 0.9f, "Enter Symbol ...", UI::Font::Get(UI::FontType::Large), true))
    {
      Utils::ConvertUpper(s_searchedString);
    }
    
    ImGui::SameLine();
    float iconSize = ImGui::GetItemRectSize().y - 12;
    if (KanVasX::UI::DrawButtonImage("Refresh", s_reloadIconID, false, {iconSize, iconSize}, {0.0, 6.0}) || ImGui::IsKeyDown(ImGuiKey_Enter))
    {
      AddStockInManager(s_searchedString);
    }
  }
  
  void StockUI::ShowStcokBasicData()
  {
    StockData stockData = StockManager::GetSelectedStockData();

    const ImU32 textColor = KanVasX::Color::TextBright;

    // Name & price
    std::string name = stockData.shortName + " (" + stockData.currency + ")";
    
    KanVest_Text(Header_36, name.c_str(), glm::vec2(30.0f, 0.0f), textColor);
    KanVest_Text(Header_24, stockData.longName.c_str(), glm::vec2(30.0f, 0.0f), textColor);
    KanVest_Text(Header_48, Utils::FormatDoubleToString(stockData.livePrice), glm::vec2(30.0f, 0.0f), textColor);
    
    // Change
    std::string change = (stockData.change > 0 ? "+" : "") +
    Utils::FormatDoubleToString(stockData.change) +
    (stockData.change > 0 ? " ( +" : " ( ") +
    Utils::FormatDoubleToString(stockData.changePercent) + "%)";
    
    ImU32 changeColor = stockData.change > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;
    KanVest_Text(Header_30, change, glm::vec2(30.0f, 0.0f), changeColor);
    
    static const float UnderLineSize = 0.90;
    KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, UnderLineSize, {20.0f, 0.0f});
    KanVasX::UI::ShiftCursorY(20);
    
    // 52-Week Range
    std::string fiftyTwoWeek = Utils::FormatDoubleToString(stockData.fiftyTwoLow) + " - " + Utils::FormatDoubleToString(stockData.fiftyTwoHigh);
    KanVest_Text(FixedWidthHeader_18, "52-Week Range ", glm::vec2(20.0f, 0.0f), textColor);
    ImGui::SameLine();
    KanVest_Text(FixedWidthHeader_18, fiftyTwoWeek, glm::vec2(20.0f, 0.0f), textColor);
    
    // Day Range
    std::string dayRange = Utils::FormatDoubleToString(stockData.dayLow) + " - " + Utils::FormatDoubleToString(stockData.dayHigh);
    KanVest_Text(FixedWidthHeader_18, "Day Range     ", glm::vec2(20.0f, 0.0f), textColor);
    ImGui::SameLine();
    KanVest_Text(FixedWidthHeader_18, dayRange, glm::vec2(20.0f, 0.0f), textColor);
    
    // Volume
    KanVest_Text(FixedWidthHeader_18, "Volume        ", glm::vec2(20.0f, 0.0f), textColor);
    ImGui::SameLine();
    KanVest_Text(FixedWidthHeader_18, Utils::FormatLargeNumber(stockData.volume), glm::vec2(20.0f, 0.0f), textColor);
    
    KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, UnderLineSize, {20.0f, 0.0f});
  }
  
  void StockUI::DrawPortfolioTable(Portfolio* portfolio)
  {
    auto& holdings = portfolio->GetHoldings();
    
    // ---- Sorting ----
    std::sort(holdings.begin(), holdings.end(), [](const Holding& a, const Holding& b)
              {
      auto cmp = [&](auto& x, auto& y) { return g_sortAscending ? x < y : x > y; };
      switch (g_sortColumn)
      {
        case 0: return cmp(a.symbol, b.symbol);
        case 1: return cmp(a.averagePrice, b.averagePrice);
        case 2: return cmp(a.quantity, b.quantity);
        case 3: return cmp(a.stockValue, b.stockValue);
        case 4: return cmp(a.investment, b.investment);
        case 5: return cmp(a.value, b.value);
        case 6: return cmp(a.profitLoss, b.profitLoss);
        case 7: return cmp(a.profitLossPercent, b.profitLossPercent);
        case 8: return cmp(a.dayChange, b.dayChange);
        case 9: return cmp(a.dayChangePercent, b.dayChangePercent);
        default: return false;
      }
    });
    
    // ---- Table ----
    if (ImGui::BeginTable("HoldingsTable", 10,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                          ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable))
    {
      ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_DefaultSort);
      ImGui::TableSetupColumn("ATP");
      ImGui::TableSetupColumn("Qty");
      ImGui::TableSetupColumn("LTP");
      ImGui::TableSetupColumn("Inv");
      ImGui::TableSetupColumn("Value");
      ImGui::TableSetupColumn("P/L");
      ImGui::TableSetupColumn("P/L %");
      ImGui::TableSetupColumn("Day Change");
      ImGui::TableSetupColumn("Day Change %");

      // ---- Header Row ----
      {
        KanVasX::ScopedFont headerfont(UI::Font::Get(UI::FontType::FixedWidthHeader_14));
        ImGui::TableSetupScrollFreeze(ImGui::TableGetColumnCount(), 1);
        ImGui::TableNextRow(ImGuiTableRowFlags_Headers, 22.0f);
        
        for (int col = 0; col < ImGui::TableGetColumnCount(); col++)
        {
          ImGui::TableSetColumnIndex(col);
          ImGui::TableHeader(ImGui::TableGetColumnName(col));
        }
      }
      
      // ---- Sort Header ----
      if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs())
      {
        if (sortSpecs->SpecsCount > 0)
        {
          g_sortColumn = sortSpecs->Specs[0].ColumnIndex;
          g_sortAscending = sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending;
        }
      }
      
      // ---- Holdings Rows ----
      for (int idx = 0; idx < holdings.size(); idx++)
      {
        auto& h = holdings[idx];
        ImGui::PushID(idx);
        ImGui::TableNextRow();
        
        // Selectable row
        ImGui::TableSetColumnIndex(0);
        auto cursor = ImGui::GetCursorPos();
        if (ImGui::Selectable(("##row_" + std::to_string(idx)).c_str(), g_selectedRow == idx,
                              ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
        {
          StockManager::SetSelectedStockSymbol(h.symbol);
          StockManager::SetSelectedStockHoldingData(h.averagePrice, h.quantity, h.symbol);
          g_selectedRow = idx;
        }
        
        // --- Double-click to Edit ---
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
          g_editIndex = idx;
          g_showEditModal = true;
        }
        
        // Context Menu
        if (ImGui::BeginPopupContextItem())
        {
          if (ImGui::MenuItem("Edit"))
          {
            g_editIndex = idx;
            g_showEditModal = true;
          }
          if (ImGui::MenuItem("Delete"))
          {
            holdings.erase(holdings.begin() + idx);
            UserManager::GetCurrentUser().SavePortfolio();
            ImGui::EndPopup();
            ImGui::PopID();
            break;
          }
          ImGui::EndPopup();
        }
        
        // Update data
        {
          StockData stockData("");
          if (StockManager::GetShortTermStockData(h.symbol, stockData))
          {
            h.stockValue = stockData.livePrice;
            h.investment = h.averagePrice * h.quantity;
            h.value = h.stockValue * h.quantity;
            h.profitLoss = h.value - h.investment;
            h.profitLossPercent = (h.profitLoss * 100) / h.investment;
            h.dayChange = stockData.change;
            h.dayChangePercent = stockData.changePercent;
          }
        }
        
        // Draw columns
        ImGui::SetCursorPos(cursor);
        auto PrintCell = [](int32_t colIdx, const auto& data, ImU32 textColor = KanVasX::Color::TextBright)
        {
          ImGui::TableSetColumnIndex(colIdx);
          KanVasX::ScopedColor scopedTextColor(ImGuiCol_Text, textColor);
          
          if constexpr (std::is_same_v<std::decay_t<decltype(data)>, double>)
            ImGui::Text("%.2f", data);
          else if constexpr (std::is_same_v<std::decay_t<decltype(data)>, float>)
            ImGui::Text("%.2f", data);
          else if constexpr (std::is_same_v<std::decay_t<decltype(data)>, int>)
            ImGui::Text("%d", data);
          else if constexpr (std::is_same_v<std::decay_t<decltype(data)>, std::string>)
            ImGui::Text("%s", data.c_str());
          else
            ImGui::Text("%s", std::to_string(data).c_str());
        };
        
        PrintCell(0, h.symbol);
        PrintCell(1, h.averagePrice);
        PrintCell(2, h.quantity);
        PrintCell(3, h.stockValue);
        PrintCell(4, h.investment);
        PrintCell(5, h.value);
        PrintCell(6, h.profitLoss, h.profitLoss > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red);
        PrintCell(7, h.profitLossPercent, h.profitLoss > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red);
        PrintCell(8, h.dayChange, h.dayChange > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red);
        PrintCell(9, h.dayChangePercent, h.dayChangePercent > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red);

        ImGui::PopID();
        
        if (ImGui::IsKeyPressed(ImGuiKey_Backspace))
        {
          if (g_selectedRow >= 0)
          {
            holdings.erase(holdings.begin() + g_selectedRow);
            UserManager::GetCurrentUser().SavePortfolio();
            g_selectedRow = -1;
          }
        }
      }
      
      // ---- Inline Add Row ----
      static bool showAddRow = false;
      static char symbolBuf[32] = "";
      static float avgPrice = 0.0f;
      static int quantity = 0;
      static int selectedVision = 0;
      
      // Toggle add-row when ⌘ + N is pressed
      if (ImGui::IsKeyDown(ImGuiKey_LeftSuper) && ImGui::IsKeyPressed(ImGuiKey_N))
      {
        showAddRow = !showAddRow;
        if (showAddRow)
        {
          symbolBuf[0] = '\0';
          avgPrice = 0.0f;
          quantity = 0;
          selectedVision = 0;
        }
      }
      
      if (showAddRow)
      {
        float availX = ImGui::GetContentRegionAvail().x;

        ImGui::TableNextRow();
        ImGui::PushID("AddRow");
        
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::TableSetColumnIndex(0);
        if (std::string(symbolBuf).empty())
        {
          ImGui::SetKeyboardFocusHere();
        }
        ImGui::PushItemWidth(availX);
        ImGui::InputTextWithHint("##symbol", "Symbol", symbolBuf, IM_ARRAYSIZE(symbolBuf));
        
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(availX);
        ImGui::InputFloat("##avg", &avgPrice, 0, 0, "%.2f");
        
        ImGui::TableSetColumnIndex(2);
        ImGui::PushItemWidth(availX);
        ImGui::InputInt("##qty", &quantity);
                
        if (ImGui::IsKeyPressed(ImGuiKey_Enter))
        {
          std::string symbol = symbolBuf;
          if (!symbol.empty() && avgPrice > 0 && quantity > 0)
          {
            Holding h;
            h.symbol = symbol;
            h.averagePrice = avgPrice;
            h.quantity = quantity;
            h.vision = static_cast<Holding::Vision>(selectedVision);
            
            PortfolioController portfolioController(*portfolio);
            portfolioController.AddHolding(h);
            UserManager::GetCurrentUser().SavePortfolio();
            
            // Reset
            if (!ImGui::IsKeyDown(ImGuiKey_LeftSuper))
            {
              showAddRow = false;
            }
            else
            {
              symbolBuf[0] = '\0';
              avgPrice = 0.0f;
              quantity = 0;
              selectedVision = 0;
            }
          }
        }
        
        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
          showAddRow = false;
        }
        
        ImGui::PopID();
      }
      
      ImGui::EndTable();
    }
    
    // ---- Edit Modal ----
    if (g_showEditModal && g_editIndex >= 0 && g_editIndex < holdings.size())
    {
      ImGui::OpenPopup("EditHoldingModal");
      g_showEditModal = false;
      KanVasX::UI::SetNextWindowAtCenter();
      KanVasX::UI::SetNextWindowSize({800, 80});
    }
    
    if (ImGui::BeginPopupModal("EditHoldingModal", nullptr,
                               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                               ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar))
    {
      Holding& h = holdings[g_editIndex];
      static char symbolBuf[32];
      static float avgPrice;
      static int quantity;
      static int selectedVision;
      static bool initialized = false;
      
      if (!initialized)
      {
        strcpy(symbolBuf, h.symbol.c_str());
        avgPrice = h.averagePrice;
        quantity = h.quantity;
        selectedVision = static_cast<int>(h.vision);
        initialized = true;
      }
      
      KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundLight, 30);
      KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_22), "Edit Holding", KanVasX::UI::AlignX::Center);
      KanVasX::UI::ShiftCursorY(10.0f);
      
      const char* visions[] = {"Long Term", "Mid Term", "Short Term"};
      float availX = ImGui::GetContentRegionAvail().x;
      
      ImGui::PushItemWidth(availX * 0.19); ImGui::InputTextWithHint("##symbol", "Symbol", symbolBuf, IM_ARRAYSIZE(symbolBuf)); ImGui::SameLine();
      ImGui::PushItemWidth(availX * 0.19); ImGui::InputFloat("##avg", &avgPrice, 0, 0, "%.2f"); ImGui::SameLine();
      ImGui::PushItemWidth(availX * 0.19); ImGui::InputInt("##qty", &quantity); ImGui::SameLine();
      ImGui::PushItemWidth(availX * 0.19); ImGui::Combo("##vision", &selectedVision, visions, IM_ARRAYSIZE(visions)); ImGui::SameLine();
      
      if (ImGui::Button("Save", ImVec2(60, 0)) || ImGui::IsKeyPressed(ImGuiKey_Enter))
      {
        h.symbol = symbolBuf;
        h.averagePrice = avgPrice;
        h.quantity = quantity;
        h.vision = static_cast<Holding::Vision>(selectedVision);
        
        PortfolioController portfolioController(*portfolio);
        portfolioController.EditHolding(h);

        UserManager::GetCurrentUser().SavePortfolio();
        initialized = false;
        ImGui::CloseCurrentPopup();
      }
      
      ImGui::SameLine();
      if (ImGui::Button("Cancel", ImVec2(80, 0)) || ImGui::IsKeyPressed(ImGuiKey_Escape))
      {
        initialized = false;
        ImGui::CloseCurrentPopup();
      }
      
      ImGui::EndPopup();
    }
  }

} // namespace KanVest
