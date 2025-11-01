//
//  StockAPI.hpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#pragma once

namespace KanVest
{
  class StockAPI
  {
  public:
    /// This function fetch data from URL and return the string data to be parsed
    /// - Parameters:
    ///   - symbolName: Symbol name
    static std::string FetchLiveData(const std::string& symbolName);
    /// This function fetch data from URL and return the string data to be parsed
    /// - Parameters:
    ///   - symbolName: Symbol name
    ///   - period1: period 1
    ///   - period2: period 2
    static std::string FetchHistoryData(const std::string& symbolName, const std::string& period1, const std::string& period2);
  };
} // namespace KanVest
