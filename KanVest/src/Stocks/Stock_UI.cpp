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
      static bool ValidData = false;
            
      KanVasX::UI::ShiftCursor({0.0f, 5.0f});
      
      static char searchedString[128] = "BEL";
      const float contentRegionAvail = ImGui::GetContentRegionAvail().x;
      if (KanVasX::Widget::Search(searchedString, 128, KanVasX::Settings::FrameHeight, contentRegionAvail * 0.20f,
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
        ValidData = true;
      }
      
      KanVasX::UI::ShiftCursorY(5.0f);
      
      const float availY = ImGui::GetContentRegionAvail().y;
      KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundLight, availY, 0.198);

      auto Format2 = [](double value) {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%.2f", value);
        return std::string(buf);
      };
      
      if (ValidData)
      {
        KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_34), stockData.symbol.c_str(), KanVasX::UI::AlignX::Left, {30.0f, 20.0f}, KanVasX::Color::TextBright);
        KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_44), Format2(stockData.livePrice), KanVasX::UI::AlignX::Left, {30.0f, 10.0f}, KanVasX::Color::TextBright);
      }
    }
    KanVasX::Panel::End();
  }
} // namespace KanVest
