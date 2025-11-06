//
//  StockParser.hpp
//  KanVest
//
//  Created by Ashish . on 06/11/25.
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
    /// This function extract the stock data as double value
    /// - Parameters:
    ///   - text: parsing text
    ///   - key: key parser
    static std::string ExtractString(const std::string& text, const std::string& key);
    /// This extract arrya
    /// - Parameters:
    ///   - text: text key
    ///   - key: key
    static std::vector<double> ExtractArray(const std::string& text, const std::string& key);
    /// This function parse time as tring into time_t
    /// - Parameter timeString: time string
    static time_t ParseDateYYYYMMDD(const std::string &timeString);
  };
} // namespace KanVest
