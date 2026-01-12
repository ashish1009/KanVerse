//
//  UI_Chart.cpp
//  KanVest
//
//  Created by Ashish . on 12/01/26.
//

#include "UI_Chart.hpp"

namespace KanVest
{
#define Font(font) KanVest::UI::Font::Get(KanVest::UI::FontType::font)
  
  using Align = KanVasX::UI::AlignX;
  using Color = KanVasX::Color;
  
  void Chart::Show(const StockData &stockData)
  {
    IK_PERFORMANCE_FUNC("Chart::Show");
  }
} // namespace KanVest
