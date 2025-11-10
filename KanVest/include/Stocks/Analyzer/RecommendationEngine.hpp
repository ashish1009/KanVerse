//
//  RecommendationEngine.hpp
//  KanVest
//
//  Created by Ashish . on 10/11/25.
//

#pragma once
#include "Stocks/StockData.hpp"
#include "Stocks/Analyzer/TechnicalAnalyzer.hpp"
#include "Stocks/Analyzer/MomentumAnalyzer.hpp"
#include "Stocks/Analyzer/VolatilityAnalyzer.hpp"
#include "Stocks/Analyzer/ChartAnalyzer.hpp"
#include "Stocks/Analyzer/PerformanceSummary.hpp"

namespace KanVest
{
  struct Recommendation
  {
    std::string action;   // Buy, Sell, Hold
    double quantity = 0.0;
    std::string explanation;  // Human-readable reasoning
  };
  
  class RecommendationEngine
  {
  public:
    /// Generate recommendation based on all analysis and current holdings
    static Recommendation Generate(
                                   const StockData& stock,
                                   const TechnicalReport& techReport,
                                   const MomentumReport& momentumReport,
                                   const VolatilityReport& volReport,
                                   const ChartReport& chartReport,
                                   const PerformanceReport& perfReport,
                                   double userHolding = 0.0
                                   );
    
  private:
    static double DetermineQuantity(double stockPrice, double userHolding, const std::string& action);
  };
} // namespace KanVest
