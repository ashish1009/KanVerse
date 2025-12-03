//
//  APIProvider.cpp
//  KanVest
//
//  Created by Ashish . on 03/12/25.
//

#include "APIProvider.hpp"

namespace KanVest
{
  void API_Provider::Initialize(StockAPIProvider apiProvider)
  {
    s_stockAPIProvider = apiProvider;
    
    switch (s_stockAPIProvider)
    {
      case StockAPIProvider::Yahoo:
        s_apiKeys.price            = "regularMarketPrice";
        s_apiKeys.prevClose        = "chartPreviousClose";
        s_apiKeys.changePercent    = "regularMarketChangePercent";
        s_apiKeys.volume           = "regularMarketVolume";
        
        s_apiKeys.shortName        = "shortName";
        s_apiKeys.longName         = "longName";
        s_apiKeys.currency         = "currency";
        s_apiKeys.exchangeName     = "exchangeName";
        
        s_apiKeys.instrumentType   = "instrumentType";
        s_apiKeys.dataGranularity  = "dataGranularity";
        s_apiKeys.range            = "range";
        s_apiKeys.timezone         = "timezone";
        
        s_apiKeys.fiftyTwoHigh     = "fiftyTwoWeekHigh";
        s_apiKeys.fiftyTwoLow      = "fiftyTwoWeekLow";
        s_apiKeys.dayHigh          = "regularMarketDayHigh";
        s_apiKeys.dayLow           = "regularMarketDayLow";
        
        break;
        
      default:
        IK_ASSERT(false, "Invalid API")
    }
  }
  
  APIKeys API_Provider::GetAPIKeys()
  {
    return s_apiKeys;
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

} // namespace KanVest
