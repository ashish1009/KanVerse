//
//  RecommendationEngine.cpp
//  KanVest
//
//  Created by Ashish . on 10/11/25.
//

#include "RecommendationEngine.hpp"

namespace KanVest
{
//  std::unordered_map<std::string, int> candlestickScoreMap = {
//    {"BullishEngulfing", 2},
//    {"Hammer", 2},
//    {"MorningStar", 3},
//    {"PiercingLine", 2},
//    {"BearishEngulfing", -2},
//    {"ShootingStar", -2},
//    {"EveningStar", -3},
//    {"DarkCloudCover", -2},
//    {"Doji", 0}, // neutral
//    {"SpinningTop", 0} // neutral
//    // Add more as needed
//  };
//  
//  std::unordered_map<std::string, int> chartPatternScoreMap = {
//    {"HeadAndShoulders", -3},
//    {"InverseHeadAndShoulders", 3},
//    {"DoubleTop", -2},
//    {"DoubleBottom", 2},
//    {"TriangleAscending", 2},
//    {"TriangleDescending", -2},
//    {"Flag", 1},
//    {"Pennant", 1}
//    // Add more patterns here
//  };
//
  
  static float GetMovingAverageScore(float currentPrice, const std::map<int, double>& SMA, const std::map<int, double>& EMA)
  {
    float score = 0.0f;
    
    // Technical periods considered
    std::vector<int> periods = {5, 10, 20, 30, 50, 100, 150, 200};
    
    // Weight per period (longer periods have more influence)
    std::unordered_map<int, double> weights = {
      {5, 0.5}, {10, 0.75}, {20, 1.0}, {30, 1.25},
      {50, 1.5}, {100, 2.0}, {150, 2.25}, {200, 2.5}
    };
    
    // --- Analyze SMA/EMA influence ---
    for (int days : periods)
    {
      double sma = (SMA.count(days)) ? SMA.at(days) : 0.0;
      double ema = (EMA.count(days)) ? EMA.at(days) : 0.0;
      double w = weights[days];
      
      if (sma > 0.0)
      {
        if (currentPrice > sma)
          score += w * 2.0; // bullish
        else
          score -= w * 2.0; // bearish
      }
      
      if (ema > 0.0)
      {
        if (currentPrice > ema)
          score += w * 3.0; // stronger bullish bias for EMA
        else
          score -= w * 3.0; // stronger bearish bias
      }
    }
    return score;
  }
  
  Recommendation RecommendationEngine::Generate(
                                                const StockData& stock,
                                                const TechnicalReport& techReport,
//                                                const MomentumReport& momentumReport,
//                                                const VolatilityReport& volReport,
//                                                const ChartReport& chartReport,
//                                                const PerformanceReport& perfReport,
                                                const UserHoldingForAnalyzer& userHolding
                                                )
  {
    IK_PERFORMANCE_FUNC("RecommendationEngine::Generate");
    Recommendation rec;
    rec.score = 50.0;
    
    if (!stock.IsValid())
    {
      return rec;
    }
    
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
    
    // SMA / EMA
    rec.score += GetMovingAverageScore(stock.history.back().close, techReport.SMA, techReport.EMA);

//    // ============================================================
//    // Interval Momentum Boost — recent price action (major fix)
//    // ============================================================
//    if (intervalChangePercent >= 10.0) {
//      rec.score += 10;
//      rec.explanation += "Strong recent rally (+10%). ";
//    } else if (intervalChangePercent >= 5.0) {
//      rec.score += 6;
//      rec.explanation += "Moderate upward trend (+5%). ";
//    } else if (intervalChangePercent <= -5.0) {
//      rec.score -= 6;
//      rec.explanation += "Moderate decline (-5%). ";
//    } else if (intervalChangePercent <= -10.0) {
//      rec.score -= 10;
//      rec.explanation += "Sharp decline (-10%). ";
//    }

//    if (techReport.EMA.count(20) && techReport.EMA.count(50))
//    {
//      double ema20 = techReport.EMA.at(20);
//      double ema50 = techReport.EMA.at(50);
//      if (ema20 > ema50) {
//        rec.score += 6;
//        rec.explanation += "EMA20>EMA50 bullish crossover. ";
//      } else {
//        rec.score -= 6;
//        rec.explanation += "EMA20<EMA50 bearish crossover. ";
//      }
//    }


//    // ============================================================
//    // MOMENTUM (short-term trend direction)
//    // ============================================================
//    if (momentumReport.shortTermBehavior == Momentum::VeryPositive) rec.score += 10;
//    else if (momentumReport.shortTermBehavior == Momentum::Positive) rec.score += 6;
//    else if (momentumReport.shortTermBehavior == Momentum::Negative) rec.score -= 6;
//    else if (momentumReport.shortTermBehavior == Momentum::VeryNegative) rec.score -= 10;
//    
//    // ============================================================
//    // RSI — Overbought / Oversold (caution signal, not reversal)
//    // ============================================================
//    if (techReport.RSI < 30) {
//      rec.score += 5;
//      rec.explanation += "RSI oversold (<30). ";
//    }
//    else if (techReport.RSI > 70) {
//      rec.score -= 3; // smaller penalty, not a sell trigger
//      rec.explanation += "RSI overbought (>70). ";
//    }
//    
//    // ============================================================
//    // MACD — momentum confirmation
//    // ============================================================
//    if (!std::isnan(techReport.MACD) && !std::isnan(techReport.MACDSignal))
//    {
//      if (techReport.MACD > techReport.MACDSignal) {
//        rec.score += 7;
//        rec.explanation += "MACD bullish crossover. ";
//      } else {
//        rec.score -= 7;
//        rec.explanation += "MACD bearish crossover. ";
//      }
//    }
//    
//    // ============================================================
//    // Awesome Oscillator
//    // ============================================================
//    if (techReport.AwesomeOscillator > 0) {
//      rec.score += 4;
//      rec.explanation += "AO bullish momentum. ";
//    } else {
//      rec.score -= 4;
//      rec.explanation += "AO bearish momentum. ";
//    }
//    
//    // ============================================================
//    // Stochastic RSI — only strong oversold/overbought conditions
//    // ============================================================
//    if (techReport.StochasticRSI < 20) {
//      rec.score += 3;
//      rec.explanation += "StochRSI oversold (<20). ";
//    } else if (techReport.StochasticRSI > 80) {
//      rec.score -= 3;
//      rec.explanation += "StochRSI overbought (>80). ";
//    }
//    
//    // ============================================================
//    // Commodity Channel Index (CCI)
//    // ============================================================
//    if (techReport.CCI > 100) {
//      rec.score += 3;
//      rec.explanation += "CCI indicates strong upward momentum (>100). ";
//    } else if (techReport.CCI < -100) {
//      rec.score -= 3;
//      rec.explanation += "CCI indicates weak momentum (<-100). ";
//    }
//    
//    // ============================================================
//    // ADX — trend strength
//    // ============================================================
//    if (techReport.ADX > 25) {
//      rec.score += 4;
//      rec.explanation += "ADX shows strong trend (>25). ";
//    } else if (techReport.ADX < 20 && techReport.ADX > 0) {
//      rec.score -= 2;
//      rec.explanation += "ADX shows weak trend (<20). ";
//    }
//    
//    // ============================================================
//    // MFI (Money Flow Index)
//    // ============================================================
//    if (techReport.MFI < 20) {
//      rec.score += 3;
//      rec.explanation += "MFI oversold (<20). ";
//    } else if (techReport.MFI > 80) {
//      rec.score -= 3;
//      rec.explanation += "MFI overbought (>80). ";
//    }
//    
//    // ============================================================
//    // OBV (On-Balance Volume)
//    // ============================================================
//    if (techReport.OBV > 0) {
//      rec.score += 3;
//      rec.explanation += "OBV rising (buying pressure). ";
//    } else if (techReport.OBV < 0) {
//      rec.score -= 3;
//      rec.explanation += "OBV falling (selling pressure). ";
//    }
//    
//    // ============================================================
//    // VWAP (fair price)
//    // ============================================================
//    if (techReport.VWAP > 0.0) {
//      if (stock.livePrice > techReport.VWAP) {
//        rec.score += 4;
//        rec.explanation += "Price above VWAP (bullish). ";
//      } else {
//        rec.score -= 4;
//        rec.explanation += "Price below VWAP (bearish). ";
//      }
//    }
//    
//
//    // ============================================================
//    // Volatility Adjustment (reduces conviction slightly)
//    // ============================================================
//    rec.score -= volReport.shortTermVolatility / 12.0;
//    
//    // ============================================================
//    // Clamp & Decision
//    // ============================================================
//    rec.score = std::clamp(rec.score, 0.0, 100.0);
//    
//    if (rec.score >= 75) rec.action = Action::StrongBuy;
//    else if (rec.score >= 60) rec.action = Action::Buy;
//    else if (rec.score <= 25) rec.action = Action::StrongSell;
//    else if (rec.score <= 40) rec.action = Action::Sell;
//    else rec.action = Action::Hold;
//
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
//
//  // --------------------------
//  // Determine quantity to trade
//  // --------------------------
//  double RecommendationEngine::DetermineQuantity(double stockPrice, const UserHoldingForAnalyzer& userHolding, Action action)
//  {
//    if (action == Action::Buy || action == Action::StrongBuy)
//    {
//      if (userHolding.quantity > 0)
//        return std::max(1.0, userHolding.quantity * 0.5);
//      else
//        return 100.0;
//    }
//    else if (action == Action::Sell || action == Action::StrongSell)
//    {
//      return userHolding.quantity * 0.5;
//    }
//    return 0.0;
//  }
  
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
