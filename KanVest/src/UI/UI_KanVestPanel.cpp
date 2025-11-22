//
//  UI_KanVestPanel.cpp
//  KanVest
//
//  Created by Ashish . on 21/11/25.
//

#include "UI_KanVestPanel.hpp"

#include "User/UserManager.hpp"

#include "Portfolio/PortfolioController.hpp"

#include "UI/UI_Utils.hpp"

#include "Stock/StockManager.hpp"

namespace KanVest::UI
{
#define Font(font) KanVest::UI::Font::Get(KanVest::UI::FontType::font)

  using Align = KanVasX::UI::AlignX;
  
  void Panel::SetShadowTextureId(ImTextureID shadowTextureID)
  {
    s_shadowTextureID = shadowTextureID;
  }
  void Panel::SetReloadTextureId(ImTextureID reloadIconID)
  {
    s_reloadIconID = reloadIconID;
  }
  
  void Panel::Show()
  {
    IK_PERFORMANCE_FUNC("Panel::Show");

    static float Spacing = 2.0f;
    KanVasX::ScopedStyle WindowPadding(ImGuiStyleVar_WindowPadding, ImVec2(Spacing, Spacing));
    KanVasX::ScopedStyle ItemSpacing(ImGuiStyleVar_ItemSpacing, ImVec2(Spacing, Spacing));
    
    KanVasX::Panel::Begin("Stock Analyzer");

    /*
      - Stock basic data
        - Stock name
        - Stock price
        - Stock change
        - 52 week high low (Graph)
        - Day high low
      - Portfolio
      - Watchlist
      - Recommendation
      - Chart (Wide)
      - Technical data (Wide)
        - Data
        - Corresponding chart
     */
    
    {
      KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, KanVasX::Color::Background);

      float totalWidth  = ImGui::GetContentRegionAvail().x;
      float totalHeight = ImGui::GetContentRegionAvail().y;
      
      if (ImGui::BeginChild(" Stock - Data ", ImVec2(totalWidth * 0.3, totalHeight )))
      {
      }
      ImGui::EndChild();
      ImGui::SameLine();
      
      if (ImGui::BeginChild(" Cell 2 ", ImVec2(totalWidth * 0.4, totalHeight )))
      {
      }
      ImGui::EndChild();
      ImGui::SameLine();
      
      if (ImGui::BeginChild(" Portfolio ", ImVec2(totalWidth * 0.298, totalHeight )))
      {
        ShowPortfolio();
      }

      ImGui::EndChild();
    }

    KanVasX::Panel::End(0);
  }
  
  void Panel::ShowPortfolio()
  {
    IK_PERFORMANCE_FUNC("Panel::ShowPortfolio");
    float totalHeight = ImGui::GetContentRegionAvail().y;

    KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, KanVasX::Color::Alpha(KanVasX::Color::Highlight, 0.2f));
    if (ImGui::BeginChild(" Summary ", ImVec2(0.0f, totalHeight * 0.2f )))
    {
      static bool g_sortAscending = true;
      static int g_sortColumn = 0;

      // Get portfolio
      Portfolio* portfolio = UserManager::GetCurrentUser().portfolio.get();
      
      // Get holding
      std::vector<Holding>& holdings = portfolio->GetHoldings();

      // Update computed data for holdings
      for (int idx = 0; idx < holdings.size(); idx++)
      {
        auto& h = holdings[idx];
        
        StockData stockData("");
        StockManager::GetStockData(h.symbolName, stockData);
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
      
      // ---- Sorting ----
      std::sort(holdings.begin(), holdings.end(), [](const Holding& a, const Holding& b)
                {
        auto cmp = [&](auto& x, auto& y) { return g_sortAscending ? x < y : x > y; };
        switch (g_sortColumn)
        {
          case 0: return cmp(a.symbolName, b.symbolName);
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
      
      std::string totalPortfolioValueString = "₹ " + std::to_string((int32_t)portfolio->GetTotalInvestment());
      KanVasX::UI::Text(Font(Header_28), totalPortfolioValueString, Align::Left, {20.0f, 10.0f});
      
      KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
    }
    ImGui::EndChild();
  }
} // namespace KanVest::UI
