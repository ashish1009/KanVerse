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
      const float contentRegionAvailX = ImGui::GetContentRegionAvail().x;
      const float contentRegionAvailY = ImGui::GetContentRegionAvail().y;;

      // 🔒 Non-resizable, borderless, fixed-size table
      if (ImGui::BeginTable("StockAnalyzerTable", 3, ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingFixedFit))
      {
        // Check if data captured
        static bool ValidData = false;

        // Stock Data
        static StockData stockData{""};

        // History date range
        static KanVasX::Date startDate{2024, 1, 1};
        static KanVasX::Date endDate{2024, 3, 31};
        
        // UI
        float totalWidth = ImGui::GetContentRegionAvail().x;
        float firstColWidth = totalWidth * 0.3f;
        float secondColWidth = totalWidth * 0.4f;
        float thirdColWidth = totalWidth * 0.3f;
        
        ImGui::TableSetupColumn("Stock Info", ImGuiTableColumnFlags_WidthFixed, firstColWidth);
        ImGui::TableSetupColumn("Chart", ImGuiTableColumnFlags_WidthFixed, secondColWidth);
        ImGui::TableSetupColumn("Analytics", ImGuiTableColumnFlags_WidthFixed, thirdColWidth);
        
        ImGui::TableNextRow();
        
        // ============================================================
        // 🧭 COLUMN 1 : Stock Details
        // ============================================================
        {
          ImGui::TableSetColumnIndex(0);
          KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundLight, ImGui::GetContentRegionAvail().y * 0.75f, 0.295);
          
          static char searchedString[128] = "BEL";
          
          if (KanVasX::Widget::Search(searchedString, 128, KanVasX::Settings::FrameHeight, contentRegionAvailX * 0.2839f, "Enter Symbol ...", UI::Font::Get(UI::FontType::Large), true))
          {
            Utils::ConvertUpper(searchedString);
          }
          
          ImGui::SameLine();
          float iconSize = ImGui::GetItemRectSize().y - 12;
          if (KanVasX::UI::DrawButtonImage("Refresh", s_reloadIconID, false, {iconSize, iconSize}, {-8.0, 6.0}) ||
              ImGui::IsKeyDown(ImGuiKey_Enter))
          {
            time_t startTime = StockParser::ParseDateYYYYMMDD(startDate.ToString());
            time_t endTime = StockParser::ParseDateYYYYMMDD(endDate.ToString());
            
            if (startTime == 0 || endTime == 0 || endTime <= startTime)
            {
              endTime = time(nullptr);
              startTime = endTime - 60*60*24*90;
            }
            
            // Use period1= start (UTC), period2 = end (UTC) + one day to include end date
            long period1 = static_cast<long>(startTime);
            long period2 = static_cast<long>(endTime + 60*60*24);

            stockData = StockController::UpdateStockData(searchedString, std::to_string(period1), std::to_string(period2));

            ValidData = true;
          }
          
          KanVasX::UI::ShiftCursorY(8.0f);
          if (ValidData)
          {
            using namespace UI;
            static const auto &textColor = KanVasX::Color::TextBright;
            static const auto &alignLeft = KanVasX::UI::AlignX::Left;
            
            // Stock Name and Value
            std::string name = stockData.shortName + " (" + stockData.currency + ")";
            KanVasX::UI::Text(Font::Get(FontType::Header_36), name.c_str(), alignLeft, {30.0f, 10.0f}, textColor);
            KanVasX::UI::Text(Font::Get(FontType::Header_48), Utils::FormatDoubleToString(stockData.livePrice), alignLeft, {30.0f, 10.0f}, textColor);
            
            // Change
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
            std::string fiftytwoWeek = Utils::FormatDoubleToString(stockData.fiftyTwoLow) + " - " +
            Utils::FormatDoubleToString(stockData.fiftyTwoHigh);
            KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), "52-Week Range ", alignLeft, {30.0f, 10.0f}, textColor);
            ImGui::SameLine();
            KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), fiftytwoWeek, alignLeft, {50.0f, 0.0f}, textColor);
            
            // Day
            std::string dayRange = Utils::FormatDoubleToString(stockData.dayLow) + " - " +
            Utils::FormatDoubleToString(stockData.dayHigh);
            KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), "Day Range     ", alignLeft, {30.0f, 10.0f}, textColor);
            ImGui::SameLine();
            KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), dayRange, alignLeft, {50.0f, 0.0f}, textColor);
            
            // Voldume
            KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), "Volume        ", alignLeft, {30.0f, 10.0f}, textColor);
            ImGui::SameLine();
            KanVasX::UI::Text(Font::Get(FontType::FixedWidthHeader_20), Utils::FormatLargeNumber(stockData.volume), alignLeft, {50.0f, 0.0f}, textColor);
            
            KanVasX::UI::ShiftCursorY(10);
            KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, 0.2, {20.0, 5.0});
          }
        }
        
        // ============================================================
        // 📊 COLUMN 2 : Chart
        // ============================================================
        {
          ImGui::TableSetColumnIndex(1);
          KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundLight, ImGui::GetContentRegionAvail().y * 0.75f, 0.395);
          
          KanVasX::Date::RangeSelectorUI(startDate, endDate);
        }
        
        // ============================================================
        // 📊 COLUMN 2 : Analytics / Date Range
        // ============================================================
        {
          ImGui::TableSetColumnIndex(2);
          KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundLight, ImGui::GetContentRegionAvail().y * 0.75f, 0.295);
        }
      }
      ImGui::EndTable();

      {
        KanVasX::UI::SetCursorPosY(contentRegionAvailY * 0.765f);
        KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundLight, ImGui::GetContentRegionAvail().y);
      }
      KanVasX::Panel::End();
    }
  }


} // namespace KanVest
