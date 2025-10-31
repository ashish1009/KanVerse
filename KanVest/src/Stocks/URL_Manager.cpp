//
//  URL_Manager.cpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#include "URL_Manager.hpp"

namespace KanVest
{
  std::string URL::Get()
  {
    return "https://query1.finance.yahoo.com/v8/finance/chart/";
  };
  
  std::string URL::GetValueParserPattern(const std::string& key)
  {
    return "\"" + key + "\"\\s*:\\s*([-+]?[0-9]*\\.?[0-9]+)";
  }
} // namespace KanVest
