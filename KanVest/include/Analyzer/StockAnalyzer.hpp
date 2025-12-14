//
//  StockAnalyzer.hpp
//  KanVest
//
//  Created by Ashish . on 14/12/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  struct Recommendation
  {
  };

  class Analyzer
  {
  public:
    /// This function analyze stock from data
    /// - Parameter stockData: stock data
    static Recommendation AnalzeStock(const StockData& stockData);
  };
} // namespace KanVest
