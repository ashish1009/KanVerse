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
#include "Stocks/Analyzer/PerformanceAnalyzer.hpp"

namespace KanVest
{
  struct UserHoldingForAnalyzer
  {
    double quantity = 0.0;       // Number of shares
    double avgPrice = 0.0;       // Average buy price
  };
  
  enum class Action {StrongBuy, Buy, Hold, Sell, StrongSell};

  struct Recommendation
  {
    Action action;
    double quantity = 0.0;
    double score = 50.0;
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
                                   const UserHoldingForAnalyzer& userHolding
                                   );
    
  private:
    static double DetermineQuantity(double stockPrice, const UserHoldingForAnalyzer& userHolding, Action action);
  };
  
  namespace Utils
  {
    std::string GetActionString(Action action);
  }
} // namespace KanVest
