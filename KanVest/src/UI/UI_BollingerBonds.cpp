//
//  UI_BollingerBonds.cpp
//  KanVest
//
//  Created by Ashish . on 19/11/25.
//

#include "UI_BollingerBonds.hpp"

#include "UI/UI_Utils.hpp"

#include "Analyzer/Indicator/BollingerBands.hpp"

namespace KanVest
{
  static bool IsValid(double v)
  {
    return !std::isnan(v) && !std::isinf(v);
  };
  
  static void Buildhpp_UI(const BBResult& rsiData)
  {
  };
  
  void UI_BollingerBonds::Show(const StockData& stockData)
  {
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_24), "No data for stock", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Error);
      return;
    }
  }
} // namespace KanVest
