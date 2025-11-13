//
//  VolatilityAnalyzer.cpp
//  KanVest
//
//  Created by Ashish . on 10/11/25.
//

#include "VolatilityAnalyzer.hpp"

namespace KanVest
{
  VolatilityReport VolatilityAnalyzer::Analyze(const StockData& stock, int shortTermPeriod, int longTermPeriod)
  {
    IK_PERFORMANCE_FUNC("VolatilityAnalyzer::Analyze");
    VolatilityReport report;
    
    if (stock.history.size() < 2)
    {
      report.explanation = "Not enough historical data to compute volatility.";
      return report;
    }
    
    // Short-term volatility
    int shortStart = std::max(0, static_cast<int>(stock.history.size()) - shortTermPeriod);
    std::vector<StockPoint> shortHistory(stock.history.begin() + shortStart, stock.history.end());
    report.shortTermVolatility = ComputeStdDev(shortHistory);
    
    // Long-term volatility
    int longStart = std::max(0, static_cast<int>(stock.history.size()) - longTermPeriod);
    std::vector<StockPoint> longHistory(stock.history.begin() + longStart, stock.history.end());
    report.longTermVolatility = ComputeStdDev(longHistory);
    
    // Human-readable explanation
    report.explanation = GenerateExplanation(report.shortTermVolatility, report.longTermVolatility);
    
    return report;
  }
  
  double VolatilityAnalyzer::ComputeStdDev(const std::vector<StockPoint>& history)
  {
    IK_PERFORMANCE_FUNC("VolatilityAnalyzer::ComputeStdDev");
    if (history.size() < 2) return 0.0;
    
    // Compute daily returns
    std::vector<double> returns;
    for (size_t i = 1; i < history.size(); ++i)
    {
      double ret = (history[i].close - history[i-1].close) / history[i-1].close;
      returns.push_back(ret);
    }
    
    double mean = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    double variance = 0.0;
    for (double r : returns) variance += (r - mean) * (r - mean);
    variance /= returns.size();
    
    return std::sqrt(variance) * 100; // return % volatility
  }
  
  std::string VolatilityAnalyzer::GenerateExplanation(double shortVol, double longVol)
  {
    IK_PERFORMANCE_FUNC("VolatilityAnalyzer::GenerateExplanation");
    std::string desc = "Short-term volatility is " + std::to_string(shortVol) + "%, ";
    desc += "long-term volatility is " + std::to_string(longVol) + "%. ";
    
    if (shortVol > longVol)
      desc += "Volatility is increasing recently, indicating higher risk in the short-term.";
    else if (shortVol < longVol)
      desc += "Volatility is decreasing recently, indicating more stable price movement.";
    else
      desc += "Volatility is stable compared to long-term trends.";
    
    return desc;
  }
  
  namespace Utils
  {
    // Converts volatility (%) into descriptive string
    std::string VolatilityDescription(double volatilityPercent)
    {
      if (volatilityPercent < 0.5)
        return "Very Low Volatile";
      else if (volatilityPercent < 1.5)
        return "Low Volatile";
      else if (volatilityPercent < 3.0)
        return "Moderate Volatile";
      else if (volatilityPercent < 6.0)
        return "High Volatile";
      else
        return "Very High Volatile";
    }
    
    // Example usage for VolatilityReport
    std::string DescribeVolatility(const VolatilityReport& report)
    {
      std::ostringstream oss;
      oss << "Short-term volatility: "
      << VolatilityDescription(report.shortTermVolatility)
      << " (" << report.shortTermVolatility << "%), "
      << "Long-term volatility: "
      << VolatilityDescription(report.longTermVolatility)
      << " (" << report.longTermVolatility << "%)";
      return oss.str();
    }
  }
} // namespace KanVest
