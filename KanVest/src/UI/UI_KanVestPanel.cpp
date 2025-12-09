//
//  UI_KanVestPanel.cpp
//  KanVest
//
//  Created by Ashish . on 02/12/25.
//

#include "UI_KanVestPanel.hpp"

#include "UI/UI_Utils.hpp"

#include "Stock/StockManager.hpp"

namespace KanVest::UI
{
#define Font(font) KanVest::UI::Font::Get(KanVest::UI::FontType::font)
  
  using Align = KanVasX::UI::AlignX;
  using Color = KanVasX::Color;

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
    KanVasX::Panel::Begin("Stock Analyzer");
    
    // Search Bar
    ShowStockSearchBar();
    
    {
      static constexpr std::string symbolName = "Nifty";
      StockData stockData = StockManager::GetLatest(symbolName);
      if (!stockData.IsValid())
      {
        std::string errorMessage = "Invalid stock data for " + symbolName;
        KanVasX::UI::Text(Font(Header_22), errorMessage, Align::Left, {0.0f, 0.0f}, KanVasX::Color::Error);
      }
      else
      {
        ImGui::Text("symbol  :%s ", stockData.symbol.c_str());
        ImGui::Text("currency  :%s ", stockData.currency.c_str());
        ImGui::Text("exchangeName  :%s ", stockData.exchangeName.c_str());
        ImGui::Text("shortName  :%s ", stockData.shortName.c_str());
        ImGui::Text("longName  :%s ", stockData.longName.c_str());
        ImGui::Text("instrumentType  :%s ", stockData.instrumentType.c_str());
        ImGui::Text("timezone  :%s ", stockData.timezone.c_str());
        ImGui::Text("range  :%s ", stockData.range.c_str());
        ImGui::Text("dataGranularity  :%s ", stockData.dataGranularity.c_str());
  
        ImGui::Text("livePrice : %f ", stockData.livePrice);
        ImGui::Text("prevClose : %f ", stockData.prevClose);
  
        ImGui::Text("change : %f ", stockData.change);
        ImGui::Text("changePercent : %f ", stockData.changePercent);
  
        ImGui::Text("volume : %f ", stockData.volume);
  
        ImGui::Text("fiftyTwoHigh : %f ", stockData.fiftyTwoHigh);
        ImGui::Text("fiftyTwoLow : %f ", stockData.fiftyTwoLow);
        ImGui::Text("dayHigh : %f ", stockData.dayHigh);
        ImGui::Text("dayLow : %f ", stockData.dayLow);
      }
    }
    
    // Frame Rate
    KanVasX::UI::Text(Font(Regular), Utils::FormatDoubleToString(ImGui::GetIO().Framerate), Align::Right, {0.0f, ImGui::GetContentRegionAvail().y - 18.0f}, Color::Gray);

    KanVasX::Panel::End(0.0f);
  }
  
  void Panel::ShowStockSearchBar()
  {
    IK_PERFORMANCE_FUNC("Panel::ShowSearchBar");
    
    if (KanVasX::Widget::Search(s_searchedStockString, 128, 8.0f, ImGui::GetContentRegionAvail().x, "Enter Symbol ...", Font(Large), 40.0f))
    {
      KanViz::Utils::String::ToUpper(s_searchedStockString);
    }
    
    if (ImGui::IsKeyPressed(ImGuiKey_Enter))
    {
      StockManager::AddRequest("Nifty", Range::_1D, Interval::_5M);
    }
  }
} // namespace KanVest::UI
