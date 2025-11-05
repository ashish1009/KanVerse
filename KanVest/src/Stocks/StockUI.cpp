//
//  StockUI.cpp
//  KanVest
//
//  Created by Ashish . on 05/11/25.
//

#include "StockUI.hpp"

#include "User/UserManager.hpp"

namespace KanVest
{
  static int g_sortColumn = 0;
  static bool g_sortAscending = true;
  static bool g_showAddModal = false;
  static int g_selectedRow = -1;

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
    if (ImGui::BeginChild("StockDetailsCell", ImVec2(-1, ImGui::GetContentRegionAvail().y * 0.5))) // fixed height
    {
      KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundLight, 40);
      KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_26), "Portfolio", KanVasX::UI::AlignX::Center, {0, 5});

      DrawPortfolioTable(UserManager::GetCurrentUser().portfolio.get());
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
  
  void StockUI::DrawPortfolioTable(Portfolio* portfolio)
  {
    auto& holdings = portfolio->GetHoldings();
    
    // Sort table
    std::sort(holdings.begin(), holdings.end(), [](const Holding& a, const Holding& b)
              {
      auto cmp = [&](auto& x, auto& y) { return g_sortAscending ? x < y : x > y; };
      switch (g_sortColumn)
      {
        case 0: return cmp(a.symbol, b.symbol);
        case 1: return cmp(a.averagePrice, b.averagePrice);
        case 2: return cmp(a.quantity, b.quantity);
        case 3: return cmp(a.value, b.value);
        case 4: return cmp(a.profitLossPercent, b.profitLossPercent);
        default: return false;
      }
    });
    
    // Sci-fi look
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0.1f, 0.15f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, ImVec4(0.2f, 0.7f, 1.0f, 0.4f));
    ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImVec4(0.05f, 0.08f, 0.12f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    
    if (ImGui::BeginTable("HoldingsTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable))
    {
      ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_DefaultSort);
      ImGui::TableSetupColumn("Avg Price");
      ImGui::TableSetupColumn("Qty");
      ImGui::TableSetupColumn("Value");
      ImGui::TableSetupColumn("P/L %");
      ImGui::TableSetupColumn("Vision");
      ImGui::TableHeadersRow();
      
      // Sortable header
      if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs())
      {
        if (sortSpecs->SpecsCount > 0)
        {
          g_sortColumn = sortSpecs->Specs[0].ColumnIndex;
          g_sortAscending = sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending;
        }
      }
      
      // Rows
      int idx = 0;
      for (auto& h : holdings)
      {
        ImGui::TableNextRow();
        ImGui::PushID(idx);
        
        if (ImGui::Selectable("##row", g_selectedRow == idx, ImGuiSelectableFlags_SpanAllColumns))
          g_selectedRow = idx;
        
        if (ImGui::BeginPopupContextItem())
        {
          if (ImGui::MenuItem("Edit"))
          {
            // TODO: implement edit modal
          }
          if (ImGui::MenuItem("Delete"))
          {
            holdings.erase(holdings.begin() + idx);
            ImGui::EndPopup();
            ImGui::PopID();
            break;
          }
          ImGui::EndPopup();
        }
        
        ImGui::TableSetColumnIndex(0); ImGui::Text("%s", h.symbol.c_str());
        ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", h.averagePrice);
        ImGui::TableSetColumnIndex(2); ImGui::Text("%d", h.quantity);
        ImGui::TableSetColumnIndex(3); ImGui::Text("%.2f", h.value);
        ImGui::TableSetColumnIndex(4);
        ImGui::TextColored(h.profitLoss >= 0 ? ImVec4(0.2f, 1.0f, 0.3f, 1.0f) : ImVec4(1.0f, 0.2f, 0.3f, 1.0f),
                           "%.2f%%", h.profitLossPercent);
        ImGui::TableSetColumnIndex(5); ImGui::Text("%s", PortfolioUtils::VisionToString(h.vision).c_str());
        
        ImGui::PopID();
        idx++;
      }
      
      ImGui::EndTable();
    }
    
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    
    // ---- Floating Add Holding Button (Fixed) ----
    ImVec2 buttonSize = ImVec2(80, 30);
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();
    float paddingX = 10.0f;
    float paddingY = 10.0f;
    
    ImVec2 buttonPos = ImVec2(windowPos.x + windowSize.x - buttonSize.x - paddingX,
                              windowPos.y + windowSize.y - buttonSize.y - paddingY);
    
    // Draw the floating button in overlay
    ImGui::SetNextWindowPos(buttonPos);
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("AddHoldingOverlay", nullptr,
                 ImGuiWindowFlags_NoDecoration |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoScrollWithMouse |
                 ImGuiWindowFlags_NoSavedSettings |
                 ImGuiWindowFlags_AlwaysAutoResize);
        
    if (ImGui::Button("+ Add ", buttonSize))
      g_showAddModal = true; // 🔥 trigger modal
    
    ImGui::End();
    ImGui::PopStyleVar();
    
    // ---- Actual Modal ----
    if (g_showAddModal)
    {
      ImGui::OpenPopup("AddHoldingModal");
      g_showAddModal = false;
      
      KanVasX::UI::SetNextWindowAtCenter();
    }
    
    if (ImGui::BeginPopupModal("AddHoldingModal", nullptr,
                               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar))
    {
      static char symbolBuf[32] = "";
      static float avgPrice = 0.0;
      static int quantity = 0;
      static int selectedVision = 0;
      static const char* visions[] = {"Long Term", "Mid Term", "Short Term"};

      KanVasX::UI::DrawFilledRect(KanVasX::Color::BackgroundLight, 30);
      KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_22), "Add Holding", KanVasX::UI::AlignX::Center, {0, 0});
      KanVasX::UI::ShiftCursorY(10.0f);
            
      float availX = ImGui::GetContentRegionAvail().x;
      ImGui::PushItemWidth(availX * 0.19); ImGui::InputTextWithHint("##symbol", "Symbol", symbolBuf, IM_ARRAYSIZE(symbolBuf)); ImGui::SameLine();
      ImGui::PushItemWidth(availX * 0.19); ImGui::InputFloat("##avg", &avgPrice, 0, 0, "%.2f"); KanVasX::UI::Tooltip("Average price");  ImGui::SameLine();
      ImGui::PushItemWidth(availX * 0.19); ImGui::InputInt("##qty", &quantity); KanVasX::UI::Tooltip("Quantity");  ImGui::SameLine();
      ImGui::PushItemWidth(availX * 0.19); ImGui::Combo("##vision", &selectedVision, visions, IM_ARRAYSIZE(visions)); ImGui::SameLine();
      
      if (ImGui::Button("Add", ImVec2(60, 0)) || ImGui::IsKeyPressed(ImGuiKey_Enter))
      {
        std::string symbol = symbolBuf;
        if (!symbol.empty() && avgPrice > 0 && quantity > 0)
        {
          Holding h;
          h.symbol = symbol;
          h.averagePrice = avgPrice;
          h.quantity = quantity;
          h.vision = static_cast<Holding::Vision>(selectedVision);
          portfolio->AddHolding(h);
          UserManager::GetCurrentUser().SavePortfolio();
          
          symbolBuf[0] = '\0';
          avgPrice = 0.0f;
          quantity = 0;
          selectedVision = 0;
        }
        else ImGui::OpenPopup("InvalidInput");
      }
      ImGui::SameLine();
      if (ImGui::Button("Cancel", ImVec2(80, 0)) or ImGui::IsKeyPressed(ImGuiKey_Escape))
      {
        ImGui::CloseCurrentPopup();
      }
      
      if (ImGui::BeginPopup("InvalidInput"))
      {
        ImGui::Text("Please fill all fields correctly!");
        if (ImGui::Button("OK") || ImGui::IsKeyPressed(ImGuiKey_Enter))
          ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
      }

//      ImGui::InputText("Symbol", symbolBuf, sizeof(symbolBuf));
//      ImGui::InputDouble("Average Price", &avgPrice, 0, 0, "%.2f");
//      ImGui::InputInt("Quantity", &qty);
//      ImGui::Combo("Vision", &visionIdx, visionNames, IM_ARRAYSIZE(visionNames));
//      
//      ImGui::Separator();
//      if (ImGui::Button("Add", ImVec2(120, 0)))
//      {
//        if (strlen(symbolBuf) > 0 && qty > 0 && avgPrice > 0.0)
//        {
//          Holding h;
//          h.symbol = symbolBuf;
//          h.averagePrice = avgPrice;
//          h.quantity = qty;
//          h.vision = static_cast<Holding::Vision>(visionIdx);
//          portfolio->AddHolding(h);
//          
//          symbolBuf[0] = '\0';
//          avgPrice = 0;
//          qty = 0;
//          
//          ImGui::CloseCurrentPopup();
//        }
//      }
//      
//      ImGui::SameLine();
//      if (ImGui::Button("Cancel", ImVec2(120, 0)))
//        ImGui::CloseCurrentPopup();
      
      ImGui::EndPopup();
    }
  }
  
} // namespace KanVest
