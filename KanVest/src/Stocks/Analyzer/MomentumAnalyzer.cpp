//
//  MomentumAnalyzer.cpp
//  KanVest
//
//  Created by Ashish . on 10/11/25.
//

#include "MomentumAnalyzer.hpp"

namespace KanVest
{
  MomentumReport MomentumAnalyzer::Analyze(const StockData& stock, const TechnicalReport& techReport, const VolatilityReport& volReport)
  {
    MomentumReport report;
    
    // Use 5-day SMA for short-term, 50-day SMA for long-term
    double shortMA = techReport.SMA.count(5) ? techReport.SMA.at(5) : stock.prevClose;
    double longMA  = techReport.SMA.count(50) ? techReport.SMA.at(50) : stock.prevClose;
    
    // Short-term behavior
    report.shortTermBehavior = DetermineBehavior(shortMA, longMA, volReport.shortTermVolatility);
    
    // Long-term behavior
    report.longTermBehavior = DetermineBehavior(longMA, techReport.SMA.count(100) ? techReport.SMA.at(100) : stock.prevClose, volReport.longTermVolatility);
    
    // Human-readable explanation
    std::ostringstream oss;
    oss << "Short-term behavior is " << report.shortTermBehavior
    << " based on 5-day SMA vs 50-day SMA and current volatility.\n";
    oss << "Long-term behavior is " << report.longTermBehavior
    << " based on 50-day SMA vs 100-day SMA and long-term volatility.\n";
    
    report.explanation = oss.str();
    return report;
  }
  
  std::string MomentumAnalyzer::DetermineBehavior(double shortMA, double longMA, double volatility)
  {
    double diffPercent = (shortMA - longMA) / longMA * 100.0;
    
    if (diffPercent > 2.0 && volatility < 3.0)
      return "Very Positive";
    else if (diffPercent > 0.5)
      return "Positive";
    else if (diffPercent < -2.0 && volatility > 5.0)
      return "Very Negative";
    else if (diffPercent < -0.5)
      return "Negative";
    else
      return "Neutral";
  }
} // namespace KanVest
