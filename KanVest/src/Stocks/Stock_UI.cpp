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
  } // namespace Utils
  
  void StockUI::Initialize(ImTextureID reloadIconID)
  {
    s_reloadIconID = reloadIconID;
  }

  void StockUI::StockAnalyzer()
  {
    KanVasX::Panel::Begin("Stock Analyzer");
    {
      static StockData stockData{""};
      
//      KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundDark, 40, 0.22);
      
      KanVasX::UI::ShiftCursor({5.0f, 5.0f});
      
      static char searchedString[128];
      const float contentRegionAvail = ImGui::GetContentRegionAvail().x;
      if (KanVasX::Widget::Search(searchedString, 128, KanVasX::Settings::FrameHeight, contentRegionAvail * 0.2f,
                                  "Enter Symbol ...", UI::Font::Get(UI::FontType::Large) , true))
      {
        Utils::ConvertUpper(searchedString);
      }
      
      ImGui::SameLine();
      float iconSize = ImGui::GetItemRectSize().y - 12;
      if (KanVasX::UI::DrawButtonImage("Refresh", s_reloadIconID, false, {iconSize, iconSize}, {-8.0, 6.0}) or
          ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Enter))
      {
        stockData = StockController::UpdateStockData(searchedString);
      }
      
      if (stockData.livePrice != -1)
      {
        ImGui::Text("Name  : %s", stockData.symbol.c_str());
        ImGui::Text("Price : %f", stockData.livePrice);
      }
    }
    KanVasX::Panel::End();
  }
} // namespace KanVest
