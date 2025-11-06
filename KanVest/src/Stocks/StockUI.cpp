//
//  StockUI.cpp
//  KanVest
//
//  Created by Ashish . on 05/11/25.
//

#include "StockUI.hpp"

#include "Stocks/StockManager.hpp"

#include "User/UserManager.hpp"

#include "Portfolio/PortfolioController.hpp"

namespace KanVest
{
  static int g_sortColumn = 0;
  static int g_selectedRow = -1;
  static int g_editIndex = -1;

  static bool g_sortAscending = true;
  static bool g_showAddModal = false;
  static bool g_showEditModal = false;
  static bool g_focusSymbolNextFrame = false;

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
    if (ImGui::BeginChild("StockPortfolioCell", ImVec2(-1, ImGui::GetContentRegionAvail().y * 0.5))) // fixed height
    {
      KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundLight, 40);
      KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_26), "Portfolio", KanVasX::UI::AlignX::Center, {0, 5});

      DrawPortfolioTable(UserManager::GetCurrentUser().portfolio.get());
    }
    ImGui::EndChild();

    if (ImGui::BeginChild("ChartCell", ImVec2(-1, ImGui::GetContentRegionAvail().y))) // fixed height
    {
//      for (auto& [s, d] : StockManager::GetStokCache())
//      {
//        ImGui::Text("Symbol, %s : %f", d.symbol.c_str(), d.livePrice);
//      }
    }
    ImGui::EndChild();
  }
  
  void StockUI::ShowWatchlist()
  {
    if (ImGui::BeginChild("WatchlistCell", ImVec2(-1, ImGui::GetContentRegionAvail().y))) // fixed height
    {
      KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundLight, 40);
      KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_26), "Watchlist", KanVasX::UI::AlignX::Center, {0, 5});
    }
    ImGui::EndChild();
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
  
  void StockUI::DrawPortfolioTable(Portfolio* portfolio)
  {
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
        default: return false;
      }
    });
    
    // ---- Table ----
    if (ImGui::BeginTable("HoldingsTable", 9,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                          ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable))
    {
      ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_DefaultSort);
      ImGui::TableSetupColumn("ATP");
      ImGui::TableSetupColumn("Qty");
      ImGui::TableSetupColumn("LTP");
      ImGui::TableSetupColumn("Inv");
      ImGui::TableSetupColumn("Value");
      ImGui::TableSetupColumn("P/L");
      ImGui::TableSetupColumn("P/L %");
      ImGui::TableSetupColumn("Vision");
      
      // ---- Header Row ----
      {
        KanVasX::ScopedFont headerfont(UI::Font::Get(UI::FontType::FixedWidthHeader_14));
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
          g_selectedRow = idx;
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
          if (StockManager::GetStock(h.symbol, stockData))
          {
            h.stockValue = stockData.livePrice;
            h.investment = h.averagePrice * h.quantity;
            h.value = h.stockValue * h.quantity;
            h.profitLoss = h.value - h.investment;
            h.profitLossPercent = (h.profitLoss * 100) / h.investment;
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
        PrintCell(8, PortfolioUtils::VisionToString(h.vision));
        
        ImGui::PopID();
      }
      
      // ---- Inline Add Row ----
      static bool showAddRow = false;
      static char symbolBuf[32] = "";
      static float avgPrice = 0.0f;
      static int quantity = 0;
      static int selectedVision = 0;
      static const char* visions[] = {"Long Term", "Mid Term", "Short Term"};
      
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
        
        ImGui::TableSetColumnIndex(8);
        ImGui::PushItemWidth(availX);
        ImGui::Combo("##vision", &selectedVision, visions, IM_ARRAYSIZE(visions));
        
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
