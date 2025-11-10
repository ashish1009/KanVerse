//
//  ChartAnalyzer.cpp
//  KanVest
//
//  Created by Ashish . on 10/11/25.
//

#include "ChartAnalyzer.hpp"

namespace KanVest
{
  ChartReport ChartAnalyzer::Analyze(const StockData& stock)
  {
    ChartReport report;
    
    DetectSupportResistance(stock, report);
    DetectCandlestickPatterns(stock, report);
    DetectChartPatterns(stock, report);
    
    report.explanation = GenerateExplanation(report);
    return report;
  }
  
  // --------------------------
  // Support and Resistance
  // --------------------------
  void ChartAnalyzer::DetectSupportResistance(const StockData& stock, ChartReport& report)
  {
    if (stock.history.empty()) return;
    
    // Simple method: local minima/maxima
    for (size_t i = 1; i < stock.history.size() - 1; ++i)
    {
      double prev = stock.history[i - 1].close;
      double curr = stock.history[i].close;
      double next = stock.history[i + 1].close;
      
      // Support: local minimum
      if (curr < prev && curr < next) report.supportLevels.push_back(curr);
      
      // Resistance: local maximum
      if (curr > prev && curr > next) report.resistanceLevels.push_back(curr);
    }
    
    // Optional: keep unique and sorted
    std::sort(report.supportLevels.begin(), report.supportLevels.end());
    report.supportLevels.erase(std::unique(report.supportLevels.begin(), report.supportLevels.end()), report.supportLevels.end());
    
    std::sort(report.resistanceLevels.begin(), report.resistanceLevels.end(), std::greater<double>());
    report.resistanceLevels.erase(std::unique(report.resistanceLevels.begin(), report.resistanceLevels.end()), report.resistanceLevels.end());
  }
  
  // --------------------------
  // Simple Candlestick Patterns
  // --------------------------
  void ChartAnalyzer::DetectCandlestickPatterns(const StockData& stock, ChartReport& report)
  {
    for (const auto& point : stock.history)
    {
      double body = std::abs(point.close - point.open);
      double candleRange = point.high - point.low;
      
      if (body <= 0.1 * candleRange) report.candlestickPatterns.push_back("Doji");
      if ((point.close > point.open) && (point.low < point.open) && ((point.high - point.close) > 2 * body)) report.candlestickPatterns.push_back("Hammer");
      if ((point.close < point.open) && (point.high > point.open) && ((point.open - point.low) > 2 * body)) report.candlestickPatterns.push_back("Shooting Star");
    }
  }
  
  // --------------------------
  // Simple Chart Patterns
  // --------------------------
  void ChartAnalyzer::DetectChartPatterns(const StockData& stock, ChartReport& report)
  {
    // Placeholder: advanced pattern detection (head & shoulders, double top/bottom)
    // For now, detect simple uptrend/downtrend
    if (stock.history.size() < 2) return;
    
    double first = stock.history.front().close;
    double last  = stock.history.back().close;
    
    if (last > first) report.chartPatterns.push_back("Uptrend");
    else if (last < first) report.chartPatterns.push_back("Downtrend");
    else report.chartPatterns.push_back("Sideways");
  }
  
  // --------------------------
  // Generate Human-readable Explanation
  // --------------------------
  std::string ChartAnalyzer::GenerateExplanation(const ChartReport& report)
  {
    std::ostringstream oss;
    
    oss << "Support levels detected: ";
    for (double s : report.supportLevels) oss << s << " ";
    if (report.supportLevels.empty()) oss << "None";
    
    oss << "\nResistance levels detected: ";
    for (double r : report.resistanceLevels) oss << r << " ";
    if (report.resistanceLevels.empty()) oss << "None";
    
    oss << "\nCandlestick patterns observed: ";
    for (const auto& pattern : report.candlestickPatterns) oss << pattern << " ";
    if (report.candlestickPatterns.empty()) oss << "None";
    
    oss << "\nChart pattern: ";
    for (const auto& cp : report.chartPatterns) oss << cp << " ";
    if (report.chartPatterns.empty()) oss << "None";
    
    return oss.str();
  }
} // namespace KanVest
