//
//  MomentumAnalyzer.hpp
//  KanVest
//
//  Created by Ashish . on 10/11/25.
//

#pragma once
#include "Stocks/StockData.hpp"
#include "Stocks/Analyzer/TechnicalAnalyzer.hpp"
#include "Stocks/Analyzer/VolatilityAnalyzer.hpp"

namespace KanVest
{
  enum class Momentum {VeryPositive, Positive, Neutral, Negative, VeryNegative};
  struct MomentumReport
  {
    Momentum shortTermBehavior; // Very Positive, Positive, Neutral, Negative
    Momentum longTermBehavior;
    std::string explanation;       // Human-readable summary
  };
  
  class MomentumAnalyzer
  {
  public:
    /// Analyze momentum using technicals + volatility
    static MomentumReport Analyze(const StockData& stock, const TechnicalReport& techReport, const VolatilityReport& volReport);
  private:
    static Momentum DetermineBehavior(double shortMA, double longMA, double volatility);
  };
  
  namespace Utils
  {
    std::string GetMomentumString(Momentum momentum);
  }
} // namespace KanVest
