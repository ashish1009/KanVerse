//
//  StockManager.cpp
//  KanVest
//
//  Created by Ashish . on 02/12/25.
//

#include "StockManager.hpp"

#include "Stock/StockUtils.hpp"

#include "Stock/StockAPI.hpp"

namespace KanVest
{
  StockData StockManager::GetStockData(const std::string& stockSymbolName, Range range, Interval interval)
  {
    // Normalize the symbol. Add required .NS to Fetch data
    std::string symbol = Utils::NormalizeSymbol(stockSymbolName);

    // Get API Keys to extract data
    APIKeys apiKeys = API_Provider::GetAPIKeys();

    // Fetch Data
    std::string response = FetchStockFallbackData(symbol, range, interval, apiKeys);

    return StockData();
  }
  
  std::string StockManager::FetchStockFallbackData(const std::string& symbol, Range range, Interval interval, const APIKeys& keys)
  {
    std::string data = StockAPI::FetchLiveData(symbol, range, interval);
    
    if (data.find("\"" + keys.price + "\"") == std::string::npos &&
        symbol.find(".NS") != std::string::npos)
    {
      std::string altSymbol = symbol.substr(0, symbol.find(".NS")) + ".BO";
      std::string altData = StockAPI::FetchLiveData(altSymbol, range, interval);
      if (altData.find("\"" + keys.price + "\"") != std::string::npos)
        return altData;
    }
    
    return data;
  }

}
