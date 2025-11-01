//
//  StockController.hpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#pragma once

#include "Stocks/StockData.hpp"

namespace KanVest
{          
  static std::vector<double> extractArray(const std::string& text, const std::string& key) {
    std::vector<double> values;
    std::string patternStr = "\"" + key + "\"\\s*:\\s*\\[([^\\]]+)\\]";
    std::regex pattern(patternStr);
    std::smatch match;
    if (std::regex_search(text, match, pattern)) {
      std::string arr = match[1].str();
      std::regex numPattern("([-+]?[0-9]*\\.?[0-9]+)");
      for (std::sregex_iterator it(arr.begin(), arr.end(), numPattern), end; it != end; ++it)
        values.push_back(std::stod((*it)[1].str()));
    }
    return values;
  }
  
  /// This class manages stock data updation
  class StockController
  {
  public:
    /// This function fetches real time data for stock and returns the stock data
    /// - Parameter symbol: symbolName
    static StockData UpdateStockData(const std::string& symbolName, const std::string& period1, const std::string& period2);
  };
} // namespace KanVest
