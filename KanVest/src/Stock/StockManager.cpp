//
//  StockManager.cpp
//  KanVest
//
//  Created by Ashish . on 02/12/25.
//

#include "StockManager.hpp"

#include "Stock/StockUtils.hpp"

namespace KanVest
{
  StockData StockManager::GetStockData(const std::string& stockSymbolName, const std::string& range, const std::string& interval)
  {
    // Normalize the symbol. Add required .NS to Fetch data
    std::string symbol = Utils::NormalizeSymbol(stockSymbolName);

    auto apiKeys = API_Provider::GetAPIKeys();

    return StockData();
  }
}
