//
//  KanVestPanel.cpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
//

#include "KanVestPanel.hpp"

namespace KanVest::UI
{
  void Panel::SetShadowTextureId(ImTextureID shadowTextureID)
  {
    s_shadowTextureID = shadowTextureID;
  }

  void Panel::Show()
  {
    IK_PERFORMANCE_FUNC("StockUI::StockPanel");
    
    KanVasX::Panel::Begin("Stock Analyzer");

    KanVasX::ScopedColor childColor(ImGuiCol_ChildBg, KanVasX::Color::BackgroundDark);

    {
      float totalWidth  = ImGui::GetContentRegionAvail().x;
      float totalHeight = ImGui::GetContentRegionAvail().y;
      float childHeight = totalHeight * 0.4985f;
      
      if (ImGui::BeginChild(" Stock-Analyzer ", ImVec2(totalWidth, childHeight)))
      {
        ShowStockAnalyzerTable();
      }
      ImGui::EndChild();
      
      if (ImGui::BeginChild(" Portfolio-Chart ", ImVec2(totalWidth, 0)))
      {
        ShowPortfolioChartTable();
      }
      ImGui::EndChild();
    }
    
    KanVasX::Panel::End(0);
  }
  
  void Panel::ShowStockAnalyzerTable()
  {
    if (ImGui::BeginTable("StockAnalyzerTable", 3, ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingFixedFit))
    {
      float totalHeight = ImGui::GetContentRegionAvail().y;
      float totalWidth = ImGui::GetContentRegionAvail().x;
      
      float tableHeight = totalHeight * 0.992;
      float firstColWidth = totalWidth * 0.33;
      float secondColWidth = totalWidth * 0.33;
      float thirtColWidth = totalWidth - firstColWidth -  secondColWidth;
      
      ImGui::TableSetupColumn("Stock-Data", ImGuiTableColumnFlags_WidthFixed, firstColWidth);
      ImGui::TableSetupColumn("Stock-Analyzer", ImGuiTableColumnFlags_WidthFixed, secondColWidth);
      ImGui::TableSetupColumn("Stock-Summary", ImGuiTableColumnFlags_WidthFixed, thirtColWidth);

      ImGui::TableNextRow();
      
      // Column 1 Stock Details
      {
        ImGui::TableSetColumnIndex(0);
        if (ImGui::BeginChild("Stock-Data-Cell", ImVec2(firstColWidth, tableHeight))) // fixed height
        {
          KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
        }
        ImGui::EndChild();
      }
      
      // Column 2 Analyzer
      {
        ImGui::TableSetColumnIndex(1);
        if (ImGui::BeginChild("Stock-Analyzer-Cell", ImVec2(secondColWidth, tableHeight))) // fixed height
        {
          KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
        }
        ImGui::EndChild();
      }

      // Column 3 Summary
      {
        ImGui::TableSetColumnIndex(2);
        if (ImGui::BeginChild("Stock-Summary-Cell", ImVec2(thirtColWidth * 0.978, tableHeight))) // fixed height
        {
          KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
        }
        ImGui::EndChild();
      }

      ImGui::EndTable();
    }
  }
  
  void Panel::ShowPortfolioChartTable()
  {
    if (ImGui::BeginTable("PortfolioChartTable", 2, ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingFixedFit))
    {
      float totalHeight = ImGui::GetContentRegionAvail().y;
      float totalWidth = ImGui::GetContentRegionAvail().x;
      
      float tableHeight = totalHeight * 0.992;
      float firstColWidth = totalWidth * 0.5;
      float secondColWidth = totalWidth - firstColWidth;
      
      ImGui::TableSetupColumn("Chart", ImGuiTableColumnFlags_WidthFixed, firstColWidth);
      ImGui::TableSetupColumn("Portfolio", ImGuiTableColumnFlags_WidthFixed, secondColWidth);
      
      ImGui::TableNextRow();

      // Column 1 Chart
      {
        ImGui::TableSetColumnIndex(0);
        if (ImGui::BeginChild("Chart-Cell", ImVec2(firstColWidth, tableHeight))) // fixed height
        {
          KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
        }
        ImGui::EndChild();
      }
      
      // Column 2 Portfolio
      {
        ImGui::TableSetColumnIndex(1);
        if (ImGui::BeginChild("Portfolio-Cell", ImVec2(secondColWidth * 0.99, tableHeight))) // fixed height
        {
          KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
        }
        ImGui::EndChild();
      }
      ImGui::EndTable();
    }
  }
} // namespace KanVest
