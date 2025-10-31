//
//  StockController.cpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#include "StockController.hpp"

#include "Stocks/StockAPI.hpp"

namespace KanVest
{
  StockData StockController::UpdateStockData(const std::string& symbolName)
  {
    // Ensure symbol has a suffix (.NS, .BO, etc.)
    std::string symbol = symbolName;
    if (symbol.find('.') == std::string::npos) {
      symbol += ".NS"; // default to NSE
    }

    // Fetch data from URL
    std::string liveURL = "https://query1.finance.yahoo.com/v8/finance/chart/" + symbol;
    std::string liveData = StockAPI::FetchURL(liveURL);
    
    // If liveData doesn't contain usual fields, try .BO fallback for Indian stocks
    if (liveData.find("\"regularMarketPrice\"") == std::string::npos and symbolName.find(".NS") != std::string::npos)
    {
      std::string altSymbol = symbol.substr(0, symbol.find(".NS")) + ".BO";
      std::string altData = StockAPI::FetchURL("https://query1.finance.yahoo.com/v8/finance/chart/" + altSymbol);

      if (altData.find("\"regularMarketPrice\"") != std::string::npos)
      {
        symbol = altSymbol;
        liveData = altData;
      }
    }

    // Update Stock data
    StockData stockData(symbol);
    
    stockData.livePrice = extractValue(liveData, "regularMarketPrice");
    
    return stockData;
  }
} // namespace KanVest
