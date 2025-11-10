//
//  RecommendationEngine.cpp
//  KanVest
//
//  Created by Ashish . on 10/11/25.
//

#include "RecommendationEngine.hpp"

namespace KanVest
{
  std::unordered_map<std::string, int> candlestickScoreMap = {
    {"BullishEngulfing", 2},
    {"Hammer", 2},
    {"MorningStar", 3},
    {"PiercingLine", 2},
    {"BearishEngulfing", -2},
    {"ShootingStar", -2},
    {"EveningStar", -3},
    {"DarkCloudCover", -2},
    {"Doji", 0}, // neutral
    {"SpinningTop", 0} // neutral
    // Add more as needed
  };
  
  std::unordered_map<std::string, int> chartPatternScoreMap = {
    {"HeadAndShoulders", -3},
    {"InverseHeadAndShoulders", 3},
    {"DoubleTop", -2},
    {"DoubleBottom", 2},
    {"TriangleAscending", 2},
    {"TriangleDescending", -2},
    {"Flag", 1},
    {"Pennant", 1}
    // Add more patterns here
  };
  
  Recommendation RecommendationEngine::Generate(
                                                const StockData& stock,
                                                const TechnicalReport& techReport,
                                                const MomentumReport& momentumReport,
                                                const VolatilityReport& volReport,
                                                const ChartReport& chartReport,
                                                const PerformanceReport& perfReport,
                                                const UserHoldingForAnalyzer& userHolding)
  {
    Recommendation rec;
    
    // ---------- Compute Unrealized P/L ----------
    double unrealizedPLPercent = 0.0;
    if (userHolding.quantity > 0)
      unrealizedPLPercent = (stock.livePrice - userHolding.avgPrice) / userHolding.avgPrice * 100.0;
    
    // ---------- Compute interval price change ----------
    double intervalChangePercent = 0.0;
    if (!stock.history.empty())
    {
      const auto& start = stock.history.front();
      const auto& end = stock.history.back();
      intervalChangePercent = (end.close - start.close) / start.close * 100.0;
    }
    
    // Momentum (0-10)
    if (momentumReport.shortTermBehavior == Momentum::VeryPositive) rec.score += 10;
    else if (momentumReport.shortTermBehavior == Momentum::Positive) rec.score += 5;
    else if (momentumReport.shortTermBehavior == Momentum::Negative) rec.score -= 5;
    else if (momentumReport.shortTermBehavior == Momentum::VeryNegative) rec.score -= 10;
    
    // RSI (0-10)
    if (techReport.RSI < 30) rec.score += 7;    // oversold => bullish
    else if (techReport.RSI > 70) rec.score -= 7; // overbought => bearish
    
    // SMA50 (0-5)
    if (techReport.SMA.count(50) && stock.livePrice > techReport.SMA.at(50)) rec.score += 5;
    else rec.score -= 5;
    
    if (techReport.AwesomeOscillator > 0) rec.score += 3; else rec.score -= 3;
    if (techReport.StochasticRSI < 20) rec.score += 5; else if (techReport.StochasticRSI > 80) rec.score -= 5;
    if (techReport.CCI > 100) rec.score += 3; else if (techReport.CCI < -100) rec.score -= 3;

    // Cap score based on interval change
    if (intervalChangePercent <= -10.0)       // huge drop
      rec.score = std::min(rec.score, 40.0);          // max 40%, cannot Strong Buy
    else if (intervalChangePercent <= -5.0)   // moderate drop
      rec.score = std::min(rec.score, 50.0);          // max 50%, cannot be Strong Buy
    else if (intervalChangePercent >= 10.0)   // huge rise
      rec.score = std::max(rec.score, 80.0);          // minimum Strong Buy

    // Volatility adjustment (0-5)
    // Higher short-term volatility reduces confidence
    rec.score -= volReport.shortTermVolatility / 10.0;
    
    // Candlestick patterns (optional, 0-5)
    double patternScore = 0.0;
    for (const auto& pattern : chartReport.candlestickPatterns)
    {
      if (candlestickScoreMap.count(pattern))
        patternScore += candlestickScoreMap[pattern];
    }
    
    // Normalize to maximum ±5 points regardless of number of candles
    patternScore /= std::max(1.0, static_cast<double>(chartReport.candlestickPatterns.size()));
    patternScore = std::clamp(patternScore, -5.0, 5.0);
    
    rec.score += patternScore;

    double chartScore = 0.0;
    for (const auto& pattern : chartReport.chartPatterns)
    {
      if (chartPatternScoreMap.count(pattern))
        chartScore += candlestickScoreMap[pattern];
    }
    
    // Normalize to maximum ±5 points regardless of number of candles
    patternScore /= std::max(1.0, static_cast<double>(chartReport.candlestickPatterns.size()));
    patternScore = std::clamp(patternScore, -5.0, 5.0);
    
    rec.score += chartScore;
    
    // Clip rec.score to 0-100%
    rec.score = std::clamp(rec.score, 0.0, 100.0);
    
    // ---------- Map rec.score to Action ----------
    Action actionLevel;
    if (rec.score >= 70) actionLevel = Action::StrongBuy;
    else if (rec.score >= 55) actionLevel = Action::Buy;
    else if (rec.score <= 30) actionLevel = Action::StrongSell;
    else if (rec.score <= 45) actionLevel = Action::Sell;
    else actionLevel = Action::Hold;
    
    rec.action = actionLevel;
    
    // ---------- Determine Quantity ----------
    rec.quantity = DetermineQuantity(stock.livePrice, userHolding, rec.action);
    
    // ---------- Human-readable Explanation ----------
    std::ostringstream oss;
    oss << "Recommendation: " << Utils::GetActionString(rec.action) << "\n";
    oss << "rec.score: " << rec.score << "%\n";
    oss << "Quantity suggested: " << rec.quantity << "\n";
    oss << "Reasoning:\n";
    
    if (userHolding.quantity > 0)
      oss << "- Unrealized P/L: " << unrealizedPLPercent << "% on " << userHolding.quantity
      << " shares at avg price " << userHolding.avgPrice << "\n";
    
    oss << "- Interval price change: " << intervalChangePercent << "%\n";
    oss << "- Momentum: Short-term trend is " << Utils::GetMomentumString(momentumReport.shortTermBehavior)
    << ", long-term trend is " << Utils::GetMomentumString(momentumReport.longTermBehavior) << ".\n";
    oss << "- Technicals: RSI = " << techReport.RSI
    << ", SMA50 = " << (techReport.SMA.count(50) ? techReport.SMA.at(50) : 0.0) << ".\n";
    oss << "- Volatility: Short-term = " << volReport.shortTermVolatility
    << "%, Long-term = " << volReport.longTermVolatility << "%.\n";
    oss << "- Chart Candlestick patterns: ";
    for (const auto& p : chartReport.candlestickPatterns) oss << p << " ";
    if (chartReport.candlestickPatterns.empty()) oss << "None";
    oss << ".\n";
    
    rec.explanation = oss.str();
    return rec;
  }

  // --------------------------
  // Determine quantity to trade
  // --------------------------
  double RecommendationEngine::DetermineQuantity(double stockPrice, const UserHoldingForAnalyzer& userHolding, Action action)
  {
    if (action == Action::Buy || action == Action::StrongBuy)
    {
      if (userHolding.quantity > 0)
        return std::max(1.0, userHolding.quantity * 0.5);
      else
        return 100.0;
    }
    else if (action == Action::Sell || action == Action::StrongSell)
    {
      return userHolding.quantity * 0.5;
    }
    return 0.0;
  }
  
  namespace Utils
  {
    std::string GetActionString(Action action)
    {
      switch (action)
      {
        case Action::StrongBuy: return "Strong Buy";
        case Action::Buy:       return "Buy";
        case Action::Hold:      return "Hold";
        case Action::Sell:      return "Sell";
        case Action::StrongSell:return "Strong Sell";
        default: return "Unknown";
      }
    }
  }
} // namespace KanVest
