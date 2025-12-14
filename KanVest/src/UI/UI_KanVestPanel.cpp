//
//  UI_KanVestPanel.cpp
//  KanVest
//
//  Created by Ashish . on 02/12/25.
//

#include "UI_KanVestPanel.hpp"

#include "UI/UI_Utils.hpp"
#include "UI/UI_Chart.hpp"

#include "Stock/StockManager.hpp"

namespace KanVest::UI
{
#define Font(font) KanVest::UI::Font::Get(KanVest::UI::FontType::font)
  
  using Align = KanVasX::UI::AlignX;
  using Color = KanVasX::Color;

  void Panel::Initialize()
  {
    s_selectedStockSymbol = "Nifty";
    StockManager::AddRequest(s_selectedStockSymbol, Range::_1D, Interval::_2M);
  }
  
  void Panel::SetShadowTextureId(ImTextureID shadowTextureID)
  {
    s_shadowTextureID = shadowTextureID;
  }
  void Panel::SetReloadTextureId(ImTextureID reloadIconID)
  {
    s_reloadIconID = reloadIconID;
  }
  void Panel::SetSettingTextureId(ImTextureID settingIconID)
  {
    s_settingIconID = settingIconID;
  }
  void Panel::SetOpenEyeTextureId(ImTextureID openEyeTextureID)
  {
    s_openEyeTextureID = openEyeTextureID;
  }
  void Panel::SetCloseEyeTextureId(ImTextureID closeEyeTextureID)
  {
    s_closeEyeTextureID = closeEyeTextureID;
  }

  void Panel::Show()
  {
    IK_PERFORMANCE_FUNC("Panel::Show");
    
    ImGui::Begin("Stock Analyzer");
    
    // Search Bar
    float avalaWidth = ImGui::GetContentRegionAvail().x;
    KanVasX::UI::ShiftCursorX(avalaWidth * 0.4f);
    ShowStockSearchBar(avalaWidth * 0.2f, 8.0f);
    
    // Get Stock selected data
    StockData stockData = StockManager::GetLatest(s_selectedStockSymbol);
    
    // Stock Basic Information
    {
      KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, Color::Alpha(Color::BackgroundLight, 0.2f));
      if (ImGui::BeginChild(" Stock - Chart ", ImVec2(ImGui::GetContentRegionAvail().x * 0.3f, ImGui::GetContentRegionAvail().y * 0.5f)))
      {
        ShowStockData(stockData);
        KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
        ImGui::EndChild();
      }
    }
    
    // Show Chart
    {
      KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, Color::Null);
      if (ImGui::BeginChild(" Stock - Data ", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y)))
      {
        Chart::Show(stockData);
        ImGui::EndChild();
      }
    }
    
    // Frame Rate
    KanVasX::UI::Text(Font(Regular), Utils::FormatDoubleToString(ImGui::GetIO().Framerate), Align::Right, {0.0f, ImGui::GetContentRegionAvail().y - 18.0f}, Color::Gray);

    ImGui::End();
  }
  
  void Panel::ShowStockSearchBar(float width, float height)
  {
    IK_PERFORMANCE_FUNC("Panel::ShowSearchBar");
    
    if (KanVasX::Widget::Search(s_searchedStockString, 128, height, width, "Enter Symbol ...", Font(Large), 40.0f))
    {
      KanViz::Utils::String::ToUpper(s_searchedStockString);
    }
    
    if (ImGui::IsKeyPressed(ImGuiKey_Enter))
    {
      s_selectedStockSymbol = s_searchedStockString;
      StockManager::AddRequest(s_selectedStockSymbol, Range::_1D, API_Provider::GetValidIntervalForRange(Range::_1D));
    }
  }
  
  void Panel::ShowStockData(const StockData& stockData)
  {
    IK_PERFORMANCE_FUNC("Panel::ShowStockData");
    
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(Font(Header_24), "No data for stock", Align::Left, {20.0f, 10.0f}, Color::Error);
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
} // namespace KanVest::UI
