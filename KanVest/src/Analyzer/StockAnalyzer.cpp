//
//  StockAnalyzer.cpp
//  KanVest
//
//  Created by Ashish . on 23/11/25.
//

#include "StockAnalyzer.hpp"

namespace KanVest
{
  const Recommendation& Analyzer::AnalzeStock(const StockData &stockData)
  {
    static Recommendation EmptyRecommendation;
    return EmptyRecommendation;
  }
  
  void Analyzer::SetHoldings(const Holding &holding)
  {
    s_stockHolding = holding;
  }
} // namespace KanVest
