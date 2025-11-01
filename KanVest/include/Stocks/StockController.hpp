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
  static time_t parseDateYYYYMMDD(const std::string &s) {
    // Accepts "YYYY-MM-DD", returns time_t for 00:00:00 local time on that date
    std::tm tm = {};
    if (s.size() != 10) return 0;
    std::istringstream iss(s);
    iss >> std::get_time(&tm, "%Y-%m-%d");
    if (iss.fail()) return 0;
    tm.tm_hour = 0; tm.tm_min = 0; tm.tm_sec = 0;
    // mktime assumes localtime. Yahoo wants UTC timestamps, but using local is usually fine for daily ranges.
    return mktime(&tm);
  }
          
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
    static StockData UpdateStockData(const std::string& symbolName);
  };
} // namespace KanVest
