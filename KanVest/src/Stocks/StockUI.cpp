//
//  StockUI.cpp
//  KanVest
//
//  Created by Ashish . on 05/11/25.
//

#include "StockUI.hpp"

namespace KanVest
{
  namespace Utils
  {
    void ConvertUpper(char* string)
    {
      for (char* p = string; *p; ++p)
        *p = static_cast<char>(toupper(*p));
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
          ShowWatchlist();
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
  }
  void StockUI::ShowPortfolio()
  {
    if (ImGui::BeginChild("Portfolio", ImVec2(-1, ImGui::GetContentRegionAvail().y * 0.6))) // fixed height
    {
      
    }
    ImGui::EndChild();
  }
  void StockUI::ShowWatchlist()
  {
    
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

    }
  }
} // namespace KanVest
