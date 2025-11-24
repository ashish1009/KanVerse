//
//  UI_KanVestPanel.cpp
//  KanVest
//
//  Created by Ashish . on 21/11/25.
//

#include "UI_KanVestPanel.hpp"

#include <Vendors/Choc/text/choc_StringUtilities.h>

#include "User/UserManager.hpp"

#include "UI/UI_Utils.hpp"
#include "UI/UI_MovingAverage.hpp"
#include "UI/UI_Momentum.hpp"
#include "UI/UI_ADX.hpp"
#include "UI/UI_MACD.hpp"
#include "UI/UI_MFI.hpp"
#include "UI/UI_Stochastic.hpp"
#include "UI/UI_BollingerBand.hpp"

#include "Stock/StockManager.hpp"

#include "Analyzer/StockAnalyzer.hpp"

namespace KanVest::UI
{
#define Font(font) KanVest::UI::Font::Get(KanVest::UI::FontType::font)

  static bool g_showInvestment = true;
  static bool g_sortAscending = true;
  static bool g_showEditModal = false;
  static bool g_showAddRow = false;
  static bool g_showSettingModel = false;

  static int g_sortColumn = 0;
  static int g_selectedIdx = -1;

  static const std::string g_hiddenString = "*****";

  static char symbolBuf[32];
  static float avgPrice;
  static float quantity;

  auto FinalString = [](const std::string & data) {
    return g_showInvestment ? data : g_hiddenString;
  };

  using Align = KanVasX::UI::AlignX;

  namespace Utils
  {
    std::string ToLower(const std::string& string)
    {
      std::string result;
      result.reserve(string.size());
      
      for (const char c : string)
      {
        result += static_cast<char>(std::tolower(c));
      }
      
      return result;
    }
    void ConvertUpper(char* string)
    {
      for (char* p = string; *p; ++p)
        *p = static_cast<char>(toupper(*p));
    }

    bool IsMatchingSearch(const std::string& item, const std::string_view& searchQuery,
                          bool caseSensitive = false, bool stripWhiteSpaces = false, bool stripUnderscores = false)
    {
      if (searchQuery.empty())
      {
        return true;
      }
      
      if (item.empty())
      {
        return false;
      }
      
      std::string itemSanitized = stripUnderscores ? choc::text::replace(item, "_", " ") : item;
      
      if (stripWhiteSpaces)
      {
        itemSanitized = choc::text::replace(itemSanitized, " ", "");
      }
      
      std::string searchString = stripWhiteSpaces ? choc::text::replace(searchQuery, " ", "") : std::string(searchQuery);
      
      if (!caseSensitive)
      {
        itemSanitized = ToLower(itemSanitized);
        searchString = ToLower(searchString);
      }
      
      bool result = false;
      if (choc::text::contains(searchString, " "))
      {
        std::vector<std::string> searchTerms = choc::text::splitAtWhitespace(searchString);
        for (const auto& searchTerm : searchTerms)
        {
          if (!searchTerm.empty() and choc::text::contains(itemSanitized, searchTerm))
          {
            result = true;
          }
          else
          {
            result = false;
            break;
          }
        }
      }
      else
      {
        result = choc::text::contains(itemSanitized, searchString);
      }
      
      return result;
    }
    
    static std::vector<StockPoint> FilterTradingDays(const std::vector<StockPoint>& history)
    {
      std::vector<StockPoint> filtered;
      filtered.reserve(history.size());
      
      for (const auto& h : history)
      {
        time_t t = static_cast<time_t>(h.timestamp);
        struct tm* tm_info = localtime(&t);
        int wday = tm_info->tm_wday; // 0 = Sunday, 6 = Saturday
        if (wday != 0 && wday != 6)
        {
          filtered.push_back(h);
        }
      }
      
      return filtered;
    }
  }
  
  void Panel::Initialize()
  {
    AddStockInManager(s_searchedStockString);
  }
  
  void Panel::SetShadowTextureId(ImTextureID shadowTextureID)
  {
    s_shadowTextureID = shadowTextureID;
  }
  void Panel::SetReloadTextureId(ImTextureID reloadIconID)
  {
    s_reloadIconID = reloadIconID;
  }
  void Panel::SetSettingTextureId(ImTextureID settingIconID)
  {
    s_settingIconID = settingIconID;
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
        
        KanVasX::UI::ShiftCursorY(ImGui::GetContentRegionAvail().y - 35.0f);
        ShowStockSearchBar(5.0f);
      }
      ImGui::EndChild();
      ImGui::SameLine();
      
      static float chioldWidth = totalWidth * 0.45;
      if (ImGui::BeginChild(" Chart-Technicals ", ImVec2(chioldWidth, totalHeight )))
      {
        if (ImGui::BeginChild(" Technical ", ImVec2(chioldWidth, totalHeight - 330.0f )))
        {
          ShowStockTechnicalData();
        }
        ImGui::EndChild();
        if (ImGui::BeginChild(" Chart ", ImVec2(chioldWidth, 0)))
        {
          ShowChart();
          KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
        }
        ImGui::EndChild();
      }
      ImGui::EndChild();
      ImGui::SameLine();
      
      if (ImGui::BeginChild(" Portfolio ", ImVec2(totalWidth * 0.248, totalHeight)))
      {
        ShowPortfolio();

        KanVasX::UI::ShiftCursor({ImGui::GetContentRegionAvail().x - 80.0f, ImGui::GetContentRegionAvail().y - 22.0f});
        KanVasX::ScopedColor textColor(ImGuiCol_Text, KanVasX::Color::Gray);
        ImGui::Text("FPS : %.1f", ImGui::GetIO().Framerate);
      }

      ImGui::EndChild();
    }

    KanVasX::Panel::End(0);
  }
  
  void Panel::ShowPortfolioSummary(Portfolio* portfolio)
  {
    IK_PERFORMANCE_FUNC("Panel::ShowPortfolioSummary");

    KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, KanVasX::Color::Alpha(KanVasX::Color::Value(KanVasX::Color::Background, 2.0f), 0.2f));
    if (ImGui::BeginChild(" Summary ", ImVec2(0.0f, 160.0f )))
    {
      float totalPortfolioValue = portfolio->GetTotalValue();
      float totalPortfolioInvestment = portfolio->GetTotalInvestment();
      
      float profitLoss = totalPortfolioValue - totalPortfolioInvestment;
      float profitLossPercent = (profitLoss * 100) / totalPortfolioInvestment;
      
      float todayChange = portfolio->GetTodayChange();
      float todayChangePercent = (todayChange * 100) / totalPortfolioInvestment;
      
      // Total Value UI
      std::string totalPortfolioValueString = "₹" + FinalString(Utils::FormatWithCommas((int32_t)totalPortfolioValue));
      KanVasX::UI::Text(Font(Header_48), totalPortfolioValueString, Align::Left, {20.0f, 10.0f});
      
      ImGui::SameLine();
      if (KanVasX::UI::DrawButtonImage("ShowInvestment", g_showInvestment ? s_openEyeTextureID : s_closeEyeTextureID,
                                       false, {20.0f, 20.0f}, {5.0f, 20.0f}))
      {
        g_showInvestment ^= 1;
      }
      
      // Profit loss UI
      std::string profitLossTag = profitLoss > 0 ? "Overall Gain" : "Overall Loss";
      std::string pnlSign = profitLoss > 0 ? "+" : "";
      ImU32 profitLossColor = profitLoss > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;
      
      KanVasX::UI::Text(Font(Header_22), profitLossTag, Align::Left, {20.0f, -5.0f});
      
      std::string profitLossString = "₹" + pnlSign + FinalString(Utils::FormatWithCommas((int32_t)profitLoss));
      ImGui::SameLine();
      KanVasX::UI::Text(Font(Header_28), profitLossString, Align::Left, {0.0f, 0.0f}, profitLossColor);
      
      profitLossString = " (" + pnlSign + FinalString(Utils::FormatDoubleToString(profitLossPercent)) + "%)";
      ImGui::SameLine();
      KanVasX::UI::Text(Font(Header_26), profitLossString, Align::Left, {0.0f, 0.0f});
      
      // Total Investment
      KanVasX::UI::Text(Font(Header_24), "Invested Value", Align::Left, {20.0f, 5.0f});
      ImGui::SameLine();
      std::string dayProfitLossTag = todayChange > 0 ? "Range Gain" : "Range Loss";
      KanVasX::UI::Text(Font(Header_24), dayProfitLossTag, Align::Right, {-20.0f, 0.0f});
      
      std::string totalPortfolioInvestmentString = "₹" + FinalString(Utils::FormatWithCommas((int32_t)totalPortfolioInvestment));
      KanVasX::UI::Text(Font(Header_28), totalPortfolioInvestmentString, Align::Left, {20.0f, 0.0f});
      
      ImGui::SameLine();
      std::string dayPnlSign = todayChange > 0 ? "+" : "";
      ImU32 dayProfitLossColor = todayChange > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;

      std::string dayChangeString = dayPnlSign + "₹" + FinalString(Utils::FormatWithCommas((int32_t)todayChange));
      ImGui::SameLine();
      KanVasX::UI::Text(Font(Header_28), dayChangeString, Align::Right, {-100.0f, 0.0f}, dayProfitLossColor);
      
      profitLossString = " (" + dayPnlSign + FinalString(Utils::FormatDoubleToString(todayChangePercent)) + "%)";
      ImGui::SameLine();
      KanVasX::UI::Text(Font(Header_22), profitLossString, Align::Left, {0.0f, 0.0f});
      
      KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
    }
    ImGui::EndChild();
  }
  
  void Panel::ShowHolding(Holding& h)
  {
    IK_PERFORMANCE_FUNC("Panel::ShowHolding");

    ImVec2 childSize(ImGui::GetContentRegionAvail().x, 120.0f);
    KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, KanVasX::Color::Alpha(KanVasX::Color::BackgroundDark, 0.2f));
    if (ImGui::BeginChild(h.symbolName.c_str(), childSize, true))
    {
      
      // Symbol PL
      {
        KanVasX::UI::Text(Font(Header_28), h.symbolName, Align::Left, {5.0f, 5.0f});
        ImGui::SameLine();
        
        // Right Data
        {
          ImU32 profitLossColor = h.profitLoss > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;
          std::string pnlSign = h.profitLoss > 0 ? "+" : "";
          std::string profitLossString = pnlSign + "₹" + FinalString(Utils::FormatWithCommas((int32_t)h.profitLoss));
          std::string profitLossPercentString = " (" + pnlSign + FinalString(Utils::FormatDoubleToString(h.profitLossPercent)) + "%)";

          KanVasX::ScopedFont header(Font(Header_26));
          {
            KanVasX::ScopedColor textColor(ImGuiCol_Text, profitLossColor);
            
            float textSize = ImGui::CalcTextSize(profitLossString.data()).x + ImGui::CalcTextSize(profitLossPercentString.data()).x;
            float xOffset = (ImGui::GetColumnWidth() - textSize - 5.0f) ;
            
            ImGui::SetCursorPos({ImGui::GetCursorPosX() + xOffset, ImGui::GetCursorPosY()});
            ImGui::Text("%s", profitLossString.data());
          }
          ImGui::SameLine();
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
          
          std::string pnlSign = h.dayChangePercent > 0 ? "+" : "";
          ImU32 ltpColor = h.dayChangePercent > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;

          std::string ltpString = "LTP ";
          std::string ltpValueString = "₹" + Utils::FormatDoubleToString(h.stockValue);
          std::string ltpPerString = " (" + pnlSign + FinalString(Utils::FormatDoubleToString(h.dayChangePercent)) + "%)";
          
          KanVasX::ScopedFont header(Font(Header_18));
          float textSize = ImGui::CalcTextSize(ltpString.data()).x + ImGui::CalcTextSize(ltpValueString.data()).x + ImGui::CalcTextSize(ltpPerString.data()).x;
          float xOffset = (ImGui::GetColumnWidth() - textSize - 8.0f) ;
          
          ImGui::SetCursorPos({ImGui::GetCursorPosX() + xOffset, ImGui::GetCursorPosY()});
          ImGui::Text("%s", ltpString.data());
          
          {
            KanVasX::ScopedColor textColor(ImGuiCol_Text, ltpColor);
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
          
          std::string pnlSign = h.dayChange > 0 ? "+" : "";
          ImU32 ltpColor = h.dayChange > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;

          std::string gainTagString = h.profitLoss > 0 ? "Range Gain " : "Range Loss";
          std::string gainValueString = "₹" + Utils::FormatDoubleToString(h.dayChange);
          
          KanVasX::ScopedFont header(Font(Header_18));
          float textSize = ImGui::CalcTextSize(gainTagString.data()).x + ImGui::CalcTextSize(gainValueString.data()).x;
          float xOffset = (ImGui::GetColumnWidth() - textSize - 5.0f) ;
          
          ImGui::SetCursorPos({ImGui::GetCursorPosX() + xOffset, ImGui::GetCursorPosY()});
          ImGui::Text("%s", gainTagString.data());
          
          {
            KanVasX::ScopedColor textColor(ImGuiCol_Text, ltpColor);
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
        std::string invextedString = "₹" + Utils::FormatWithCommas(int32_t(h.investment));
        KanVasX::UI::Text(Font(Header_20), invextedString, Align::Left, {0.0f, 0.0f});
        
        // Right Data
        {
          ImGui::SameLine();
          
          std::string valueTagString = "Current ";
          std::string valueString = "₹" + Utils::FormatWithCommas(int32_t(h.value));
          
          KanVasX::ScopedFont header(Font(Header_20));
          float textSize = ImGui::CalcTextSize(valueTagString.data()).x + ImGui::CalcTextSize(valueString.data()).x;
          float xOffset = (ImGui::GetColumnWidth() - textSize - 5.0f) ;
          
          ImGui::SetCursorPos({ImGui::GetCursorPosX() + xOffset, ImGui::GetCursorPosY()});
          ImGui::Text("%s", valueTagString.data());
          
          ImGui::SameLine();
          ImGui::Text("%s", valueString.data());
        }
      }
    }
    ImGui::EndChild();
  }
 
  void Panel::EditHolding(Portfolio* portfolio, Holding &h)
  {
    IK_PERFORMANCE_FUNC("Panel::EditHolding");

    ImVec2 childSize(ImGui::GetContentRegionAvail().x, 135.0f);
    KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, KanVasX::Color::Alpha(KanVasX::Color::BackgroundDark, 0.2f));
    if (ImGui::BeginChild(h.symbolName.c_str(), childSize, true))
    {
      // Symbol PL
      {
        {
          KanVasX::ScopedFont header(Font(Header_34));
          ImGui::PushItemWidth(100);
          ImGui::InputTextWithHint("##symbol", "Symbol", symbolBuf, IM_ARRAYSIZE(symbolBuf));
        }
        ImGui::SameLine();
        
        // Right Data
        {
          std::string pnlSign = h.profitLoss > 0 ? "+" : "-";
          std::string profitLossString = pnlSign + "₹" + FinalString(Utils::FormatWithCommas((int32_t)h.profitLoss));
          std::string profitLossPercentString = " (" + pnlSign + FinalString(Utils::FormatDoubleToString(h.profitLossPercent)) + "%)";
          ImU32 profitLossColor = h.profitLoss > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;

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
        
        {
          KanVasX::ScopedFont header(Font(Header_18));
          ImGui::PushItemWidth(50);
          ImGui::InputFloat("##avg", &avgPrice, 0, 0, "%.2f");
        }
        
        // Right Data
        {
          ImGui::SameLine();
          
          std::string pnlSign = h.dayChangePercent > 0 ? "+" : "-";
          std::string ltpString = "LTP ";
          std::string ltpValueString = "₹" + Utils::FormatDoubleToString(h.stockValue);
          std::string ltpPerString = " (" + pnlSign + FinalString(Utils::FormatDoubleToString(h.dayChangePercent)) + "%)";
          ImU32 profitLossColor = h.dayChangePercent > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;

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
        {
          KanVasX::ScopedFont header(Font(Header_18));
          ImGui::InputFloat("##qty", &quantity, 0, 0, "%.2f");
        }
        
        // Right Data
        {
          ImGui::SameLine();
          
          std::string gainTagString = h.profitLoss > 0 ? "Range Gain " : "Range Loss ";
          std::string gainValueString = "₹" + Utils::FormatDoubleToString(h.dayChange);
          ImU32 profitLossColor = h.dayChangePercent > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;

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
        std::string invextedString = "₹" + Utils::FormatWithCommas(int32_t(h.investment));
        KanVasX::UI::Text(Font(Header_20), invextedString, Align::Left, {0.0f, 0.0f});
        
        // Right Data
        {
          ImGui::SameLine();
          
          std::string valueTagString = "Current ";
          std::string valueString = "₹" + Utils::FormatWithCommas(int32_t(h.value));
          
          KanVasX::ScopedFont header(Font(Header_20));
          float textSize = ImGui::CalcTextSize(valueTagString.data()).x + ImGui::CalcTextSize(valueString.data()).x;
          float xOffset = (ImGui::GetColumnWidth() - textSize - 5.0f) ;
          
          ImGui::SetCursorPos({ImGui::GetCursorPosX() + xOffset, ImGui::GetCursorPosY()});
          ImGui::Text("%s", valueTagString.data());
          
          ImGui::SameLine();
          ImGui::Text("%s", valueString.data());
        }
      }
      
      if (ImGui::IsKeyPressed(ImGuiKey_Escape))
      {
        g_showEditModal = false;
      }
      if (g_showEditModal and (ImGui::IsKeyDown(ImGuiKey_LeftSuper) and ImGui::IsKeyPressed(ImGuiKey_Enter)))
      {
        std::string symbol = symbolBuf;
        if (!symbol.empty() && avgPrice > 0 && quantity > 0)
        {
          Holding h;
          h.symbolName = symbol;
          h.averagePrice = avgPrice;
          h.quantity = quantity;
          
          PortfolioController portfolioController(*portfolio);
          portfolioController.EditHolding(h);
          UserManager::GetCurrentUser().SavePortfolio();
          
          g_showEditModal = false;
        }
      }
    }
    ImGui::EndChild();
  }
  void Panel::NewHolding(Portfolio* portfolio)
  {
    IK_PERFORMANCE_FUNC("Panel::NewHolding");
    
    ImVec2 childSize(ImGui::GetContentRegionAvail().x, 110.0f);
    KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, KanVasX::Color::Alpha(KanVasX::Color::BackgroundDark, 0.2f));
    if (ImGui::BeginChild("New Holding", childSize, true))
    {
      // Symbol PL
      {
        KanVasX::ScopedFont header(Font(Header_32));
        ImGui::PushItemWidth(100);
        ImGui::InputTextWithHint("##symbol", "Symbol", symbolBuf, IM_ARRAYSIZE(symbolBuf));
      }
      
      // ATP LTP
      {
        KanVasX::UI::Text(Font(Header_18), "ATP ", Align::Left, {5.0f, 5.0f});
        ImGui::SameLine();
        
        {
          KanVasX::ScopedFont header(Font(Header_18));
          ImGui::PushItemWidth(50);
          ImGui::InputFloat("##avg", &avgPrice, 0, 0, "%.2f");
        }
      }
      
      // Shares
      {
        KanVasX::UI::Text(Font(Header_18), "Shares ", Align::Left, {5.0f, 0.0f});
        ImGui::SameLine();
        {
          KanVasX::ScopedFont header(Font(Header_18));
          ImGui::InputFloat("##qty", &quantity, 0, 0, "%.2f");
        }
      }
            
      if (ImGui::IsKeyPressed(ImGuiKey_Escape))
      {
        g_showAddRow = false;
      }
      if (g_showAddRow and ImGui::IsKeyDown(ImGuiKey_LeftSuper) and ImGui::IsKeyPressed(ImGuiKey_Enter))
      {
        std::string symbol = symbolBuf;
        if (!symbol.empty() && avgPrice > 0 && quantity > 0)
        {
          Holding h;
          h.symbolName = symbol;
          h.averagePrice = avgPrice;
          h.quantity = quantity;
          
          PortfolioController portfolioController(*portfolio);
          portfolioController.AddHolding(h);
          UserManager::GetCurrentUser().SavePortfolio();
          
          g_showAddRow = false;
        }
      }
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

    KanVasX::UI::ShiftCursor({5.0f, 1.0f});
    ShowHoldingSearchBar(5.0f);
    KanVasX::UI::ShiftCursorY(1.0f);

    float totalWidth  = ImGui::GetContentRegionAvail().x;
    float totalHeight = ImGui::GetContentRegionAvail().y;
    
    if (g_showAddRow)
    {
      NewHolding(portfolio);
    }

    KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, KanVasX::Color::Alpha(KanVasX::Color::Highlight, 0.2f));
    if (ImGui::BeginChild("Holding Data", ImVec2(totalWidth, totalHeight), false, ImGuiWindowFlags_NoScrollbar))
    {
      for (int idx = 0; idx < holdings.size(); idx++)
      {
        auto& h = holdings[idx];
        
        if (!Utils::IsMatchingSearch(h.symbolName, s_searchedHoldingString))
        {
          continue;
        }
        
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

        if (g_showEditModal and g_selectedIdx == idx)
        {
          EditHolding(portfolio, h);
        }
        else
        {
          ShowHolding(h);
        }

        if (ImGui::IsItemClicked())
        {
          g_selectedIdx = idx;
          StockManager::SetSelectedStockSymbol(h.symbolName);
        }
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
          g_selectedIdx = idx;
          g_showEditModal = true;
          
          memcpy(symbolBuf, h.symbolName.c_str(), 32);
          avgPrice = h.averagePrice;
          quantity = h.quantity;
        }
        KanVasX::UI::ShiftCursorY(1.0f);
      }
      
      if (ImGui::IsKeyDown(ImGuiKey_LeftSuper) && ImGui::IsKeyPressed(ImGuiKey_N))
      {
        g_showAddRow = !g_showAddRow;
        if (g_showAddRow)
        {
          symbolBuf[0] = '\0';
          avgPrice = 0.0f;
          quantity = 0;
        }
      }
    }
    ImGui::EndChild();
  }
  
  void Panel::ShowStockData()
  {
    IK_PERFORMANCE_FUNC("Panel::ShowStockData");
    
    StockData stockData = StockManager::GetSelectedStockData();
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(Font(Header_24), "No data for stock", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Error);
      return;
    }

    KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, KanVasX::Color::Alpha(KanVasX::Color::Value(KanVasX::Color::Background, 2.0f), 0.2f));
    if (ImGui::BeginChild(" Stock Data ", ImVec2(0.0f, 150.0f )))
    {
      // Name & price
      std::string name = stockData.shortName;
      std::string longNameName = stockData.longName != "" ? stockData.longName : stockData.shortName;

      KanVasX::UI::Text(Font(Header_46), name, Align::Left, {20.0f, 10.0f}, KanVasX::Color::TextBright);
      KanVasX::UI::Text(Font(Header_26), longNameName, Align::Left, {20.0f, 0.0f}, KanVasX::Color::TextBright);
      KanVasX::UI::Text(Font(Header_56), KanVest::UI::Utils::FormatDoubleToString(stockData.livePrice), Align::Left, {20.0f, 0.0f},
                        KanVasX::Color::TextBright);
      
      // Change
      std::string change = (stockData.change > 0 ? "+" : "") +
      KanVest::UI::Utils::FormatDoubleToString(stockData.change) +
      (stockData.change > 0 ? " ( +" : " ( ") +
      KanVest::UI::Utils::FormatDoubleToString(stockData.changePercent) + "%)";
      
      ImU32 changeColor = stockData.change > 0 ? KanVasX::Color::Cyan : KanVasX::Color::Red;
      ImGui::SameLine();
      KanVasX::UI::Text(Font(Header_30), change, Align::Left, {20.0f, 15.0f}, changeColor);

      KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
    }
    ImGui::EndChild();
  
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

    KanVasX::UI::ShiftCursorY(5.0f);
    KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1);
    
    ShowStockAnalyzerSummary();
  }
  
  void Panel::ShowStockSearchBar(float height)
  {
    IK_PERFORMANCE_FUNC("Panel::ShowSearchBar");
    const float contentRegionAvailX = ImGui::GetContentRegionAvail().x;
    if (KanVasX::Widget::Search(s_searchedStockString, 128, height, contentRegionAvailX - 30.0f, "Enter Symbol ...", Font(Large), true))
    {
      Utils::ConvertUpper(s_searchedStockString);
    }
    
    ImGui::SameLine();
    float iconSize = ImGui::GetItemRectSize().y - 12;
    if (KanVasX::UI::DrawButtonImage("Refresh", s_reloadIconID, false, {iconSize, iconSize}, {0.0, 6.0}) or ImGui::IsKeyPressed(ImGuiKey_Enter))
    {
      AddStockInManager(s_searchedStockString);
    }
  }

  void Panel::ShowHoldingSearchBar(float height)
  {
    IK_PERFORMANCE_FUNC("Panel::ShowSearchBar");
    const float contentRegionAvailX = ImGui::GetContentRegionAvail().x;
    if (KanVasX::Widget::Search(s_searchedHoldingString, 128, height, contentRegionAvailX - 40.0f, "Enter Symbol ...", Font(Large), true))
    {
      Utils::ConvertUpper(s_searchedHoldingString);
    }
    
    ImGui::SameLine();
    float iconSize = ImGui::GetItemRectSize().y - 12;
    if (KanVasX::UI::DrawButtonImage("Setting", s_settingIconID, false, {iconSize, iconSize}, {0.0, 6.0}))
    {
      ImGui::OpenPopup("SettingModel");
      g_showSettingModel = false;
    }
    
    if (KanVasX::UI::BeginPopup("SettingModel"))
    {
      KanVasX::UI::EndPopup();
    }
  }
  
  void Panel::ShowChart()
  {
    IK_PERFORMANCE_FUNC("Panel::ShowChart");
    StockData stockData = StockManager::GetSelectedStockData();
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(Font(Header_24), "No Chart Available", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Error);
      return;
    }
    
    const auto& history = stockData.history;

    if (history.empty())
    {
      KanVasX::UI::Text(Font(Header_24), "No Chart Available", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Error);
      return;
    }

    // Keep only trading days (you already had this).
    std::vector<StockPoint> filtered = Utils::FilterTradingDays(history);
    if (filtered.empty())
    {
      KanVasX::UI::Text(Font(Header_24), "No Chart Available", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Error);
      return;
    }

    // We'll use sequential x indices so weekends/holidays are skipped visually.
    std::vector<double> xs, opens, highs, lows, closes;
    xs.reserve(filtered.size());
    opens.reserve(filtered.size());
    highs.reserve(filtered.size());
    lows.reserve(filtered.size());
    closes.reserve(filtered.size());
    
    double ymin = DBL_MAX;
    double ymax = -DBL_MAX;

    for (size_t i = 0; i < filtered.size(); ++i)
    {
      const auto& h = filtered[i];
      xs.push_back(static_cast<double>(i)); // sequential index
      opens.push_back(h.open);
      highs.push_back(h.high);
      lows.push_back(h.low);
      closes.push_back(h.close);
      ymin = std::min(ymin, h.low);
      ymax = std::max(ymax, h.high);
    }
    
    // Build human-readable labels locally (e.g., "2025-11-03").
    // To avoid too many labels, we'll place a label every few points.
    std::vector<std::string> labelStrings;
    std::vector<const char*> labelPtrs;
    std::vector<double> labelPositions;

    // Decide label step based on number of points: aim for ~8-12 labels max.
    int targetLabels = 10;
    size_t n = filtered.size();
    size_t labelStep = 1;
    if (n > 0)
    {
      labelStep = std::max<size_t>(1, (n + targetLabels - 1) / targetLabels);
    }

    labelStrings.reserve((n + labelStep - 1) / labelStep);
    labelPositions.reserve(labelStrings.capacity());
    labelPtrs.reserve(labelStrings.capacity());

    for (size_t i = 0; i < n; i += labelStep)
    {
      // Convert timestamp to UTC date string
      time_t t = static_cast<time_t>(filtered[i].timestamp);
      struct tm tm{};

      gmtime_r(&t, &tm);

      char buf[64];
      // Format: YYYY-MM-DD (change format if you want time too)
      std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
      labelStrings.emplace_back(buf);
      labelPositions.push_back(static_cast<double>(i));
    }

    // Convert label strings to char* for ImPlot
    for (auto &s : labelStrings)
      labelPtrs.push_back(s.c_str());

    // Start plotting
    if (ImPlot::BeginPlot("##", ImVec2(ImGui::GetContentRegionAvail().x - 1.0f, 300.0f)))
    {
      // We use AutoFit for X and set Y limits explicitly
      ImPlot::SetupAxes("", "", ImPlotAxisFlags_NoDecorations | ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
      ImPlot::SetupAxisLimits(ImAxis_Y1, ymin - 1.0, ymax + 1.0, ImGuiCond_Always);

      // Setup X axis ticks with our custom positions and labels (compressed timeline)
      if (!labelPositions.empty())
      {
        // ImPlot::SetupAxisTicks accepts arrays of positions and labels.
        // Provide positions (double*) and labels (const char*[])
        ImPlot::SetupAxisTicks(ImAxis_X1, labelPositions.data(), static_cast<int>(labelPositions.size()), labelPtrs.data());
      }

      // Plot close line (optional)
      ImPlot::PlotLine("", xs.data(), closes.data(), static_cast<int>(xs.size()));

      ImDrawList* drawList = ImPlot::GetPlotDrawList();

      if (s_showCandle)
      {
        for (size_t i = 0; i < xs.size(); ++i)
        {
          ImU32 color = (closes[i] >= opens[i]) ? KanVasX::Color::Cyan : KanVasX::Color::Red;
          
          ImVec2 pHigh  = ImPlot::PlotToPixels(ImPlotPoint(xs[i], highs[i]));
          ImVec2 pLow   = ImPlot::PlotToPixels(ImPlotPoint(xs[i], lows[i]));
          ImVec2 pOpen  = ImPlot::PlotToPixels(ImPlotPoint(xs[i], opens[i]));
          ImVec2 pClose = ImPlot::PlotToPixels(ImPlotPoint(xs[i], closes[i]));
          
          // Wick
          drawList->AddLine(pLow, pHigh, IM_COL32(200, 200, 200, 255));
          
          float top = std::min(pOpen.y, pClose.y);
          float bottom = std::max(pOpen.y, pClose.y);
          float cx = pOpen.x;
          float width = 4.0f;
          
          // Filled body and border
          drawList->AddRectFilled(ImVec2(cx - width, top), ImVec2(cx + width, bottom), color);
          drawList->AddRect(ImVec2(cx - width, top), ImVec2(cx + width, bottom), IM_COL32(40, 40, 40, 255));
        }
      }
      
      // --- Tooltip (hover info with date + time) ---
      if (ImPlot::IsPlotHovered() && !filtered.empty())
      {
        ImPlotPoint mouse = ImPlot::GetPlotMousePos();
        
        // Find nearest candle index
        int idx = (int)std::round(mouse.x);
        idx = std::clamp(idx, 0, (int)filtered.size() - 1);
        
        const StockPoint& p = filtered[idx];
        
        // Convert timestamp → readable date + time
        time_t t = static_cast<time_t>(p.timestamp);
        struct tm tm {};
#if defined(_WIN32)
        localtime_s(&tm, &t); // Use local time for readability
#else
        localtime_r(&t, &tm);
#endif
        char dateTimeBuf[64];
        std::strftime(dateTimeBuf, sizeof(dateTimeBuf), "%Y-%m-%d %H:%M", &tm);
        
        // Draw tooltip near the cursor
        {
          KanVasX::ScopedFont formattedText(Font(FixedWidthHeader_16));
          ImGui::BeginTooltip();
          ImGui::TextColored(ImVec4(1, 0.8f, 0, 1), "%s", dateTimeBuf);
          ImGui::Separator();
          ImGui::Text("Open : %.2f", p.open);
          ImGui::Text("High : %.2f", p.high);
          ImGui::Text("Low  : %.2f", p.low);
          ImGui::Text("Close: %.2f", p.close);
          ImGui::EndTooltip();
        }
      }
      
      ImPlot::EndPlot();
    }
    
    bool modify = false;
    for (int i = 0; i < IM_ARRAYSIZE(StockManager::ValidRange); ++i)
    {
      auto buttonColor = StockManager::ValidRange[i] == StockManager::GetCurrentRange() ? KanVasX::Color::Gray : KanVasX::Color::BackgroundDark;
      
      std::string displayText = StockManager::ValidRange[i];
      std::string uniqueLabel = displayText + "##Range" + std::to_string(i);
      if (KanVasX::UI::DrawButton(uniqueLabel, nullptr, buttonColor))
      {
        StockManager::SetCurrentRange(StockManager::ValidRange[i]);
        StockManager::SetCurrentInterval(StockManager::RangeIntervalMap[StockManager::GetCurrentRange()][1].c_str());
        
        modify = true;
      }
      if (i < IM_ARRAYSIZE(StockManager::ValidRange) - 1)
      {
        ImGui::SameLine();
      }
    }
    
    ImGui::SameLine();
    KanVasX::UI::ShiftCursorX(20);
    ImGui::Checkbox(" Show Candle", &s_showCandle);
    
    ImGui::SameLine();
    std::vector<std::string> intervalValues = StockManager::RangeIntervalMap[StockManager::GetCurrentRange()];
    KanVasX::UI::ShiftCursorX(ImGui::GetContentRegionAvail().x - (intervalValues.size() * 40));
    
    for (int i = 0; i < intervalValues.size(); ++i)
    {
      auto buttonColor = intervalValues[i] == StockManager::GetCurrentInterval() ? KanVasX::Color::Gray : KanVasX::Color::BackgroundDark;
      
      std::string displayText = intervalValues[i];
      std::string uniqueLabel = displayText + "##Interval" + std::to_string(i);
      
      if (KanVasX::UI::DrawButton(uniqueLabel, nullptr, buttonColor))
      {
        StockManager::SetCurrentInterval(intervalValues[i].c_str());
        modify = true;
      }
      if (i < intervalValues.size())
      {
        ImGui::SameLine();
      }
    }
    
    ImGui::NewLine();
  }
  
  void Panel::ShowStockAnalyzerSummary()
  {
    IK_PERFORMANCE_FUNC("Panel::ShowChart");
    
    // Selected stock data
    StockData stockData = StockManager::GetSelectedStockData();
    if (!stockData.IsValid())
    {
      return;
    }
    
    // Set current holding
    Analyzer::SetHoldings(UserManager::GetCurrentUser().portfolio->GetHolding(stockData.symbol));
    
    // Get recommendataion data
    Recommendation recommendation = Analyzer::AnalzeStock(stockData);
    
    float score = static_cast<float>(recommendation.score);
    
    // Determine color based on score
    ImU32 scoreColor;
    std::string scoreString = "Technically Neutral";

    if (score < 15)      { scoreString = "Technically Strong Bearish"; scoreColor = KanVasX::Color::Red; }
    else if (score < 30) { scoreString = "Technically Bearish"; scoreColor = KanVasX::Color::Orange; }
    else if (score < 60) { scoreString = "Technically Neutral"; scoreColor = KanVasX::Color::Yellow; }
    else if (score < 80) { scoreString = "Technically Bullish"; scoreColor = KanVasX::Color::Cyan; }
    else                 { scoreString = "Technically Strong Bullish"; scoreColor = KanVasX::Color::Green; }

    // Scoped color
    {
      KanVasX::ScopedStyle headerPaddingAndHeight(ImGuiStyleVar_FramePadding, ImVec2{1.0f, 1.0f});
      KanVasX::ScopedColor plotColor(ImGuiCol_PlotHistogram, scoreColor);
      // Convert score 0-100 to fraction 0.0-1.0

      // Print Score
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_32), Utils::FormatDoubleToString(score), KanVasX::UI::AlignX::Left, {10.0f, 0}, scoreColor);

      // Print /100
      static const std::string totalScoreString = "/100";
      ImGui::SameLine();
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Medium), totalScoreString, KanVasX::UI::AlignX::Left, {0, 10.0f}, KanVasX::Color::White);

      ImGui::SameLine();
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Large), scoreString, KanVasX::UI::AlignX::Right, {-10.0f, 5.0f}, scoreColor);

      float fraction = score / 100.0f;
      ImGui::ProgressBar(fraction, ImVec2(-1, 0), "");
    }
  }
  
  void Panel::ShowStockTechnicalData()
  {
    IK_PERFORMANCE_FUNC("Panel::ShowStockTechnicalData");
    KanVasX::UI::DrawFilledRect(KanVasX::Color::Alpha(KanVasX::Color::Value(KanVasX::Color::Background, 2.0f), 0.2f), 30.0f);
    
    KanVasX::UI::Text(Font(Header_26), "Technicals", KanVasX::UI::AlignX::Center, {0.0f, 2.0f});
    
    enum class TechnicalTab {SMA, EMA, RSI, MACD, ADX, MFI, Stochastic, BB, Pivot, Max};
    static TechnicalTab tab = TechnicalTab::SMA;
    
    float availX = ImGui::GetContentRegionAvail().x;
    float technicalButtonSize = (availX / (uint32_t)TechnicalTab::Max) - 2.0f;
    
    auto TechnicalButton = [technicalButtonSize](const std::string& title, TechnicalTab checkerTtab, const std::string& tooltip)
    {
      if (KanVasX::UI::DrawButton(title, KanVest::UI::Font::Get(KanVest::UI::FontType::Medium),
                                  tab == checkerTtab ? KanVasX::Color::Value(KanVasX::Color::Background, 1.0f) : KanVasX::Color::BackgroundDark,
                                  KanVasX::Color::TextBright, false, 5.0f, {technicalButtonSize, 30}))
      {
        tab = checkerTtab;
      }
      KanVasX::UI::Tooltip(tooltip);
      KanVasX::UI::DrawItemActivityOutline();
    };
    
    KanVasX::UI::ShiftCursor({2.0f, 1.0f});
    TechnicalButton("SMA", TechnicalTab::SMA, "Simple Moving Average"); ImGui::SameLine();
    TechnicalButton("EMA", TechnicalTab::EMA, "Exponantial Moving Average"); ImGui::SameLine();
    TechnicalButton("RSI", TechnicalTab::RSI, "Relative Strength Indicator"); ImGui::SameLine();
    TechnicalButton("MACD", TechnicalTab::MACD, "Moving Average Convergence Divergence"); ImGui::SameLine();
    TechnicalButton("MFI", TechnicalTab::MFI, "Tooltip"); ImGui::SameLine();
    TechnicalButton("ADX", TechnicalTab::ADX, "Tooltip"); ImGui::SameLine();
    TechnicalButton("Stochastic", TechnicalTab::Stochastic, "Tooltip"); ImGui::SameLine();
    TechnicalButton("BB", TechnicalTab::BB, "Tooltip"); ImGui::SameLine();
    TechnicalButton("Pivot", TechnicalTab::Pivot, "Tooltip");
    ImGui::Separator();
    
    if (tab == TechnicalTab::SMA)
    {
      UI_MovingAverage::ShowSMA(StockManager::GetSelectedStockData(), s_shadowTextureID);
    }
    else if (tab == TechnicalTab::EMA)
    {
      UI_MovingAverage::ShowEMA(StockManager::GetSelectedStockData(), s_shadowTextureID);
    }
    else if (tab == TechnicalTab::RSI)
    {
      UI_Momentum::ShowRSI(StockManager::GetSelectedStockData());
    }
    else if (tab == TechnicalTab::MACD)
    {
      UI_MACD::Show(StockManager::GetSelectedStockData());
    }
  }

  void Panel::AddStockInManager(const std::string& symbol)
  {
    StockManager::AddStock(symbol);
    StockManager::SetSelectedStockSymbol(symbol);
  }
} // namespace KanVest::UI
