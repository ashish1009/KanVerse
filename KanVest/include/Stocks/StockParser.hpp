//
//  StockParser.hpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#pragma once

namespace KanVest
{
  /// This class parse the stock data from string to corresponding value
  class StockParser
  {
  public:
    /// This function extract the stock data as double value
    /// - Parameters:
    ///   - text: parsing text
    ///   - key: key parser
    static double ExtractValue(const std::string& text, const std::string& key);
  };
} // namespace KanVest
