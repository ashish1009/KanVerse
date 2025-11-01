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
      if (KanVasX::Widget::Search(searchedString, 128, KanVasX::Settings::FrameHeight, contentRegionAvail * 0.30f,
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
      KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundLight, availY, 0.298);
      
      if (ValidData)
      {
        static const auto& textColor = KanVasX::Color::TextBright;
        static const auto& alignLeft = KanVasX::UI::AlignX::Left;
        using namespace UI;
 
        std::string name = stockData.shortName + " (" + stockData.currency + ")";
        
        // Namme
        KanVasX::UI::Text(Font::Get(FontType::Header_36), name.c_str(), alignLeft, {30.0f, 10.0f}, textColor);
        
        // Price
        KanVasX::UI::Text(Font::Get(FontType::Header_48), Utils::FormatDoubleToString(stockData.livePrice), alignLeft, {30.0f, 10.0f}, textColor);
        
        // Change and percent
        std::string change = stockData.change > 0 ? "+" : "";
        change += Utils::FormatDoubleToString(stockData.change);
        change += stockData.change > 0 ? " ( +" : " ( ";
        change += Utils::FormatDoubleToString(stockData.changePercent) + "%)";
        
        ImU32 changeColor = stockData.change > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;
        KanVasX::UI::Text(Font::Get(FontType::Header_30), change, alignLeft, {30.0f, 10.0f}, changeColor);

        // Under line
        KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, 0.2, {20.0, 5.0});
        KanVasX::UI::ShiftCursorY(10);
        
        // 52-Week
        std::string fiftytwoWeek = Utils::FormatDoubleToString(stockData.fiftyTwoLow) + " - " + Utils::FormatDoubleToString(stockData.fiftyTwoHigh);
        KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), "52-Week Range ", alignLeft, {30.0f, 10.0f}, textColor);
        ImGui::SameLine();
        KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), fiftytwoWeek, alignLeft, {50.0f, 0.0f}, textColor);

        // Day
        std::string datRange = Utils::FormatDoubleToString(stockData.dayLow) + " - " + Utils::FormatDoubleToString(stockData.dayHigh);
        KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), "Day Range     ", alignLeft, {30.0f, 10.0f}, textColor);
        ImGui::SameLine();
        KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), datRange, alignLeft, {50.0f, 0.0f}, textColor);

        // Volume
        KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), "Volume        ", alignLeft, {30.0f, 10.0f}, textColor);
        ImGui::SameLine();
        KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), Utils::FormatLargeNumber(stockData.volume), alignLeft, {50.0f, 0.0f}, textColor);
        
        // Under line
        KanVasX::UI::ShiftCursorY(10);
        KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, 0.2, {20.0, 5.0});
      }
    }
    KanVasX::Panel::End();
  }
} // namespace KanVest
