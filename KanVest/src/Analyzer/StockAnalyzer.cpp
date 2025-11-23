//
//  StockAnalyzer.cpp
//  KanVest
//
//  Created by Ashish . on 23/11/25.
//

#include "StockAnalyzer.hpp"

#include "Analyzer/Indicator/MovingAverage.hpp"

namespace KanVest
{
  const Recommendation& Analyzer::AnalzeStock(const StockData &stockData)
  {
    static Recommendation EmptyRecommendation;
    
    // Moving averages
    MAResult maResults = MovingAverage::Compute(stockData);
    
    return EmptyRecommendation;
  }
  
  void Analyzer::SetHoldings(const Holding &holding)
  {
    s_stockHolding = holding;
  }
} // namespace KanVest
