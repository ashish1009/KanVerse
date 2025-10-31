//
//  API_Provider.hpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
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
    std::string price;
  };
  
  /// This class stores the server URL from where data need to be extracted
  class API_Provider
  {
  public:
    static void Initialize(StockAPIProvider apiProvider);
    static std::string GetURL();
    static std::string GetValueParserPattern(const std::string& key);
    static APIKeys GetAPIKeys();
    
  private:
    inline static StockAPIProvider s_stockAPIProvider = StockAPIProvider::Yahoo;
  };
} // namespace KanVest
