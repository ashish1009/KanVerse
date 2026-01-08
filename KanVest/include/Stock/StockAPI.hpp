//
//  StockAPI.hpp
//  KanVest
//
//  Created by Ashish . on 08/01/26.
//

#pragma once

#include "URL_API/API_Provider.hpp"

namespace KanVest
{
  class StockAPI
  {
  public:
    /// This function fetch data from URL and return the string data to be parsed
    /// - Parameters:
    ///   - symbolName: Symbol name
    static std::string FetchLiveData(const std::string& symbolName, Range range, Interval interval);
  };
} // namespace KanVest
