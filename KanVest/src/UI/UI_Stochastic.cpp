//
//  UI_Stochastic.cpp
//  KanVest
//
//  Created by Ashish . on 19/11/25.
//

#include "UI_Stochastic.hpp"

#include "UI/UI_Utils.hpp"

#include "Analyzer/Indicator/Stochastic.hpp"

namespace KanVest
{
  static bool IsValid(double v)
  {
    return !std::isnan(v) && !std::isinf(v);
  };
  
  static void BuildMACD_UI(const StochasticResult& rsiData)
  {
  };
  
  void UI_Stochastic::Show(const StockData& stockData)
  {
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_24), "No data for stock", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Error);
      return;
    }
  }
} // namespace KanVest
