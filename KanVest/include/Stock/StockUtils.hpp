//
//  StockUtils.hpp
//  KanVest
//
//  Created by Ashish . on 10/01/26.
//

#include "Stock/StockMetadata.hpp"

namespace KanVest::Utils
{
  /// This function normalize the stock symbol. Adds .NS in stock also convert Nifty as its original symbol
  /// - Parameter input: symbol data
  std::string NormalizeSymbol(const std::string& input);
  
  /// This function returns the Trading days data only for weekdays
  /// - Parameter history: candle history
  std::vector<CandleData> FilterTradingDays(const std::vector<CandleData>& history);
} // namespace KanVest
