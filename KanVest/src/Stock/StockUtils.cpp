//
//  StockUtils.cpp
//  KanVest
//
//  Created by Ashish . on 02/12/25.
//

#include "StockUtils.hpp"

namespace KanVest::Utils
{
  std::string NormalizeSymbol(const std::string& input)
  {
    std::string symbol = input;
    std::transform(symbol.begin(), symbol.end(), symbol.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    
    // Return Nifty sumbol
    if (symbol == "%5ENSEI" or symbol == "NIFTY")
      return "%5ENSEI";

    // Add .NS if not present
    if (symbol.find('.') == std::string::npos)
      symbol += ".NS";
    
    return symbol;
  }
} // namespace KanVest
