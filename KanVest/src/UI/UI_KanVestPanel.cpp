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

  namespace Utils
  {
    void ConvertUpper(char* string)
    {
      for (char* p = string; *p; ++p)
        *p = static_cast<char>(toupper(*p));
    }
  }
  
  void Panel::SetShadowTextureId(ImTextureID shadowTextureID)
  {
    s_shadowTextureID = shadowTextureID;
  }
  void Panel::SetReloadTextureId(ImTextureID reloadIconID)
  {
    s_reloadIconID = reloadIconID;
  }
  void Panel::SetOpenEyeTextureId(ImTextureID openEyeTextureID)
  {
    s_openEyeTextureID = openEyeTextureID;
  }
  void Panel::SetCloseEyeTextureId(ImTextureID closeEyeTextureID)
  {
    s_closeEyeTextureID = closeEyeTextureID;
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
        ShowStockData();
      }
      ImGui::EndChild();
      ImGui::SameLine();
      
      if (ImGui::BeginChild(" Cell 2 ", ImVec2(totalWidth * 0.4, totalHeight )))
      {
        ShowSearchBar();
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

    // Get portfolio
    Portfolio* portfolio = UserManager::GetCurrentUser().portfolio.get();

    // Get holding
    std::vector<Holding>& holdings = portfolio->GetHoldings();

    static bool g_sortAscending = true;
    static int g_sortColumn = 0;
    
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

    static bool showInvestment = true;
    static const std::string HiddenString = "*****";

    auto FinalString = [](const std::string & data) {
      return showInvestment ? data : HiddenString;
    };

    KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, KanVasX::Color::Alpha(KanVasX::Color::Highlight, 0.2f));
    if (ImGui::BeginChild(" Summary ", ImVec2(0.0f, 180.0f )))
    {
      float totalPortfolioValue = portfolio->GetTotalValue();
      float totalPortfolioInvestment = portfolio->GetTotalInvestment();

      float profitLoss = totalPortfolioValue - totalPortfolioInvestment;
      float profitLossPercent = (profitLoss * 100) / totalPortfolioInvestment;

      float todayChange = portfolio->GetTodayChange();
      float todayChangePercent = (todayChange * 100) / totalPortfolioInvestment;

      // Total Value UI
      std::string totalPortfolioValueString = "₹" + FinalString(Utils::FormatWithCommas((int32_t)totalPortfolioValue));
      KanVasX::UI::Text(Font(Header_56), totalPortfolioValueString, Align::Left, {20.0f, 10.0f});
      
      ImGui::SameLine();
      if (KanVasX::UI::DrawButtonImage("ShowInvestment", showInvestment ? s_openEyeTextureID : s_closeEyeTextureID,
                                       false, {20.0f, 20.0f}, {5.0f, 20.0f}))
      {
        showInvestment ^= 1;
      }
      
      // Profit loss UI
      std::string profitLossTag = profitLoss > 0 ? "Overall Gain" : "Overall Loss";
      std::string pnlSign = profitLoss > 0 ? "+" : "-";
      ImU32 profitLossColor = profitLoss > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;

      KanVasX::UI::Text(Font(Header_26), profitLossTag, Align::Left, {20.0f, -5.0f});

      std::string profitLossString = pnlSign + "₹" + FinalString(Utils::FormatWithCommas((int32_t)profitLoss));
      ImGui::SameLine();
      KanVasX::UI::Text(Font(Header_30), profitLossString, Align::Left, {0.0f, 0.0f}, profitLossColor);
      
      profitLossString = " (" + pnlSign + FinalString(Utils::FormatDoubleToString(profitLossPercent)) + "%)";
      ImGui::SameLine();
      KanVasX::UI::Text(Font(Header_28), profitLossString, Align::Left, {0.0f, 0.0f});

      // Total Investment
      KanVasX::UI::Text(Font(Header_26), "Invested Value", Align::Left, {20.0f, 5.0f});
      ImGui::SameLine();
      std::string dayProfitLossTag = profitLoss > 0 ? "Today's Gain" : "Today's Loss";
      KanVasX::UI::Text(Font(Header_26), dayProfitLossTag, Align::Right, {-20.0f, 0.0f});

      std::string totalPortfolioInvestmentString = "₹" + FinalString(Utils::FormatWithCommas((int32_t)totalPortfolioInvestment));
      KanVasX::UI::Text(Font(Header_30), totalPortfolioInvestmentString, Align::Left, {20.0f, 0.0f});

      ImGui::SameLine();
      std::string dayChangeString = pnlSign + "₹" + FinalString(Utils::FormatWithCommas((int32_t)todayChange));
      ImGui::SameLine();
      KanVasX::UI::Text(Font(Header_30), dayChangeString, Align::Right, {-100.0f, 0.0f}, profitLossColor);

      profitLossString = " (" + pnlSign + FinalString(Utils::FormatDoubleToString(todayChangePercent)) + "%)";
      ImGui::SameLine();
      KanVasX::UI::Text(Font(Header_24), profitLossString, Align::Left, {0.0f, 5.0f});
      
      KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
    }
    ImGui::EndChild();

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
  }
  
  void Panel::ShowStockData()
  {
    IK_PERFORMANCE_FUNC("Panel::ShowStockData");
    
    KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, KanVasX::Color::Alpha(KanVasX::Color::Highlight, 0.2f));
    if (ImGui::BeginChild(" Stock Data ", ImVec2(0.0f, 250.0f )))
    {
      StockData stockData = StockManager::GetSelectedStockData();
      if (!stockData.IsValid())
      {
        KanVasX::UI::Text(Font(Header_24), "No data for stock", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Error);
        ImGui::EndChild();
        return;
      }

      // Name & price
      std::string name = stockData.shortName;
      std::string longNameName = stockData.longName != "" ? stockData.longName : stockData.shortName;

      KanVasX::UI::Text(Font(Header_46), name, Align::Left, {20.0f, 10.0f}, KanVasX::Color::TextBright);
      KanVasX::UI::Text(Font(Header_26), longNameName, Align::Left, {20.0f, 0.0f}, KanVasX::Color::TextBright);
      KanVasX::UI::Text(Font(Header_56), KanVest::UI::Utils::FormatDoubleToString(stockData.livePrice), Align::Left, {20.0f, 0.0f},
                        KanVasX::Color::TextBright);

      // Change
      std::string change = (stockData.change > 0 ? "+" : "-") +
      KanVest::UI::Utils::FormatDoubleToString(stockData.change) +
      (stockData.change > 0 ? " ( +" : " ( -") +
      KanVest::UI::Utils::FormatDoubleToString(stockData.changePercent) + "%)";
      
      ImU32 changeColor = stockData.change > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;
      ImGui::SameLine();
      KanVasX::UI::Text(Font(Header_30), change, Align::Left, {20.0f, 15.0f}, changeColor);

      KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1);
      KanVasX::UI::ShiftCursorY(5.0f);

      // Progress bar
      auto ShowPriceProgress = [](float low, float high, float currentPrice)
      {
        // Avoid division by zero
        if (high <= low) return;
        
        // Calculate fraction for progress bar (0.0 = low, 1.0 = high)
        float fraction = (currentPrice - low) / (high - low);
        fraction = std::clamp(fraction, 0.0f, 1.0f);
        
        {
          // Choose color based on fraction
          ImU32 scoreColor;
          if (fraction < 0.15)      { scoreColor = KanVasX::Color::Red; }
          else if (fraction < 0.30) { scoreColor = KanVasX::Color::Orange; }
          else if (fraction < 0.60) { scoreColor = KanVasX::Color::Yellow; }
          else if (fraction < 0.80) { scoreColor = KanVasX::Color::Cyan; }
          else                    { scoreColor = KanVasX::Color::Green; }
          
          KanVasX::ScopedColor plotColor(ImGuiCol_PlotHistogram, scoreColor);
          KanVasX::ScopedColor frameColor(ImGuiCol_FrameBg, KanVasX::Color::Gray);
          
          ImGui::ProgressBar(fraction, ImVec2(ImGui::GetContentRegionAvail().x * 0.98, 0), " ");
          
          // Draw low/high text on top of the bar
          ImVec2 p = ImGui::GetItemRectMin();   // top-left of bar
          ImVec2 size = ImGui::GetItemRectSize();
          
          ImGui::SetCursorScreenPos(p);
          KanVasX::UI::Text(Font(Header_22), KanVest::UI::Utils::FormatDoubleToString(low), Align::Left, {5.0f, 0.0f}, KanVasX::Color::Black);
          
          ImGui::SetCursorScreenPos(ImVec2(p.x + size.x - ImGui::CalcTextSize(std::to_string((int)high).c_str()).x, p.y));
          KanVasX::UI::Text(Font(Header_22), KanVest::UI::Utils::FormatDoubleToString(high), Align::Right, {-10.0f, 0.0f}, KanVasX::Color::Black);
        }
      };
      
      // 52 - Week change
      KanVasX::UI::Text(Font(FixedWidthHeader_18), "52-Week " , Align::Left, {20.0f, 10.0f}, KanVasX::Color::Text);
      ImGui::SameLine();
      KanVasX::UI::ShiftCursorY(-5.0f);
      ShowPriceProgress(stockData.fiftyTwoLow, stockData.fiftyTwoHigh, stockData.livePrice);

      // Day change
      KanVasX::UI::Text(Font(FixedWidthHeader_18), "Day     " , Align::Left, {20.0f, 10.0f}, KanVasX::Color::Text);
      ImGui::SameLine();
      KanVasX::UI::ShiftCursorY(-5.0f);
      ShowPriceProgress(stockData.dayLow, stockData.dayHigh, stockData.livePrice);

      // Volume
      KanVasX::UI::Text(Font(FixedWidthHeader_18), "Volume  " , Align::Left, {20.0f, 10.0f}, KanVasX::Color::Text);
      ImGui::SameLine();
      KanVasX::UI::Text(Font(FixedWidthHeader_18), Utils::FormatLargeNumber(stockData.volume) , Align::Left, {0.0f, 0.0f}, KanVasX::Color::Text);

      KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
    }
    ImGui::EndChild();
  }
  
  void Panel::ShowSearchBar()
  {
    IK_PERFORMANCE_FUNC("Panel::ShowSearchBar");
    const float contentRegionAvailX = ImGui::GetContentRegionAvail().x;
    KanVasX::UI::DrawFilledRect(KanVasX::Color::FrameBg, 40.0f);
    if (KanVasX::Widget::Search(s_searchedString, 128, KanVasX::Settings::FrameHeight, contentRegionAvailX - 50.0f, "Enter Symbol ...", Font(Large), true))
    {
      Utils::ConvertUpper(s_searchedString);
    }
    
    ImGui::SameLine();
    float iconSize = ImGui::GetItemRectSize().y - 12;
    if (KanVasX::UI::DrawButtonImage("Refresh", s_reloadIconID, false, {iconSize, iconSize}, {0.0, 6.0}) || ImGui::IsKeyDown(ImGuiKey_Enter))
    {
      AddStockInManager(s_searchedString);
    }
  }

  void Panel::AddStockInManager(const std::string& symbol)
  {
    StockManager::AddStock(symbol);
    StockManager::SetSelectedStockSymbol(symbol);
  }
} // namespace KanVest::UI
