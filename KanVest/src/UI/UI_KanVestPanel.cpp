//
//  UI_KanVestPanel.cpp
//  KanVest
//
//  Created by Ashish . on 08/01/26.
//

#include "UI_KanVestPanel.hpp"

#include "Stock/StockManager.hpp"

#include "Analyzer/StockAnalyzer.hpp"

#include "UI/UI_Utils.hpp"
#include "UI/UI_Chart.hpp"
#include "UI/UI_MovingAverage.hpp"
#include "UI/UI_Momentum.hpp"

namespace KanVest::UI
{
#define Font(font) KanVest::UI::Font::Get(KanVest::UI::FontType::font)
  
  using Align = KanVasX::UI::AlignX;
  using Color = KanVasX::Color;

  void Panel::SetShadowTextureId(ImTextureID shadowTextureID)
  {
    s_shadowTextureID = shadowTextureID;
  }

  void Panel::Show()
  {
    IK_PERFORMANCE_FUNC("Panel::Show");
    
    ImGui::Begin("Stock Analyzer");
    
    // Update selected stock data
    UpdateSelectedStock();

    // Get Stock Data
    StockData stockData = StockManager::GetLatestStockData(s_selectedStockSymbol);
    
    // Update if stock is changed
    s_stockChanged = s_lastSymbol != stockData.symbol || s_lastRange != stockData.range || s_lastInterval != stockData.dataGranularity;
    if (s_stockChanged)
    {
      s_lastSymbol   = stockData.symbol;
      s_lastRange    = stockData.range;
      s_lastInterval = stockData.dataGranularity;
    }

    // Analyze Stock
    if (s_stockChanged)
    {
      Analyzer::AnalzeStock(stockData);
    }

    // Show Stock Data
    ImGui::BeginChild(" Stock - Data - Analyzer ", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.4f));
    {
      float availableX = ImGui::GetContentRegionAvail().x;

      // Col 1 :
      {
        ImGui::BeginChild(" Stock - Data ", ImVec2(availableX * 0.3f, ImGui::GetContentRegionAvail().y));
        {
          ShowStockSearchBar(ImGui::GetContentRegionAvail().x - 5.0f /* Padding */ , 8.0f);
          ShowStockData(stockData);
          ShowStockAnalyzer(stockData);

          KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
        }
        ImGui::EndChild();
      }
      ImGui::SameLine();
      
      // Col 2 :
      {
        KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, Color::Null);
        ImGui::BeginChild(" Stock - Search ", ImVec2(availableX * 0.39f, ImGui::GetContentRegionAvail().y));
        {
        }
        ImGui::EndChild();
      }
      ImGui::SameLine();
      
      // Col 3 :
      {
        ImGui::BeginChild(" Stock - Analyzer ", ImVec2(availableX * 0.3f, ImGui::GetContentRegionAvail().y));
        {
          ShowStockTechnicals(stockData);
          KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
        }
        ImGui::EndChild();
      }
    }
    ImGui::EndChild();
            
    // Show Chart
    ImGui::BeginChild(" Chart", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y));
    {
      // Show chart UI
      Chart::Show(stockData);
    }
    ImGui::EndChild();

    ImGui::End();
  }
  
  void Panel::UpdateSelectedStock()
  {
    // Update stock if new data entered
    if (s_selectedStockSymbol != s_searchedStockString and ImGui::IsKeyPressed(ImGuiKey_Enter))
    {
      // Get previous symbol stock data
      const auto& prevStockData = StockManager::GetLatestStockData(s_selectedStockSymbol);
      
      // Get default range and interval
      Range range = Range::_1Y;
      Interval interval = API_Provider::GetOptimalIntervalForRange(range);
      
      // Update range with previous range if data present
      if (prevStockData.IsValid())
      {
        range = API_Provider::GetRangeEnumFromString(prevStockData.range);
        interval = API_Provider::GetIntervalEnumFromString(prevStockData.dataGranularity);
      }
      
      // Add new symbol in stock data extracter
      s_selectedStockSymbol = s_searchedStockString;
      StockManager::AddStockDataRequest(s_selectedStockSymbol, range, interval);
    }
  }
  
  void Panel::ShowStockSearchBar(float width, float height)
  {
    // Search bar
    if (KanVasX::Widget::Search(s_searchedStockString, 128, height, width, "Enter Symbol ...", Font(Large), 40.0f))
    {
      KanViz::Utils::String::ToUpper(s_searchedStockString);
    }
  }
  
  void Panel::ShowStockData(const StockData& stockData)
  {
    if (!stockData.IsValid())
    {
      std::string ErrorMessage = "No data available for symbol " + stockData.symbol;
      KanVasX::UI::Text(Font(Header_24), ErrorMessage, Align::Left, {20.0f, 10.0f}, Color::Error);
      return;
    }
    
    // Name & price
    std::string name = stockData.shortName;
    std::string longNameName = stockData.longName != "" ? stockData.longName : stockData.shortName;
    
    KanVasX::UI::Text(Font(Header_46), name, Align::Left, {20.0f, 10.0f}, Color::TextBright);
    KanVasX::UI::Text(Font(Header_26), longNameName, Align::Left, {20.0f, 0.0f}, Color::TextBright);
    KanVasX::UI::Text(Font(Header_56), KanVest::UI::Utils::FormatDoubleToString(stockData.livePrice), Align::Left, {20.0f, 0.0f}, Color::TextBright);
    
    // Change
    std::string change = (stockData.change > 0 ? "+" : "") +
    KanVest::UI::Utils::FormatDoubleToString(stockData.change) +
    (stockData.change > 0 ? " ( +" : " ( ") +
    KanVest::UI::Utils::FormatDoubleToString(stockData.changePercent) + "%)";
    
    ImU32 changeColor = stockData.change > 0 ? Color::Cyan : Color::Red;
    ImGui::SameLine();
    KanVasX::UI::Text(Font(Header_30), change, Align::Left, {20.0f, 15.0f}, changeColor);
    
    // Progress bar
    auto ShowPriceProgress = [](float low, float high, float currentPrice)
    {
      // Avoid division by zero
      if (high <= low) return;
      
      // Calculate fraction for progress bar (0.0 = low, 1.0 = high)
      float fraction = (currentPrice - low) / (high - low);
      fraction = std::clamp(fraction, 0.0f, 1.0f);
      
      {
        // Choose color based on fraction
        ImU32 scoreColor;
        if (fraction < 0.15)      { scoreColor = Color::Red; }
        else if (fraction < 0.30) { scoreColor = Color::Orange; }
        else if (fraction < 0.60) { scoreColor = Color::Yellow; }
        else if (fraction < 0.80) { scoreColor = Color::Cyan; }
        else                    { scoreColor = Color::Green; }
        
        KanVasX::ScopedColor plotColor(ImGuiCol_PlotHistogram, scoreColor);
        KanVasX::ScopedColor frameColor(ImGuiCol_FrameBg, Color::Gray);
        
        ImGui::ProgressBar(fraction, ImVec2(ImGui::GetContentRegionAvail().x -20.0f, 0), " ");
        
        // Change percent string
        std::string percentString = UI::Utils::FormatDoubleToString(fraction * 100) + "%";
        KanVasX::UI::Tooltip(percentString);
        
        // Draw low/high text on top of the bar
        ImVec2 p = ImGui::GetItemRectMin();   // top-left of bar
        ImVec2 size = ImGui::GetItemRectSize();
        
        ImGui::SetCursorScreenPos(p);
        KanVasX::UI::Text(Font(Header_22), KanVest::UI::Utils::FormatDoubleToString(low), Align::Left, {5.0f, 0.0f}, Color::Black);
        
        ImGui::SetCursorScreenPos(ImVec2(p.x + size.x - ImGui::CalcTextSize(std::to_string((int)high).c_str()).x, p.y));
        KanVasX::UI::Text(Font(Header_22), KanVest::UI::Utils::FormatDoubleToString(high), Align::Right, {-20.0f, 0.0f}, Color::Black);
      }
    };
    
    KanVasX::UI::DrawFilledRect(Color::Separator, 1);
    KanVasX::UI::ShiftCursorY(5.0f);
    
    // 52 - Week change
    KanVasX::UI::Text(Font(FixedWidthHeader_18), "52-Week " , Align::Left, {20.0f, 10.0f}, Color::Text);
    ImGui::SameLine();
    KanVasX::UI::ShiftCursorY(-5.0f);
    ShowPriceProgress(stockData.fiftyTwoLow, stockData.fiftyTwoHigh, stockData.livePrice);
    
    // Day change
    KanVasX::UI::Text(Font(FixedWidthHeader_18), "Day     " , Align::Left, {20.0f, 10.0f}, Color::Text);
    ImGui::SameLine();
    KanVasX::UI::ShiftCursorY(-5.0f);
    ShowPriceProgress(stockData.dayLow, stockData.dayHigh, stockData.livePrice);
    
    // Volume
    KanVasX::UI::Text(Font(FixedWidthHeader_18), "Volume  " , Align::Left, {20.0f, 10.0f}, Color::Text);
    ImGui::SameLine();
    KanVasX::UI::Text(Font(FixedWidthHeader_18), Utils::FormatLargeNumber(stockData.volume) , Align::Left, {0.0f, 0.0f}, Color::Text);
    
    KanVasX::UI::DrawFilledRect(Color::Separator, 1);
  }
  
  void Panel::ShowStockTechnicals(const StockData& stockData)
  {
    if (!stockData.IsValid())
    {
      std::string ErrorMessage = "No data available for symbol " + stockData.symbol;
      KanVasX::UI::Text(Font(Header_24), ErrorMessage, Align::Left, {20.0f, 10.0f}, Color::Error);
      return;
    }
    
    enum class TechnicalTab {DMA, EMA, RSI, Max};
    static TechnicalTab tab = TechnicalTab::DMA;

    float availX = ImGui::GetContentRegionAvail().x;
    float technicalButtonSize = (availX / (uint32_t)TechnicalTab::Max) - 10.0f;
    
    auto TechnicalButton = [technicalButtonSize](const std::string& title, TechnicalTab checkerTtab, const std::string& tooltip)
    {
      const auto& buttonColor = tab == checkerTtab ? Color::Button : Color::Background;
      if (KanVasX::UI::DrawButton(title, Font(Medium), buttonColor, Color::TextBright, false, 10.0f, {technicalButtonSize, 30}))
      {
        tab = checkerTtab;
      }
      KanVasX::UI::Tooltip(tooltip);
      KanVasX::UI::DrawItemActivityOutline();
    };
    
    KanVasX::UI::ShiftCursor({2.0f, 5.0f});
    TechnicalButton("DMA", TechnicalTab::DMA, "Daily Moving Average"); ImGui::SameLine();
    TechnicalButton("EMA", TechnicalTab::EMA, "Exponantial Moving Average"); ImGui::SameLine();
    TechnicalButton("RSI", TechnicalTab::RSI, "Relative Strength Indicator");

    if (tab == TechnicalTab::DMA)
    {
      UI_MovingAverage::ShowDMA(stockData, s_shadowTextureID);
    }
    else if (tab == TechnicalTab::EMA)
    {
      UI_MovingAverage::ShowEMA(stockData, s_shadowTextureID);
    }
    else if (tab == TechnicalTab::RSI)
    {
      UI_Momentum::ShowRSI(stockData);
    }
  }
  
  void Panel::ShowStockAnalyzer(const StockData& stockData)
  {
    if (!stockData.IsValid())
    {
      return;
    }
    
    // Get analyzer report
    const auto& stockReport = Analyzer::GetReport();
    
    // Show Score
    float score = static_cast<float>(stockReport.score);
    
    // Determine color based on score
    ImU32 scoreColor;
    std::string scoreString = "Technically Neutral";
    
    if (score < 10)      { scoreString = "Technically Strong Bearish";      scoreColor = KanVasX::Color::Red; }
    else if (score < 25) { scoreString = "Technically Bearish";             scoreColor = KanVasX::Color::Orange; }
    else if (score < 40) { scoreString = "Technically Moderate Bearish";    scoreColor = KanVasX::Color::Orange; }
    else if (score < 60) { scoreString = "Technically Neutral";             scoreColor = KanVasX::Color::Yellow; }
    else if (score < 75) { scoreString = "Technically Moderate Bullish";    scoreColor = KanVasX::Color::Green; }
    else if (score < 90) { scoreString = "Technically Bullish";             scoreColor = KanVasX::Color::Green; }
    else                 { scoreString = "Technically Strong Bullish";      scoreColor = KanVasX::Color::Green; }
    
    // Scoped color
    {
      KanVasX::ScopedStyle headerPaddingAndHeight(ImGuiStyleVar_FramePadding, ImVec2{1.0f, 1.0f});
      KanVasX::ScopedColor plotColor(ImGuiCol_PlotHistogram, scoreColor);
      // Convert score 0-100 to fraction 0.0-1.0
      
      // Print Score
      KanVasX::UI::ShiftCursorX(10.0f);
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_32), Utils::FormatDoubleToString(score), KanVasX::UI::AlignX::Left, {10.0f, 0}, scoreColor);
      
      // Print /100
      static const std::string totalScoreString = "/100";
      ImGui::SameLine();
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Medium), totalScoreString, KanVasX::UI::AlignX::Left, {0, 10.0f}, KanVasX::Color::White);
      
      ImGui::SameLine();
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Large), scoreString, KanVasX::UI::AlignX::Right, {-20.0f, 5.0f}, scoreColor);
      
      float fraction = score / 100.0f;
      KanVasX::UI::ShiftCursorX(20.0f);
      ImGui::ProgressBar(fraction, ImVec2(ImGui::GetContentRegionAvail().x - 20.0f, 0), "");
    }
  }
} // namespace KanVest::UI
