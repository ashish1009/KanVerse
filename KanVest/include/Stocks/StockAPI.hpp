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
    /// - Parameter url: URL
    static std::string FetchURL(const std::string& url);
  };
} // namespace KanVest
