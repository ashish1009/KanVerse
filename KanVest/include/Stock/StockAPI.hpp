//
//  StockAPI.hpp
//  KanVest
//
//  Created by Ashish . on 03/12/25.
//

#pragma once

#include "URL_API/APIProvider.hpp"

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
