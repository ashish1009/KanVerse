//
//  UI_Utils.hpp
//  KanVest
//
//  Created by Ashish . on 09/12/25.
//

#pragma once

namespace KanVest::UI::Utils
{
  static ImU32 StockProfitColor = KanVasX::Color::Green;
  static ImU32 StockLossColor = KanVasX::Color::Red;
  
  std::string FormatDoubleToString(double value);
  std::string FormatLargeNumber(double value);
} // namespace KanVest::UI::Utils
