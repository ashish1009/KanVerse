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
    
    // Show Chart
    KanVasX::UI::ShiftCursorY(ImGui::GetContentRegionAvail().y * 0.5f);
    if (ImGui::BeginChild(" Stock - Data - Chart ", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y)))
    {
      Chart::Show(stockData);
      ImGui::EndChild();
    }
    
    // Frame Rate
    KanVasX::UI::Text(Font(Regular), Utils::FormatDoubleToString(ImGui::GetIO().Framerate), Align::Right, {0.0f, ImGui::GetContentRegionAvail().y - 18.0f}, Color::Gray);

    KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
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
      StockManager::AddRequest(s_selectedStockSymbol, Range::_1D, Interval::_5M);
    }
  }
} // namespace KanVest::UI
