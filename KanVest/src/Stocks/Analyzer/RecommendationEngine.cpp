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
//    rec.score = 0.0;
//    
//    // ---------- Compute Unrealized P/L ----------
//    double unrealizedPLPercent = 0.0;
//    if (userHolding.quantity > 0)
//      unrealizedPLPercent = (stock.livePrice - userHolding.avgPrice) / userHolding.avgPrice * 100.0;
//    
//    // ---------- Compute interval price change ----------
//    double intervalChangePercent = 0.0;
//    if (!stock.history.empty())
//    {
//      const auto& start = stock.history.front();
//      const auto& end = stock.history.back();
//      intervalChangePercent = (end.close - start.close) / start.close * 100.0;
//    }
//    
//    // ============================================================
//    // MOMENTUM (short-term trend direction)
//    // ============================================================
//    if (momentumReport.shortTermBehavior == Momentum::VeryPositive) rec.score += 10;
//    else if (momentumReport.shortTermBehavior == Momentum::Positive) rec.score += 5;
//    else if (momentumReport.shortTermBehavior == Momentum::Negative) rec.score -= 5;
//    else if (momentumReport.shortTermBehavior == Momentum::VeryNegative) rec.score -= 10;
//    
//    // ============================================================
//    // RSI — Overbought / Oversold
//    // ============================================================
//    if (techReport.RSI < 30) { rec.score += 7; rec.explanation += "RSI oversold (<30). "; }
//    else if (techReport.RSI > 70) { rec.score -= 7; rec.explanation += "RSI overbought (>70). "; }
//    
//    // ============================================================
//    // SMA — 50-day trend check
//    // ============================================================
//    if (techReport.SMA.count(50))
//    {
//      if (stock.livePrice > techReport.SMA.at(50)) { rec.score += 5; rec.explanation += "Price above SMA50 (bullish). "; }
//      else { rec.score -= 5; rec.explanation += "Price below SMA50 (bearish). "; }
//    }
//    
//    // ============================================================
//    // EMA — trend sensitivity
//    // ============================================================
//    if (techReport.EMA.count(20) && techReport.EMA.count(50))
//    {
//      double ema20 = techReport.EMA.at(20);
//      double ema50 = techReport.EMA.at(50);
//      if (ema20 > ema50) { rec.score += 5; rec.explanation += "EMA20>EMA50 bullish crossover. "; }
//      else { rec.score -= 5; rec.explanation += "EMA20<EMA50 bearish crossover. "; }
//    }
//    
//    // ============================================================
//    // MACD — momentum crossover
//    // ============================================================
//    if (!std::isnan(techReport.MACD) && !std::isnan(techReport.MACDSignal))
//    {
//      if (techReport.MACD > techReport.MACDSignal) { rec.score += 7; rec.explanation += "MACD bullish crossover. "; }
//      else { rec.score -= 7; rec.explanation += "MACD bearish crossover. "; }
//    }
//    
//    // ============================================================
//    // Awesome Oscillator
//    // ============================================================
//    if (techReport.AwesomeOscillator > 0) { rec.score += 4; rec.explanation += "Awesome Oscillator bullish. "; }
//    else { rec.score -= 4; rec.explanation += "Awesome Oscillator bearish. "; }
//    
//    // ============================================================
//    // Stochastic RSI
//    // ============================================================
//    if (techReport.StochasticRSI < 20) { rec.score += 5; rec.explanation += "Stochastic RSI oversold (<20). "; }
//    else if (techReport.StochasticRSI > 80) { rec.score -= 5; rec.explanation += "Stochastic RSI overbought (>80). "; }
//    
//    // ============================================================
//    // Commodity Channel Index (CCI)
//    // ============================================================
//    if (techReport.CCI > 100) { rec.score += 3; rec.explanation += "CCI strong positive trend (>100). "; }
//    else if (techReport.CCI < -100) { rec.score -= 3; rec.explanation += "CCI weak trend (<-100). "; }
//    
//    // ============================================================
//    // ADX (trend strength)
//    // ============================================================
//    if (techReport.ADX != 0.0)
//    {
//      if (techReport.ADX > 25) { rec.score += 3; rec.explanation += "ADX shows strong trend (>25). "; }
//      else { rec.score -= 2; rec.explanation += "ADX weak trend (<20). "; }
//    }
//    
//    // ============================================================
//    // MFI (Money Flow Index)
//    // ============================================================
//    if (techReport.MFI != 0.0)
//    {
//      if (techReport.MFI < 20) { rec.score += 4; rec.explanation += "MFI oversold (<20). "; }
//      else if (techReport.MFI > 80) { rec.score -= 4; rec.explanation += "MFI overbought (>80). "; }
//    }
//    
//    // ============================================================
//    // OBV (On-Balance Volume)
//    // ============================================================
//    if (techReport.OBV > 0) { rec.score += 4; rec.explanation += "OBV rising (buying pressure). "; }
//    else if (techReport.OBV < 0) { rec.score -= 4; rec.explanation += "OBV falling (selling pressure). "; }
//    
//    // ============================================================
//    // ATR (volatility sensitivity)
//    // ============================================================
//    if (techReport.ATR > 0.0)
//      rec.explanation += "ATR=" + std::to_string(techReport.ATR) + " (volatility measure). ";
//    
//    // ============================================================
//    // VWAP (intraday fair price)
//    // ============================================================
//    if (techReport.VWAP > 0.0)
//    {
//      if (stock.livePrice > techReport.VWAP) { rec.score += 3; rec.explanation += "Price above VWAP (bullish). "; }
//      else { rec.score -= 3; rec.explanation += "Price below VWAP (bearish). "; }
//    }
//    
//    // ============================================================
//    // Volatility adjustments (reduces confidence)
//    // ============================================================
//    rec.score -= volReport.shortTermVolatility / 10.0;
//    
//    // ============================================================
//    // Chart Patterns (candlesticks & formations)
//    // ============================================================
//    double patternScore = 0.0;
//    for (const auto& pattern : chartReport.candlestickPatterns)
//      if (candlestickScoreMap.count(pattern)) patternScore += candlestickScoreMap[pattern];
//    patternScore /= std::max(1.0, static_cast<double>(chartReport.candlestickPatterns.size()));
//    patternScore = std::clamp(patternScore, -5.0, 5.0);
//    rec.score += patternScore;
//    
//    double chartScore = 0.0;
//    for (const auto& pattern : chartReport.chartPatterns)
//      if (chartPatternScoreMap.count(pattern)) chartScore += chartPatternScoreMap[pattern];
//    chartScore = std::clamp(chartScore, -5.0, 5.0);
//    rec.score += chartScore;
//    
//    // ============================================================
//    // Interval & performance scaling
//    // ============================================================
//    if (intervalChangePercent <= -10.0) rec.score = std::min(rec.score, 40.0);
//    else if (intervalChangePercent <= -5.0) rec.score = std::min(rec.score, 50.0);
//    else if (intervalChangePercent >= 10.0) rec.score = std::max(rec.score, 80.0);
//    
//    // ============================================================
//    // Normalize to 0–100 and derive final action
//    // ============================================================
//    rec.score = std::clamp(rec.score, 0.0, 100.0);
//    
//    if (rec.score >= 70) rec.action = Action::StrongBuy;
//    else if (rec.score >= 55) rec.action = Action::Buy;
//    else if (rec.score <= 30) rec.action = Action::StrongSell;
//    else if (rec.score <= 45) rec.action = Action::Sell;
//    else rec.action = Action::Hold;
//    
//    // ============================================================
//    // Suggested Quantity & Explanation
//    // ============================================================
//    rec.quantity = DetermineQuantity(stock.livePrice, userHolding, rec.action);
//    
//    std::ostringstream oss;
//    oss << "Recommendation: " << Utils::GetActionString(rec.action) << "\n";
//    oss << "Score: " << rec.score << "%\n";
//    oss << "Quantity: " << rec.quantity << "\n";
//    if (userHolding.quantity > 0)
//      oss << "Unrealized P/L: " << unrealizedPLPercent << "% on "
//      << userHolding.quantity << " shares @ " << userHolding.avgPrice << "\n";
//    oss << "Interval change: " << intervalChangePercent << "%\n";
//    oss << "Technical Summary:\n" << rec.explanation << "\n";
//    
//    rec.explanation = oss.str();
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
