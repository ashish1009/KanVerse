//
//  UI_KanVestPanel.cpp
//  KanVest
//
//  Created by Ashish . on 21/11/25.
//

#include "UI_KanVestPanel.hpp"

#include "User/UserManager.hpp"

#include "UI/UI_Utils.hpp"

#include "Stock/StockManager.hpp"

namespace KanVest::UI
{
#define Font(font) KanVest::UI::Font::Get(KanVest::UI::FontType::font)

  static bool g_showInvestment = true;
  static bool g_sortAscending = true;
  static int g_sortColumn = 0;
  static const std::string g_hiddenString = "*****";
  
  auto FinalString = [](const std::string & data) {
    return g_showInvestment ? data : g_hiddenString;
  };

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
      KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, KanVasX::Color::BackgroundLight);

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
        ShowSearchBar(s_searchedString, 10.0f);
      }
      ImGui::EndChild();
      ImGui::SameLine();
      
      if (ImGui::BeginChild(" Portfolio ", ImVec2(totalWidth * 0.298, totalHeight)))
      {
        ShowPortfolio();
      }

      ImGui::EndChild();
    }

    KanVasX::Panel::End(0);
  }
  
  void Panel::ShowPortfolioSummary(Portfolio* portfolio)
  {
    KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, KanVasX::Color::Alpha(KanVasX::Color::Highlight, 1.0f));
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
      if (KanVasX::UI::DrawButtonImage("ShowInvestment", g_showInvestment ? s_openEyeTextureID : s_closeEyeTextureID,
                                       false, {20.0f, 20.0f}, {5.0f, 20.0f}))
      {
        g_showInvestment ^= 1;
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
  }
  
  void Panel::ShowPortfolio()
  {
    IK_PERFORMANCE_FUNC("Panel::ShowPortfolio");

    // Get portfolio
    Portfolio* portfolio = UserManager::GetCurrentUser().portfolio.get();

    // Get holding
    std::vector<Holding>& holdings = portfolio->GetHoldings();
    
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

    // Show summary data
    ShowPortfolioSummary(portfolio);

    KanVasX::UI::ShiftCursor({5.0f, 5.0f});
    ShowSearchBar(s_searchedString, 5.0f);
    KanVasX::UI::ShiftCursorY(5.0f);

    float totalWidth  = ImGui::GetContentRegionAvail().x;
    float totalHeight = ImGui::GetContentRegionAvail().y;
    
    KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, KanVasX::Color::Alpha(KanVasX::Color::Highlight, 0.2f));
    if (ImGui::BeginChild("Holding Data", ImVec2(totalWidth, totalHeight), false, ImGuiWindowFlags_NoScrollbar))
    {
      for (int idx = 0; idx < holdings.size(); idx++)
      {
        auto& h = holdings[idx];
        
        // Update Stock Data
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

        std::string holdingChildID = "HoldingChild_" + std::to_string(idx);
        ImVec2 childSize(ImGui::GetContentRegionAvail().x, 125.0f);

        KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, KanVasX::Color::Alpha(KanVasX::Color::BackgroundDark, 0.2f));
        if (ImGui::BeginChild(holdingChildID.c_str(), childSize, true))
        {
          std::string pnlSign = h.profitLoss > 0 ? "+" : "-";
          ImU32 profitLossColor = h.profitLoss > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;

          // Symbol PL
          {
            KanVasX::UI::Text(Font(Header_34), h.symbolName, Align::Left, {5.0f, 5.0f});
            ImGui::SameLine();

            // Right Data
            {
              std::string profitLossString = pnlSign + "₹" + FinalString(Utils::FormatWithCommas((int32_t)h.profitLoss));
              std::string profitLossPercentString = " (" + pnlSign + FinalString(Utils::FormatDoubleToString(h.profitLossPercent)) + "%)";
              
              KanVasX::ScopedFont header(Font(Header_26));
              {
                KanVasX::ScopedColor textColor(ImGuiCol_Text, profitLossColor);
                
                float textSize = ImGui::CalcTextSize(profitLossString.data()).x + ImGui::CalcTextSize(profitLossPercentString.data()).x;
                float xOffset = (ImGui::GetColumnWidth() - textSize - 5.0f) ;
                
                ImGui::SetCursorPos({ImGui::GetCursorPosX() + xOffset, ImGui::GetCursorPosY() + 5.0f});
                ImGui::Text("%s", profitLossString.data());
              }
              ImGui::SameLine();
              KanVasX::UI::ShiftCursorY(5.0f);
              ImGui::Text("%s", profitLossPercentString.data());
            }
          }
          
          // ATP LTP
          {
            KanVasX::UI::Text(Font(Header_18), "ATP ", Align::Left, {5.0f, 5.0f});
            ImGui::SameLine();
            std::string atpString = "₹" + Utils::FormatDoubleToString(h.averagePrice);
            KanVasX::UI::Text(Font(Header_18), atpString, Align::Left, {0.0f, 0.0f});

            // Right Data
            {
              ImGui::SameLine();

              std::string ltpString = "LTP ";
              std::string ltpValueString = "₹" + Utils::FormatDoubleToString(h.stockValue);
              std::string ltpPerString = " (" + pnlSign + FinalString(Utils::FormatDoubleToString(h.dayChangePercent)) + "%)";
              
              KanVasX::ScopedFont header(Font(Header_18));
              float textSize = ImGui::CalcTextSize(ltpString.data()).x + ImGui::CalcTextSize(ltpValueString.data()).x + ImGui::CalcTextSize(ltpPerString.data()).x;
              float xOffset = (ImGui::GetColumnWidth() - textSize - 8.0f) ;
              
              ImGui::SetCursorPos({ImGui::GetCursorPosX() + xOffset, ImGui::GetCursorPosY()});
              ImGui::Text("%s", ltpString.data());

              {
                KanVasX::ScopedColor textColor(ImGuiCol_Text, profitLossColor);
                ImGui::SameLine();
                ImGui::Text("%s", ltpValueString.data());
              }

              ImGui::SameLine();
              ImGui::Text("%s", ltpPerString.data());
            }
          }

          // Shares
          {
            KanVasX::UI::Text(Font(Header_18), "Shares ", Align::Left, {5.0f, 0.0f});
            ImGui::SameLine();
            KanVasX::UI::Text(Font(Header_18), std::to_string(h.quantity), Align::Left, {0.0f, 0.0f});
            
            // Right Data
            {
              ImGui::SameLine();

              std::string gainTagString = h.profitLoss > 0 ? "Today's Gain " : "Today's Loss";
              std::string gainValueString = "₹" + Utils::FormatDoubleToString(h.dayChange);
              
              KanVasX::ScopedFont header(Font(Header_18));
              float textSize = ImGui::CalcTextSize(gainTagString.data()).x + ImGui::CalcTextSize(gainValueString.data()).x;
              float xOffset = (ImGui::GetColumnWidth() - textSize - 5.0f) ;
              
              ImGui::SetCursorPos({ImGui::GetCursorPosX() + xOffset, ImGui::GetCursorPosY()});
              ImGui::Text("%s", gainTagString.data());
              
              {
                KanVasX::ScopedColor textColor(ImGuiCol_Text, profitLossColor);
                ImGui::SameLine();
                ImGui::Text("%s", gainValueString.data());
              }
            }
          }
          
          KanVasX::UI::ShiftCursorY(5.0f);
          KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1);

          // Invested
          {
            KanVasX::UI::Text(Font(Header_20), "Invested ", Align::Left, {5.0f, 4.0f});
            ImGui::SameLine();
            KanVasX::UI::Text(Font(Header_20), Utils::FormatWithCommas(int32_t(h.investment)), Align::Left, {0.0f, 0.0f});
            
            // Right Data
            {
              ImGui::SameLine();
              
              std::string valueTagString = "Current ";
              std::string valueValueString = "₹" + Utils::FormatWithCommas(int32_t(h.investment));
              
              KanVasX::ScopedFont header(Font(Header_20));
              float textSize = ImGui::CalcTextSize(valueTagString.data()).x + ImGui::CalcTextSize(valueValueString.data()).x;
              float xOffset = (ImGui::GetColumnWidth() - textSize - 5.0f) ;
              
              ImGui::SetCursorPos({ImGui::GetCursorPosX() + xOffset, ImGui::GetCursorPosY()});
              ImGui::Text("%s", valueTagString.data());
              
              ImGui::SameLine();
              ImGui::Text("%s", valueValueString.data());
            }
          }
        }
        ImGui::EndChild();

        KanVasX::UI::ShiftCursorY(5.0f);
      }
    }
    ImGui::EndChild();
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
  
  void Panel::ShowSearchBar(char* searchString, float height)
  {
    IK_PERFORMANCE_FUNC("Panel::ShowSearchBar");
    const float contentRegionAvailX = ImGui::GetContentRegionAvail().x;
    if (KanVasX::Widget::Search(searchString, 128, height, contentRegionAvailX - 40.0f, "Enter Symbol ...", Font(Large), true))
    {
      Utils::ConvertUpper(searchString);
    }
    
    ImGui::SameLine();
    float iconSize = ImGui::GetItemRectSize().y - 12;
    if (KanVasX::UI::DrawButtonImage("Refresh", s_reloadIconID, false, {iconSize, iconSize}, {0.0, 6.0}) || ImGui::IsKeyDown(ImGuiKey_Enter))
    {
      AddStockInManager(searchString);
    }
  }

  void Panel::AddStockInManager(const std::string& symbol)
  {
    StockManager::AddStock(symbol);
    StockManager::SetSelectedStockSymbol(symbol);
  }
} // namespace KanVest::UI
