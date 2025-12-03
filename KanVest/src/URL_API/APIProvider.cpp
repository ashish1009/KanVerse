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

  std::string API_Provider::GetIntervalString(Interval interval)
  {
    switch (interval)
    {
      case Interval::_1M: return "1m";
      case Interval::_2M: return "2m";
      case Interval::_5M: return "5m";
      case Interval::_15M: return "15m";
      case Interval::_30M: return "30m";
      case Interval::_60M: return "60m";
      case Interval::_90M: return "90m";
      case Interval::_1H: return "1h";
      case Interval::_1D: return "1d";
      case Interval::_5D: return "5d";
      case Interval::_1WK: return "1wk";
      case Interval::_1MO: return "1mo";
      case Interval::_3MO: return "3mo";
      default:
        break;
    }
    IK_ASSERT(false, "Invalid interval")
    return "";
  }
  std::string API_Provider::GetRangeString(Range range)
  {
    switch (range)
    {
      case Range::_1D: return "1d";
      case Range::_5D: return "5d";
      case Range::_1MO: return "1mo";
      case Range::_6MO: return "6mo";
      case Range::_YTD: return "ytd";
      case Range::_1Y: return "1y";
      case Range::_5Y: return "5y";
      case Range::_MAX: return "max";
      default:
        break;
    }
    IK_ASSERT(false, "Invalid renage")
    return "";
  }

} // namespace KanVest
