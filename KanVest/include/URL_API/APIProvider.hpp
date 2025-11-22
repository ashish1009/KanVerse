//
//  APIProvider.hpp
//  KanVest
//
//  Created by Ashish . on 22/11/25.
//

#pragma once

namespace KanVest
{
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
    static void Initialize(StockAPIProvider apiProvider);
    static std::string GetURL();
    static std::string GetValueParserPattern(const std::string& key);
    static std::string GetStringParserPattern(const std::string& key);
    static std::string GetArrayParserPattern(const std::string& key);
    static APIKeys GetAPIKeys();
    
  private:
    inline static StockAPIProvider s_stockAPIProvider = StockAPIProvider::Yahoo;
  };
} // namespace KanVest
