//
//  UI_MFI.cpp
//  KanVest
//
//  Created by Ashish . on 19/11/25.
//

#include "UI_MFI.hpp"

#include "UI/UI_Utils.hpp"

#include "Analyzer/Indicator/MFI.hpp"

namespace KanVest
{
  static bool IsValid(double v)
  {
    return !std::isnan(v) && !std::isinf(v);
  };
  
  static void BuildMFI_UI(const MFIResult& rsiData)
  {
  };
  
  void UI_MFI::Show(const StockData& stockData)
  {
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_24), "No data for stock", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Error);
      return;
    }
  }
} // namespace KanVest
