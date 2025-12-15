//
//  StockAnalyzer.hpp
//  KanVest
//
//  Created by Ashish . on 14/12/25.
//

#pragma once

#include "Stock/StockData.hpp"

#include "Analyzer/Indicators/MovingAverage.hpp"

namespace KanVest
{
  struct StockReport
  {
  };

  class Analyzer
  {
  public:
    /// This function analyze stock from data
    /// - Parameter stockData: stock data
    static StockReport AnalzeStock(const StockData& stockData);
    
    static const std::map<int, double>& GetDMA();
    static const std::map<int, double>& GetEMA();

  private:
    inline static StockReport s_stockReport;
    inline static MAResult s_maResults;
  };
} // namespace KanVest
