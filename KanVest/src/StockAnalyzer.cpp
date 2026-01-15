//
//  StockAnalyzer.cpp
//  KanVest
//
//  Created by Ashish . on 14/01/26.
//

#include "StockAnalyzer.hpp"

namespace KanVest
{
  void Analyzer::AnalzeStock(const StockData& stockData)
  {
    // Reset report data
    s_stockReport.score = 50.0f;
    s_stockReport.summary.clear();
    
    // Technical data
    s_maResults = MovingAverage::Compute(stockData);
  }
  
  const StockReport& Analyzer::GetReport()
  {
    return s_stockReport;
  }
  const std::map<int, std::vector<double>>& Analyzer::GetDMAValues(MAPriceSource priceSource)
  {
    static std::map<int, std::vector<double>> EmptyMA;
    if (auto itr = s_maResults.find(priceSource); itr != s_maResults.end())
    {
      return itr->second.dmaValues;
    }
    return EmptyMA;
  }
  const std::map<int, std::vector<double>>& Analyzer::GetEMAValues(MAPriceSource priceSource)
  {
    static std::map<int, std::vector<double>> EmptyMA;
    if (auto itr = s_maResults.find(priceSource); itr != s_maResults.end())
    {
      return itr->second.emaValues;
    }
    return EmptyMA;
  }
} // namespace KanVest
