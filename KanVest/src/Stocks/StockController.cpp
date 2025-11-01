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
  StockData StockController::UpdateStockData(const std::string& symbolName, const std::string& period1, const std::string& period2)
  {
    // Ensure symbol has a suffix (.NS, .BO, etc.)
    std::string symbol = symbolName;
    if (symbol.find('.') == std::string::npos)
    {
      symbol += ".NS";
    }

    APIKeys keys = API_Provider::GetAPIKeys();

    // Fetch data from URL ------------------------------------
    std::string liveData = StockAPI::FetchLiveData(symbol);
    
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
    
    // Fetc Historyh data from URL -----------------------------
    std::string historyData = StockAPI::FetchHistoryData(symbol, period1, period2);
    
    // Update Stock data
    StockData stockData(symbolName);
    
    stockData.shortName = StockParser::StockParser::ExtractString(liveData, keys.shortName);
    stockData.longName = StockParser::StockParser::ExtractString(liveData, keys.longName);
    stockData.currency = StockParser::StockParser::ExtractString(liveData, keys.currency);
    stockData.exchangeName = StockParser::StockParser::ExtractString(liveData, keys.exchangeName);

    stockData.livePrice = StockParser::ExtractValue(liveData, keys.price);
    stockData.prevClose = StockParser::ExtractValue(liveData, keys.prevClose);
    stockData.change = stockData.livePrice - stockData.prevClose;
    stockData.changePercent = StockParser::ExtractValue(liveData, keys.changePercent);
    
    if (stockData.changePercent == -1 && stockData.prevClose > 0)
    {
      stockData.changePercent = (stockData.change / stockData.prevClose) * 100.0;
    }
    stockData.volume = StockParser::ExtractValue(liveData, keys.volume);

    stockData.fiftyTwoHigh = StockParser::ExtractValue(liveData, keys.fiftyTwoHigh);
    stockData.fiftyTwoLow = StockParser::ExtractValue(liveData, keys.fiftyTwoLow);
    stockData.dayHigh = StockParser::ExtractValue(liveData, keys.dayHigh);
    stockData.dayLow = StockParser::ExtractValue(liveData, keys.dayLow);

    return stockData;
  }
} // namespace KanVest
