//
//  StockAnalyzer.cpp
//  KanVest
//
//  Created by Ashish . on 14/12/25.
//

#include "StockAnalyzer.hpp"

namespace KanVest
{
  StockReport Analyzer::AnalzeStock(const StockData& stockData)
  {
    // Technical data
    s_maResults = MovingAverage::Compute(stockData);
    
    return s_stockReport;
  }
  
  const std::map<int, double>& Analyzer::GetDMA()
  {
    return s_maResults.dmaValues;
  }
  const std::map<int, double>& Analyzer::GetEMA()
  {
    return s_maResults.emaValues;
  }
} // namespace KanVest
