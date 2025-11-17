//
//  KanVestPanel.cpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
//

#include "KanVestPanel.hpp"

#include "User/UserManager.hpp"

#include "Stock/StockManager.hpp"

#include "Portfolio/PortfolioController.hpp"

namespace KanVest::UI
{
  void Panel::SetShadowTextureId(ImTextureID shadowTextureID)
  {
    s_shadowTextureID = shadowTextureID;
  }

  void Panel::Show()
  {
    IK_PERFORMANCE_FUNC("Panel::Show");
    
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
    IK_PERFORMANCE_FUNC("Panel::ShowStockAnalyzerTable");
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
    IK_PERFORMANCE_FUNC("Panel::ShowPortfolioChartTable");
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
          ShowPortfolio();

          KanVasX::UI::ShiftCursor({ImGui::GetContentRegionAvail().x * 0.92f, -30});
          KanVasX::ScopedColor textColor(ImGuiCol_Text, KanVasX::Color::Gray);
          ImGui::Text("FPS : %.1f", ImGui::GetIO().Framerate);

          KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
        }
        ImGui::EndChild();
      }
      ImGui::EndTable();
    }
  }
  
  void Panel::ShowPortfolio()
  {
    IK_PERFORMANCE_FUNC("Panel::ShowPortfolio");
    
    // Static flags
    static bool g_showEditModal = false;
    static bool g_sortAscending = true;

    static int g_sortColumn = 0;
    static int g_selectedRow = -1;
    static int g_editIndex = -1;

    Portfolio* portfolio = UserManager::GetCurrentUser().portfolio.get();
    auto& holdings = portfolio->GetHoldings();
    
    // ---- Sorting ----
    std::sort(holdings.begin(), holdings.end(), [](const Holding& a, const Holding& b)
              {
      auto cmp = [&](auto& x, auto& y) { return g_sortAscending ? x < y : x > y; };
      switch (g_sortColumn)
      {
        case 0: return cmp(a.symbol, b.symbol);
        case 1: return cmp(a.averagePrice, b.averagePrice);
        case 2: return cmp(a.quantity, b.quantity);
        case 3: return cmp(a.stockValue, b.stockValue);
        case 4: return cmp(a.investment, b.investment);
        case 5: return cmp(a.value, b.value);
        case 6: return cmp(a.profitLoss, b.profitLoss);
        case 7: return cmp(a.profitLossPercent, b.profitLossPercent);
        case 8: return cmp(a.dayChange, b.dayChange);
        case 9: return cmp(a.dayChangePercent, b.dayChangePercent);
        default: return false;
      }
    });
    
    // ---- Table ----
    if (ImGui::BeginTable("HoldingsTable", 10, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable))
    {
      ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_DefaultSort);
      ImGui::TableSetupColumn("ATP");
      ImGui::TableSetupColumn("Qty");
      ImGui::TableSetupColumn("LTP");
      ImGui::TableSetupColumn("Inv");
      ImGui::TableSetupColumn("Value");
      ImGui::TableSetupColumn("P/L");
      ImGui::TableSetupColumn("P/L %");
      ImGui::TableSetupColumn("Day Change");
      ImGui::TableSetupColumn("Day Change %");
      
      // ---- Header Row ----
      {
        KanVasX::ScopedFont headerfont(UI::Font::Get(UI::FontType::Large));
        ImGui::TableSetupScrollFreeze(ImGui::TableGetColumnCount(), 1);
        ImGui::TableNextRow(ImGuiTableRowFlags_Headers, 22.0f);
        
        for (int col = 0; col < ImGui::TableGetColumnCount(); col++)
        {
          ImGui::TableSetColumnIndex(col);
          ImGui::TableHeader(ImGui::TableGetColumnName(col));
        }
      }
      
      // ---- Sort Header ----
      if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs())
      {
        if (sortSpecs->SpecsCount > 0)
        {
          g_sortColumn = sortSpecs->Specs[0].ColumnIndex;
          g_sortAscending = sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending;
        }
      }
      
      // ---- Holdings Rows ----
      for (int idx = 0; idx < holdings.size(); idx++)
      {
        auto& h = holdings[idx];
        ImGui::PushID(idx);
        ImGui::TableNextRow();
        
        // Selectable row
        ImGui::TableSetColumnIndex(0);
        auto cursor = ImGui::GetCursorPos();
        if (ImGui::Selectable(("##row_" + std::to_string(idx)).c_str(), g_selectedRow == idx,
                              ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
        {
          StockManager::SetSelectedStockSymbol(h.symbol);
          g_selectedRow = idx;
        }
        
        // --- Double-click to Edit ---
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
          g_editIndex = idx;
          g_showEditModal = true;
        }
        
        // Context Menu
        if (ImGui::BeginPopupContextItem())
        {
          if (ImGui::MenuItem("Edit"))
          {
            g_editIndex = idx;
            g_showEditModal = true;
          }
          if (ImGui::MenuItem("Delete"))
          {
            holdings.erase(holdings.begin() + idx);
            UserManager::GetCurrentUser().SavePortfolio();
            ImGui::EndPopup();
            ImGui::PopID();
            break;
          }
          ImGui::EndPopup();
        }
        
        // Update data
        {
          StockData stockData("");
          StockManager::GetStockData(h.symbol, stockData);
          if (stockData.IsValid())
          {
            h.stockValue = stockData.livePrice;
            h.investment = h.averagePrice * h.quantity;
            h.value = h.stockValue * h.quantity;
            h.profitLoss = h.value - h.investment;
            h.profitLossPercent = (h.profitLoss * 100) / h.investment;
            h.dayChange = stockData.change;
            h.dayChangePercent = stockData.changePercent;
          }
        }
        
        // Draw columns
        ImGui::SetCursorPos(cursor);
        auto PrintCell = [](int32_t colIdx, const auto& data, ImU32 textColor = KanVasX::Color::TextBright)
        {
          ImGui::TableSetColumnIndex(colIdx);
          KanVasX::ScopedColor scopedTextColor(ImGuiCol_Text, textColor);
          
          if constexpr (std::is_same_v<std::decay_t<decltype(data)>, double>)
            ImGui::Text("%.2f", data);
          else if constexpr (std::is_same_v<std::decay_t<decltype(data)>, float>)
            ImGui::Text("%.2f", data);
          else if constexpr (std::is_same_v<std::decay_t<decltype(data)>, int>)
            ImGui::Text("%d", data);
          else if constexpr (std::is_same_v<std::decay_t<decltype(data)>, std::string>)
            ImGui::Text("%s", data.c_str());
          else
            ImGui::Text("%s", std::to_string(data).c_str());
        };
        
        PrintCell(0, h.symbol);
        PrintCell(1, h.averagePrice);
        PrintCell(2, h.quantity);
        PrintCell(3, h.stockValue);
        PrintCell(4, h.investment);
        PrintCell(5, h.value);
        PrintCell(6, h.profitLoss, h.profitLoss > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red);
        PrintCell(7, h.profitLossPercent, h.profitLoss > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red);
        PrintCell(8, h.dayChange, h.dayChange > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red);
        PrintCell(9, h.dayChangePercent, h.dayChangePercent > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red);
        
        ImGui::PopID();
        
        if (ImGui::IsKeyPressed(ImGuiKey_Backspace))
        {
          if (g_selectedRow >= 0)
          {
            holdings.erase(holdings.begin() + g_selectedRow);
            UserManager::GetCurrentUser().SavePortfolio();
            g_selectedRow = -1;
          }
        }
      }
      
      // ---- Inline Add Row ----
      static bool showAddRow = false;
      static char symbolBuf[32] = "";
      static float avgPrice = 0.0f;
      static int quantity = 0;
      static int selectedVision = 0;
      
      // Toggle add-row when ⌘ + N is pressed
      if (ImGui::IsKeyDown(ImGuiKey_LeftSuper) && ImGui::IsKeyPressed(ImGuiKey_N))
      {
        showAddRow = !showAddRow;
        if (showAddRow)
        {
          symbolBuf[0] = '\0';
          avgPrice = 0.0f;
          quantity = 0;
          selectedVision = 0;
        }
      }
      
      if (showAddRow)
      {
        float availX = ImGui::GetContentRegionAvail().x;
        
        ImGui::TableNextRow();
        ImGui::PushID("AddRow");
        
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::TableSetColumnIndex(0);
        if (std::string(symbolBuf).empty())
        {
          ImGui::SetKeyboardFocusHere();
        }
        ImGui::PushItemWidth(availX);
        ImGui::InputTextWithHint("##symbol", "Symbol", symbolBuf, IM_ARRAYSIZE(symbolBuf));
        
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(availX);
        ImGui::InputFloat("##avg", &avgPrice, 0, 0, "%.2f");
        
        ImGui::TableSetColumnIndex(2);
        ImGui::PushItemWidth(availX);
        ImGui::InputInt("##qty", &quantity);
        
        if (ImGui::IsKeyPressed(ImGuiKey_Enter))
        {
          std::string symbol = symbolBuf;
          if (!symbol.empty() && avgPrice > 0 && quantity > 0)
          {
            Holding h;
            h.symbol = symbol;
            h.averagePrice = avgPrice;
            h.quantity = quantity;
            h.vision = static_cast<Holding::Vision>(selectedVision);
            
            PortfolioController portfolioController(*portfolio);
            portfolioController.AddHolding(h);
            UserManager::GetCurrentUser().SavePortfolio();
            
            // Reset
            if (!ImGui::IsKeyDown(ImGuiKey_LeftSuper))
            {
              showAddRow = false;
            }
            else
            {
              symbolBuf[0] = '\0';
              avgPrice = 0.0f;
              quantity = 0;
              selectedVision = 0;
            }
          }
        }
        
        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
          showAddRow = false;
        }
        
        ImGui::PopID();
      }
      
      ImGui::EndTable();
    }
    
    // ---- Edit Modal ----
    if (g_showEditModal && g_editIndex >= 0 && g_editIndex < holdings.size())
    {
      ImGui::OpenPopup("EditHoldingModal");
      g_showEditModal = false;
      KanVasX::UI::SetNextWindowAtCenter();
      KanVasX::UI::SetNextWindowSize({800, 80});
    }
    
    if (ImGui::BeginPopupModal("EditHoldingModal", nullptr,
                               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                               ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar))
    {
      Holding& h = holdings[g_editIndex];
      static char symbolBuf[32];
      static float avgPrice;
      static int quantity;
      static int selectedVision;
      static bool initialized = false;
      
      if (!initialized)
      {
        strcpy(symbolBuf, h.symbol.c_str());
        avgPrice = h.averagePrice;
        quantity = h.quantity;
        selectedVision = static_cast<int>(h.vision);
        initialized = true;
      }
      
      KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundLight, 30);
      KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_22), "Edit Holding", KanVasX::UI::AlignX::Center);
      KanVasX::UI::ShiftCursorY(10.0f);
      
      const char* visions[] = {"Long Term", "Mid Term", "Short Term"};
      float availX = ImGui::GetContentRegionAvail().x;
      
      ImGui::PushItemWidth(availX * 0.19); ImGui::InputTextWithHint("##symbol", "Symbol", symbolBuf, IM_ARRAYSIZE(symbolBuf)); ImGui::SameLine();
      ImGui::PushItemWidth(availX * 0.19); ImGui::InputFloat("##avg", &avgPrice, 0, 0, "%.2f"); ImGui::SameLine();
      ImGui::PushItemWidth(availX * 0.19); ImGui::InputInt("##qty", &quantity); ImGui::SameLine();
      ImGui::PushItemWidth(availX * 0.19); ImGui::Combo("##vision", &selectedVision, visions, IM_ARRAYSIZE(visions)); ImGui::SameLine();
      
      if (ImGui::Button("Save", ImVec2(60, 0)) || ImGui::IsKeyPressed(ImGuiKey_Enter))
      {
        h.symbol = symbolBuf;
        h.averagePrice = avgPrice;
        h.quantity = quantity;
        h.vision = static_cast<Holding::Vision>(selectedVision);
        
        PortfolioController portfolioController(*portfolio);
        portfolioController.EditHolding(h);
        
        UserManager::GetCurrentUser().SavePortfolio();
        initialized = false;
        ImGui::CloseCurrentPopup();
      }
      
      ImGui::SameLine();
      if (ImGui::Button("Cancel", ImVec2(80, 0)) || ImGui::IsKeyPressed(ImGuiKey_Escape))
      {
        initialized = false;
        ImGui::CloseCurrentPopup();
      }
      
      ImGui::EndPopup();
    }
  }
} // namespace KanVest
