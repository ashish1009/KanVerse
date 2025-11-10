//
//  PerformanceAnalyzer.cpp
//  KanVest
//
//  Created by Ashish . on 10/11/25.
//

#include "PerformanceAnalyzer.hpp"

namespace KanVest
{
  PerformanceReport PerformanceSummary::Analyze(const StockData& stock, double sectorChangePercent)
  {
    PerformanceReport report;
    
    if (stock.prevClose <= 0)
    {
      report.explanation = "Previous close not available, cannot compute daily performance.";
      return report;
    }
    
    // Daily % change
    report.dailyChangePercent = (stock.livePrice - stock.prevClose) / stock.prevClose * 100.0;
    
    // Relative performance vs sector
    report.relativeToSector = report.dailyChangePercent - sectorChangePercent;
    
    // Human-readable explanation
    std::ostringstream oss;
    oss << "Stock " << stock.symbol << " moved " << report.dailyChangePercent << "% today. ";
    
    if (report.relativeToSector > 0)
      oss << "It outperformed the sector by " << report.relativeToSector << "%.";
    else if (report.relativeToSector < 0)
      oss << "It underperformed the sector by " << std::abs(report.relativeToSector) << "%.";
    else
      oss << "It performed in line with the sector.";
    
    report.explanation = oss.str();
    return report;
  }
} // namespace KanVest
