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
                                                const UserHoldingForAnalyzer& userHolding
                                                )
  {
    Recommendation rec;
    
    // ---------- Compute Unrealized P/L ----------
    double unrealizedPLPercent = 0.0;
    if (userHolding.quantity > 0)
      unrealizedPLPercent = (stock.livePrice - userHolding.avgPrice) / userHolding.avgPrice * 100.0;
    
    // ---------- Determine Action ----------
    bool bullishMomentum = momentumReport.shortTermBehavior == "Very Positive" ||
    momentumReport.shortTermBehavior == "Positive";
    bool bearishMomentum = momentumReport.shortTermBehavior == "Very Negative" ||
    momentumReport.shortTermBehavior == "Negative";
    
    bool priceAboveSMA50 = stock.livePrice > (techReport.SMA.count(50) ? techReport.SMA.at(50) : stock.prevClose);
    bool oversold = techReport.RSI < 30;
    bool overbought = techReport.RSI > 70;
        
    // ---------- Decision Logic ----------
    Action actionLevel;
    
    if (userHolding.quantity > 0)
    {
      // Already holding shares
      if ((bearishMomentum && overbought) || unrealizedPLPercent > 15.0)
        actionLevel = Action::StrongSell;
      else if (bearishMomentum || overbought || !priceAboveSMA50)  // <- use priceAboveSMA50 here
        actionLevel = Action::Sell;
      else if (bullishMomentum && (oversold || !priceAboveSMA50) && unrealizedPLPercent < -5.0)
        actionLevel = Action::StrongBuy;
      else if (bullishMomentum || priceAboveSMA50)  // <- use priceAboveSMA50 here
        actionLevel = Action::Buy;
      else
        actionLevel = Action::Hold;
    }
    else
    {
      // No holdings
      if (bullishMomentum && (oversold || !priceAboveSMA50))
        actionLevel = Action::StrongBuy;
      else if (bullishMomentum || priceAboveSMA50)
        actionLevel = Action::Buy;
      else if (bearishMomentum || !priceAboveSMA50)
        actionLevel = Action::StrongSell;
      else
        actionLevel = Action::Hold;
    }

    rec.action = actionLevel;

    // ---------- Determine Quantity ----------
    rec.quantity = DetermineQuantity(stock.livePrice, userHolding, rec.action);
    
    // ---------- Human-readable Explanation ----------
    std::ostringstream oss;
    oss << "Recommendation: " << Utils::GetActionString(rec.action) << "\n";
    oss << "Quantity suggested: " << rec.quantity << "\n";
    oss << "Reasoning:\n";
    
    // Unrealized P/L
    if (userHolding.quantity > 0)
      oss << "- Unrealized P/L: " << unrealizedPLPercent << "% on " << userHolding.quantity << " shares at avg price "
      << userHolding.avgPrice << "\n";
    
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
  double RecommendationEngine::DetermineQuantity(double stockPrice, const UserHoldingForAnalyzer& userHolding, Action action)
  {
    if (action == Action::Buy or action == Action::StrongBuy)
    {
      // Buy fixed 100 shares or 50% more than current if already holding
      if (userHolding.quantity > 0)
        return std::max(1.0, userHolding.quantity * 0.5);
      else
        return 100.0;
    }
    else if (action == Action::Sell or action == Action::StrongSell)
    {
      // Sell 50% of holding
      return userHolding.quantity * 0.5;
    }
    return 0.0; // Hold
  }

  namespace Utils
  {
    std::string GetActionString(Action action)
    {
      switch (action) {
        case Action::StrongBuy:   return "Strong Buy";
        case Action::Buy:         return "Buy";
        case Action::Hold:        return "Hold";
        case Action::Sell:        return "Sell";
        case Action::StrongSell:  return "Strong Sell";
        default:
          break;
      }
    }
  }
} // namespace KanVest
