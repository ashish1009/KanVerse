//
//  API_Provider.cpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#include "API_Provider.hpp"

namespace KanVest
{
  void API_Provider::Initialize(StockAPIProvider apiProvider)
  {
    s_stockAPIProvider = apiProvider;
  }
  
  std::string API_Provider::GetURL()
  {
    switch (s_stockAPIProvider)
    {
      case StockAPIProvider::Yahoo : return "https://query1.finance.yahoo.com/v8/finance/chart/";
      default:
        IK_ASSERT(false, "Invalid API")
    }
    return "";
  }
  
  std::string API_Provider::GetValueParserPattern(const std::string& key)
  {
    return "\"" + key + "\"\\s*:\\s*([-+]?[0-9]*\\.?[0-9]+)";
  }
  
  APIKeys API_Provider::GetAPIKeys()
  {
    APIKeys keys;
    
    switch (s_stockAPIProvider)
    {
      case StockAPIProvider::Yahoo:
        keys.price = "regularMarketPrice";
        break;
        
      default:
        IK_ASSERT(false, "Invalid API")
    }
    
    return keys;
  }
} // namespace KanVest
