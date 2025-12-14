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
    
    ImU32 changeColor = stockData.change > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;
    ImGui::SameLine();
    KanVasX::UI::Text(Font(Header_30), change, Align::Left, {20.0f, 15.0f}, changeColor);

  }
} // namespace KanVest::UI
