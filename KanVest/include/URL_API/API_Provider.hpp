//
//  API_Provider.hpp
//  KanVest
//
//  Created by Ashish . on 08/01/26.
//

#pragma once

namespace KanVest
{
  enum class Range
  {
    _1D, _5D, _1MO, _6MO, _YTD, _1Y, _5Y, _MAX
  };
  
  enum class Interval
  {
    _1M, _2M, _5M, _15M, _30M, _60M, _90M, _1H, _1D, _5D, _1WK, _1MO, _3MO
  };
  
  enum class StockAPIProvider
  {
    Yahoo,
  };
  
  struct APIKeys
  {
    std::string price = "";
    std::string prevClose = "";
    std::string changePercent = "";
    std::string volume = "";
    std::string shortName = "";
    std::string longName = "";
    std::string instrumentType = "";
    std::string timezone = "";
    std::string range = "";
    std::string dataGranularity = "";
    std::string currency = "";
    std::string exchangeName = "";
    std::string fiftyTwoHigh = "";
    std::string fiftyTwoLow = "";
    std::string dayHigh = "";
    std::string dayLow = "";
  };
  
  /// This class stores the server URL from where data need to be extracted
  class API_Provider
  {
  public:
    /// This function initializes the API Provide for URL
    /// - Parameter apiProvider: API provider type
    static void Initialize(StockAPIProvider apiProvider);
    
  private:
    /// This function returns the API Keys
    static APIKeys GetAPIKeys();
    
    /// This function returns the URL based on API provider
    static std::string GetURL();
    
    /// This function retruns double data from patterns
    /// - Parameter key: key string
    static std::string GetValueParserPattern(const std::string& key);
    /// This function retruns string data from patterns
    /// - Parameter key: key string
    static std::string GetStringParserPattern(const std::string& key);
    /// This function retruns array data from patterns
    /// - Parameter key: key string
    static std::string GetArrayParserPattern(const std::string& key);
    
  private:
    inline static StockAPIProvider s_stockAPIProvider;
    inline static APIKeys s_apiKeys;
  };
} // namespace KanVest
