//
//  RecommendationEngine.cpp
//  KanVest
//
//  Created by Ashish . on 10/11/25.
//

#include "RecommendationEngine.hpp"

namespace KanVest
{
  Recommendation RecommendationEngine::Generate(
                                                const StockData& stock,
                                                const TechnicalReport& techReport,
                                                const MomentumReport& momentumReport,
                                                const VolatilityReport& volReport,
                                                const ChartReport& chartReport,
                                                const PerformanceReport& perfReport,
                                                double userHolding
                                                )
  {
    Recommendation rec;
    
    // ---------- Determine Action ----------
    bool bullishMomentum = momentumReport.shortTermBehavior == "Very Positive" ||
    momentumReport.shortTermBehavior == "Positive";
    bool bearishMomentum = momentumReport.shortTermBehavior == "Very Negative" ||
    momentumReport.shortTermBehavior == "Negative";
    
    bool priceAboveSMA50 = stock.livePrice > (techReport.SMA.count(50) ? techReport.SMA.at(50) : stock.prevClose);
    bool oversold = techReport.RSI < 30;
    bool overbought = techReport.RSI > 70;
    
    // Decision rules
    if (bullishMomentum && priceAboveSMA50 && !overbought)
      rec.action = "Buy";
    else if (bearishMomentum || overbought)
      rec.action = (userHolding > 0) ? "Sell" : "Hold";
    else
      rec.action = "Hold";
    
    // ---------- Determine Quantity ----------
    rec.quantity = DetermineQuantity(stock.livePrice, userHolding, rec.action);
    
    // ---------- Human-readable Explanation ----------
    std::ostringstream oss;
    oss << "Recommendation: " << rec.action << "\n";
    oss << "Quantity suggested: " << rec.quantity << "\n";
    oss << "Reasoning:\n";
    
    // Momentum explanation
    oss << "- Momentum: Short-term trend is " << momentumReport.shortTermBehavior
    << ", long-term trend is " << momentumReport.longTermBehavior << ".\n";
    
    // Technicals
    oss << "- Technicals: RSI = " << techReport.RSI
    << ", MACD = " << techReport.MACD << " (Signal = " << techReport.MACDSignal << ")"
    << ", SMA50 = " << (techReport.SMA.count(50) ? techReport.SMA.at(50) : 0.0) << ".\n";
    
    // Volatility
    oss << "- Volatility: Short-term = " << volReport.shortTermVolatility
    << "%, Long-term = " << volReport.longTermVolatility << "%.\n";
    
    // Daily performance
    oss << "- Daily performance: " << perfReport.dailyChangePercent
    << "%, relative to sector: " << perfReport.relativeToSector << "%.\n";
    
    // Chart patterns
    oss << "- Chart: Support levels ";
    for (double s : chartReport.supportLevels) oss << s << " ";
    if (chartReport.supportLevels.empty()) oss << "None";
    
    oss << ", Resistance levels ";
    for (double r : chartReport.resistanceLevels) oss << r << " ";
    if (chartReport.resistanceLevels.empty()) oss << "None";
    
    oss << ", Candlestick patterns: ";
    for (const auto& p : chartReport.candlestickPatterns) oss << p << " ";
    if (chartReport.candlestickPatterns.empty()) oss << "None";
    
    oss << ", Chart patterns: ";
    for (const auto& p : chartReport.chartPatterns) oss << p << " ";
    if (chartReport.chartPatterns.empty()) oss << "None";
    
    oss << ".\n";
    
    rec.explanation = oss.str();
    return rec;
  }
  
  // --------------------------
  // Determine quantity to trade
  // --------------------------
  double RecommendationEngine::DetermineQuantity(double stockPrice, double userHolding, const std::string& action)
  {
    if (action == "Buy")
    {
      // Example: Buy fixed 100 shares if not already holding
      return std::max(1.0, 100.0);
    }
    else if (action == "Sell")
    {
      // Sell 50% of current holding
      return userHolding * 0.5;
    }
    return 0.0; // Hold
  }
} // namespace KanVest
