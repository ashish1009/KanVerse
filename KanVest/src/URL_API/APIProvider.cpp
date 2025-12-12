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
  
  Interval API_Provider::GetIntervalFromString(const std::string& interval)
  {
    if (interval == "1m") return Interval::_1M;
    else if (interval == "2m") return Interval::_2M;
    else if (interval == "5m") return Interval::_5M;
    else if (interval == "15m") return Interval::_15M;
    else if (interval == "30m") return Interval::_30M;
    else if (interval == "60m") return Interval::_60M;
    else if (interval == "90m") return Interval::_90M;
    else if (interval == "1h") return Interval::_1H;
    else if (interval == "1d") return Interval::_1D;
    else if (interval == "5d") return Interval::_5D;
    else if (interval == "1wk") return Interval::_1WK;
    else if (interval == "1mo") return Interval::_1MO;
    else if (interval == "3mo") return Interval::_3MO;
    
    IK_ASSERT(false, "Invalid interval")
    return Interval::_1M;
  }
  Range API_Provider::GetRangeFromString(const std::string& range)
  {
    if (range == "1d") return Range::_1D;
    else if (range == "5d") return Range::_5D;
    else if (range == "1mo") return Range::_1MO;
    else if (range == "6mo") return Range::_6MO;
    else if (range == "ytd") return Range::_YTD;
    else if (range == "1y") return Range::_1Y;
    else if (range == "5y") return Range::_5Y;
    else if (range == "max") return Range::_MAX;

    IK_ASSERT(false, "Invalid renage")
    return Range::_1D;
  }

  std::vector<std::string> API_Provider::GetValidRanges()
  {
    return {"1d","5d","1mo","6mo","ytd","1y","5y","max"};
  }
  std::vector<std::string> API_Provider::GetValidIntervals()
  {
    return {"1m","2m","5m","15m","30m","60m","90m","1h","1d","5d","1wk","1mo","3mo"};
  }
  std::vector<std::string> API_Provider::GetValidIntervalsStringForRange(Range range)
  {
    switch (range)
    {
      case Range::_1D:
      case Range::_5D:
        return {"1m","5m","15m","30m","60m"};
      case Range::_1MO:
        return {"5m","15m","30m","60m","1d"};
      case Range::_6MO:
        return {"1h","1d","1wk"};
      case Range::_YTD:
        return {"1h", "1d","1wk","1mo"};
      case Range::_1Y:
      case Range::_5Y:
      case Range::_MAX:
        return {"1d","1wk","1mo"};
        
      default:
        break;
    }
  }
  
  std::vector<Interval> API_Provider::GetValidIntervalsForRange(Range range)
  {
    switch (range)
    {
      case Range::_1D:
      case Range::_5D:
        return {Interval::_1M, Interval::_5M, Interval::_15M, Interval::_30M, Interval::_60M};
      case Range::_1MO:
        return {Interval::_5M, Interval::_15M, Interval::_30M, Interval::_60M, Interval::_1D};
      case Range::_6MO:
        return {Interval::_1H, Interval::_1D, Interval::_1WK};
      case Range::_YTD:
        return {Interval::_1H, Interval::_1D, Interval::_1WK, Interval::_1MO};
      case Range::_1Y:
      case Range::_5Y:
      case Range::_MAX:
        return {Interval::_1D, Interval::_1WK, Interval::_1MO};
        
      default:
        break;
    }
  }

  std::string API_Provider::GetValidIntervalStringForRange(Range range)
  {
    switch (range)
    {
      case Range::_1D:
        return "2m";
      case Range::_5D:
        return "15m";
      case Range::_1MO:
        return "60m";
      case Range::_6MO:
      case Range::_YTD:
      case Range::_1Y:
      case Range::_5Y:
      case Range::_MAX:
        return "1d";
        
      default:
        break;
    }
  }
  
  Interval API_Provider::GetValidIntervalForRange(Range range)
  {
    switch (range)
    {
      case Range::_1D:
        return Interval::_2M;
      case Range::_5D:
        return Interval::_15M;
      case Range::_1MO:
        return Interval::_60M;
      case Range::_6MO:
      case Range::_YTD:
      case Range::_1Y:
      case Range::_5Y:
      case Range::_MAX:
        return Interval::_1D;

      default:
        break;
    }
  }
  
  std::vector<std::string> API_Provider::GetValidIntervalsStringForRange(const std::string& range)
  {
    return GetValidIntervalsStringForRange(GetRangeFromString(range));
  }
  
  std::vector<Interval> API_Provider::GetValidIntervalsForRange(const std::string& range)
  {
    return GetValidIntervalsForRange(GetRangeFromString(range));
  }
  
  std::string API_Provider::GetValidIntervalStringForRange(const std::string& range)
  {
    return GetValidIntervalStringForRange(GetRangeFromString(range));
  }
  
  Interval API_Provider::GetValidIntervalForRange(const std::string& range)
  {
    return GetValidIntervalForRange(GetRangeFromString(range));
  }
} // namespace KanVest
