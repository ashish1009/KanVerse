//
//  APIProvider.hpp
//  KanVest
//
//  Created by Ashish . on 03/12/25.
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
    /// This function initializes the API Provide for URL
    /// - Parameter apiProvider: API provider type
    static void Initialize(StockAPIProvider apiProvider);
    
    /// This function returns the API Keys
    static APIKeys GetAPIKeys();

  private:
    inline static StockAPIProvider s_stockAPIProvider = StockAPIProvider::Yahoo;
    inline static APIKeys s_apiKeys;
  };
} // namespace KanVest
