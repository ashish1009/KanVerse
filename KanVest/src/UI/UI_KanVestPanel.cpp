//
//  UI_KanVestPanel.cpp
//  KanVest
//
//  Created by Ashish . on 02/12/25.
//

#include "UI_KanVestPanel.hpp"

#include "Stock/StockManager.hpp"

namespace KanVest::UI
{
#define Font(font) KanVest::UI::Font::Get(KanVest::UI::FontType::font)
  using Align = KanVasX::UI::AlignX;

  void Panel::Show()
  {
    IK_PERFORMANCE_FUNC("Panel::Show");
    
    KanVasX::Panel::Begin("Stock Analyzer");
    
    static constexpr std::string symbolName = "Nifty";
    StockData data = StockManager::GetStockData(symbolName);
    if (!data.IsValid())
    {
      std::string errorMessage = "Invalid stock data for " + symbolName;
      KanVasX::UI::Text(Font(Header_22), errorMessage, Align::Left, {0.0f, 0.0f}, KanVasX::Color::Error);
    }
    
    KanVasX::Panel::End(0.0f);
  }
} // namespace KanVest::UI
