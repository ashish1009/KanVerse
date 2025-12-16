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
  
  const std::map<int, std::vector<double>>& Analyzer::GetDMAValues()
  {
    return s_maResults.dmaValues;
  }
  const std::map<int, std::vector<double>>& Analyzer::GetEMAValues()
  {
    return s_maResults.emaValues;
  }
} // namespace KanVest
