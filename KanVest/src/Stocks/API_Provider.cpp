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
  
  std::string API_Provider::GetStringParserPattern(const std::string& key)
  {
    return "\"" + key + "\"\\s*:\\s*\"([^\"]+)\"";
  }
  std::string API_Provider::GetArrayParserPattern(const std::string& key)
  {
    return "\"" + key + "\"\\s*:\\s*\\[([^\\]]+)\\]";
  }

  APIKeys API_Provider::GetAPIKeys()
  {
    APIKeys keys;
    
    switch (s_stockAPIProvider)
    {
      case StockAPIProvider::Yahoo:
        keys.price            = "regularMarketPrice";
        keys.prevClose        = "chartPreviousClose";
        keys.changePercent    = "regularMarketChangePercent";
        keys.volume           = "regularMarketVolume";
        
        keys.shortName        = "shortName";
        keys.longName         = "longName";
        keys.currency         = "currency";
        keys.exchangeName     = "exchangeName";
        
        keys.instrumentType   = "instrumentType";
        keys.dataGranularity  = "dataGranularity";
        keys.range            = "range";
        keys.timezone         = "timezone";
        
        keys.fiftyTwoHigh     = "fiftyTwoWeekHigh";
        keys.fiftyTwoLow      = "fiftyTwoWeekLow";
        keys.dayHigh          = "regularMarketDayHigh";
        keys.dayLow           = "regularMarketDayLow";
        
        break;
        
      default:
        IK_ASSERT(false, "Invalid API")
    }
    
    return keys;
  }
} // namespace KanVest
