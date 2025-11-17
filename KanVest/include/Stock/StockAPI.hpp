//
//  StockAPI.hpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
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
    static std::string FetchLiveData(const std::string& symbolName, const std::string& interval, const std::string& range);
  };
} // namespace KanVest
