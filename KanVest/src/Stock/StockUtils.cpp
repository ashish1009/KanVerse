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
    std::transform(symbol.begin(), symbol.end(), symbol.begin(), [](unsigned char c) { return std::toupper(c); });
    
    // Return Nifty sumbol
    if (symbol == "%5ENSEI" or symbol == "NIFTY")
    {
      return "%5ENSEI";
    }

    // Add .NS if not present
    if (symbol.find('.') == std::string::npos)
    {
      symbol += ".NS";
    }
    
    return symbol;
  }
  
  std::vector<CandleData> FilterTradingDays(const std::vector<CandleData>& history)
  {
    std::vector<CandleData> filtered;
    
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
} // namespace KanVest
