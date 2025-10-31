//
//  StockController.cpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#include "StockController.hpp"

#include "Stocks/StockAPI.hpp"
#include "Stocks/StockParser.hpp"

#include "Stocks/API_Provider.hpp"

namespace KanVest
{
  StockData StockController::UpdateStockData(const std::string& symbolName)
  {
    // Ensure symbol has a suffix (.NS, .BO, etc.)
    std::string symbol = symbolName;
    if (symbol.find('.') == std::string::npos)
    {
      symbol += ".NS";
    }

    // Fetch data from URL
    std::string liveData = StockAPI::FetchLiveData(symbol);
    APIKeys keys = API_Provider::GetAPIKeys();

    // If liveData doesn't contain usual fields, try .BO fallback for Indian stocks
    if (liveData.find("\"" + keys.price + "\"") == std::string::npos and symbolName.find(".NS") != std::string::npos)
    {
      std::string altSymbol = symbol.substr(0, symbol.find(".NS")) + ".BO";
      std::string altData = StockAPI::FetchLiveData(altSymbol);

      if (altData.find("\"" + keys.price + "\"") != std::string::npos)
      {
        symbol = altSymbol;
        liveData = altData;
      }
    }

    // Update Stock data
    StockData stockData(symbolName);
    stockData.livePrice = StockParser::ExtractValue(liveData, keys.price);
    
    return stockData;
  }
} // namespace KanVest
