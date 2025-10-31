//
//  StockParser.cpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#include "StockParser.hpp"

#include "Stocks/URL_Manager.hpp"

namespace KanVest
{
  double StockParser::ExtractValue(const std::string& text, const std::string& key)
  {
    std::string patternStr = URL::GetValueParserPattern(key);
    std::regex pattern(patternStr);
    std::smatch match;
    
    if (std::regex_search(text, match, pattern))
    {
      return std::stod(match[1].str());
    }
    
    return -1;
  }
} // namespace KanVest
