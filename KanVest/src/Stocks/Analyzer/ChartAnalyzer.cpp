//
//  ChartAnalyzer.cpp
//  KanVest
//
//  Created by Ashish . on 10/11/25.
//

#include "ChartAnalyzer.hpp"

namespace KanVest
{
//  ChartReport ChartAnalyzer::Analyze(const StockData& stock)
//  {
//    IK_PERFORMANCE_FUNC("ChartAnalyzer::Analyze");
//    ChartReport report;
//    
//    DetectSupportResistance(stock, report);
//    DetectCandlestickPatterns(stock, report);
//    DetectChartPatterns(stock, report);
//    
//    report.explanation = GenerateExplanation(report);
//    return report;
//  }
//  
//  // --------------------------
//  // Support and Resistance
//  // --------------------------
//  void ChartAnalyzer::DetectSupportResistance(const StockData& stock, ChartReport& report)
//  {
//    IK_PERFORMANCE_FUNC("ChartAnalyzer::DetectSupportResistance");
//    if (stock.history.size() < 3) return;
//    
//    struct Level {
//      double price;
//      int touches;
//    };
//    
//    std::vector<Level> supports;
//    std::vector<Level> resistances;
//    
//    double tol = 0.02; // 2% tolerance for clustering
//    
//    // 1. Find local minima/maxima
//    for (size_t i = 1; i < stock.history.size() - 1; ++i)
//    {
//      double prev = stock.history[i - 1].close;
//      double curr = stock.history[i].close;
//      double next = stock.history[i + 1].close;
//      
//      // Local min -> support
//      if (curr < prev && curr < next)
//      {
//        bool merged = false;
//        for (auto& s : supports)
//        {
//          if (std::abs(curr - s.price)/s.price < tol)
//          {
//            s.price = (s.price + curr)/2.0;
//            s.touches++;
//            merged = true;
//            break;
//          }
//        }
//        if (!merged) supports.push_back({curr, 1});
//      }
//      
//      // Local max -> resistance
//      if (curr > prev && curr > next)
//      {
//        bool merged = false;
//        for (auto& r : resistances)
//        {
//          if (std::abs(curr - r.price)/r.price < tol)
//          {
//            r.price = (r.price + curr)/2.0;
//            r.touches++;
//            merged = true;
//            break;
//          }
//        }
//        if (!merged) resistances.push_back({curr, 1});
//      }
//    }
//    
//    // 2. Sort by number of touches (strongest first)
//    auto cmpTouches = [](Level a, Level b){ return a.touches > b.touches; };
//    std::sort(supports.begin(), supports.end(), cmpTouches);
//    std::sort(resistances.begin(), resistances.end(), cmpTouches);
//    
//    // 3. Keep top 2-3 strongest levels
//    if (supports.size() > 3) supports.resize(3);
//    if (resistances.size() > 3) resistances.resize(3);
//    
//    // 4. Save results to report
//    report.supportLevels.clear();
//    report.resistanceLevels.clear();
//    
//    for (auto& s : supports) report.supportLevels.push_back(s.price);
//    for (auto& r : resistances) report.resistanceLevels.push_back(r.price);
//    
//    // Sort ascending support, descending resistance
//    std::sort(report.supportLevels.begin(), report.supportLevels.end());
//    std::sort(report.resistanceLevels.begin(), report.resistanceLevels.end(), std::greater<double>());
//  }
//  
//  // --------------------------
//  // Simple Candlestick Patterns
//  // --------------------------
//  void ChartAnalyzer::DetectCandlestickPatterns(const StockData& stock, ChartReport& report)
//  {
//    IK_PERFORMANCE_FUNC("ChartAnalyzer::DetectCandlestickPatterns");
//    for (const auto& point : stock.history)
//    {
//      double body = std::abs(point.close - point.open);
//      double candleRange = point.high - point.low;
//      
//      if (body <= 0.1 * candleRange) report.candlestickPatterns.push_back("Doji");
//      if ((point.close > point.open) && (point.low < point.open) && ((point.high - point.close) > 2 * body)) report.candlestickPatterns.push_back("Hammer");
//      if ((point.close < point.open) && (point.high > point.open) && ((point.open - point.low) > 2 * body)) report.candlestickPatterns.push_back("Shooting Star");
//    }
//  }
//  
//  // --------------------------
//  // Simple Chart Patterns
//  // --------------------------
//  void ChartAnalyzer::DetectChartPatterns(const StockData& stock, ChartReport& report)
//  {
//    IK_PERFORMANCE_FUNC("ChartAnalyzer::DetectChartPatterns");
//    // Placeholder: advanced pattern detection (head & shoulders, double top/bottom)
//    // For now, detect simple uptrend/downtrend
//    if (stock.history.size() < 2) return;
//    
//    double first = stock.history.front().close;
//    double last  = stock.history.back().close;
//    
//    if (last > first) report.chartPatterns.push_back("Uptrend");
//    else if (last < first) report.chartPatterns.push_back("Downtrend");
//    else report.chartPatterns.push_back("Sideways");
//  }
//  
//  // --------------------------
//  // Generate Human-readable Explanation
//  // --------------------------
//  std::string ChartAnalyzer::GenerateExplanation(const ChartReport& report)
//  {
//    IK_PERFORMANCE_FUNC("ChartAnalyzer::GenerateExplanation");
//    std::ostringstream oss;
//    
//    oss << "Support levels detected: ";
//    for (double s : report.supportLevels) oss << s << " ";
//    if (report.supportLevels.empty()) oss << "None";
//    
//    oss << "\nResistance levels detected: ";
//    for (double r : report.resistanceLevels) oss << r << " ";
//    if (report.resistanceLevels.empty()) oss << "None";
//    
//    oss << "\nCandlestick patterns observed: ";
//    for (const auto& pattern : report.candlestickPatterns) oss << pattern << " ";
//    if (report.candlestickPatterns.empty()) oss << "None";
//    
//    oss << "\nChart pattern: ";
//    for (const auto& cp : report.chartPatterns) oss << cp << " ";
//    if (report.chartPatterns.empty()) oss << "None";
//    
//    return oss.str();
//  }
} // namespace KanVest
