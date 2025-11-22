//
//  StockManager.cpp
//  KanVest
//
//  Created by Ashish . on 22/11/25.
//

#include "StockManager.hpp"

namespace KanVest
{
  namespace Utils
  {
    static std::string NormalizeSymbol(const std::string& input)
    {
      std::string symbol = input;
      std::transform(symbol.begin(), symbol.end(), symbol.begin(),
                     [](unsigned char c) { return std::toupper(c); });
      
      if (symbol == "%5ENSEI") return symbol;
      if (symbol == "NIFTY") return "%5ENSEI";
      if (symbol.find('.') == std::string::npos)
        symbol += ".NS";
      return symbol;
    }
  } // namespace Utils
  
  // Stock operations
  bool StockManager::AddStock(const std::string& symbolName)
  {
    std::scoped_lock lock(s_mutex);
    std::string symbol = Utils::NormalizeSymbol(symbolName);
    if (std::find(s_symbols.begin(), s_symbols.end(), symbol) == s_symbols.end())
    {
      s_symbols.push_back(symbol);
      
      // ensure an empty placeholder for active cache to avoid UI checks
      s_activeCache.emplace(symbol, StockData(symbol));
      return true;
    }
    return false;
  }

  bool StockManager::EditStock(const std::string& symbol)
  {
    return AddStock(symbol);
  }

  void StockManager::RemoveStock(const std::string& symbolName)
  {
    std::scoped_lock lock(s_mutex);
    std::string symbol = Utils::NormalizeSymbol(symbolName);
    s_symbols.erase(std::remove(s_symbols.begin(), s_symbols.end(), symbol), s_symbols.end());
    s_activeCache.erase(symbol);
    
    // remove all cached variants for this symbol
    for (auto it = s_stockDataCache.begin(); it != s_stockDataCache.end(); )
    {
      if (it->first.symbol == symbol) it = s_stockDataCache.erase(it); else ++it;
    }
  }
  
  bool StockManager::GetStockData(const std::string& symbolName, StockData& outData)
  {
    std::scoped_lock lock(s_mutex);
    std::string symbol = Utils::NormalizeSymbol(symbolName);
    auto it = s_activeCache.find(symbol);
    if (it != s_activeCache.end())
    {
      outData = it->second;
      return true;
    }
    return false;
  }

} // namespace KanVest
