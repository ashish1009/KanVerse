//
//  Analyzer.cpp
//  KanVest
//
//  Created by Ashish . on 07/11/25.
//

#include "Analyzer.hpp"

#include "Stocks/StockManager.hpp"
#include "Stocks/Analyzer/Indicators.hpp"
#include "Stocks/Analyzer/CandleSticks.hpp"
#include "Stocks/Analyzer/ChartPattern.hpp"

using namespace KanVest::Analysis;
using namespace KanVest::Analysis::Indicators;
using namespace KanVest::Analysis::Candles;
using namespace KanVest::Analysis::ChartPatterns;
using namespace KanVest;

static std::vector<double> closesFrom(const std::vector<StockPoint>& h)
{
  std::vector<double> out;
  out.reserve(h.size());
  for (auto &p : h) out.push_back(p.close);
  {
    return out;
  }
}
static std::vector<double> highsFrom(const std::vector<StockPoint>& h)
{
  std::vector<double> out;
  out.reserve(h.size());
  for (auto &p : h) out.push_back(p.high);
  {
    return out;
  }
}
static std::vector<double> lowsFrom(const std::vector<StockPoint>& h)
{
  std::vector<double> out;
  out.reserve(h.size());
  for (auto &p : h) out.push_back(p.low);
  {
    return out;
  }
}
static std::vector<uint64_t> volsFrom(const std::vector<StockPoint>& h)
{
  std::vector<uint64_t> out;
  out.reserve(h.size());
  for (auto &p : h) out.push_back(p.volume);
  {
    return out;
  }
}

AnalysisReport StockAnalyzer::Analyze(const StockData& shortTerm, const StockData* longTerm, const HoldingInfo* holding) const
{
  return BuildReport(shortTerm, longTerm, holding);
}

AnalysisReport StockAnalyzer::BuildReport(const StockData& s, const StockData* l, const HoldingInfo* holding) const
{
  AnalysisReport r;
  if (!s.IsValid()) {
    r.explanation = "Invalid or insufficient data.";
    return r;
  }
  
  // --- Extract arrays ---
  auto closes = closesFrom(s.history);
  auto highs  = highsFrom(s.history);
  auto lows   = lowsFrom(s.history);
  auto vols   = volsFrom(s.history);
  if (closes.size() < 50) {
    r.explanation = "Not enough data for reliable analysis.";
    return r;
  }
  
  r.lastClose = closes.back();
  
  // --- Core Indicators ---
  r.smaShort = SMA(closes, 20);
  r.smaLong  = SMA(closes, 50);
  r.rsi      = RSI(closes, 14);
  auto [macd, macdSignal] = MACD(closes);
  r.macd = macd;
  r.macdSignal = macdSignal;
  r.atr = ATR(highs, lows, closes, 14);
  r.volatility = (r.atr / r.lastClose) * 100.0;  // ATR% of price
  r.obv = OBV(closes, vols);
  r.adx = ADX(highs, lows, closes);
  
  auto near = [](double a, double b, double tol) {
    return std::fabs(a - b) <= tol;
  };
  
  // --------------------------------------------------------
  // STEP 1: Support / Resistance Detection (Improved Clustering)
  // --------------------------------------------------------
  struct Level { double price; int touches; };
  std::vector<Level> highsDetected, lowsDetected;
  
  for (size_t i = 2; i < closes.size() - 2; ++i) {
    bool isHigh = highs[i] > highs[i-1] && highs[i] > highs[i-2] &&
    highs[i] > highs[i+1] && highs[i] > highs[i+2];
    bool isLow  = lows[i]  < lows[i-1]  && lows[i]  < lows[i-2]  &&
    lows[i]  < lows[i+1]  && lows[i]  < lows[i+2];
    if (isHigh) highsDetected.push_back({ highs[i], 1 });
    if (isLow)  lowsDetected.push_back({ lows[i], 1 });
  }
  
  auto clusterLevels = [](std::vector<Level>& lvls) {
    std::vector<Level> result;
    if (lvls.empty()) return result;
    std::sort(lvls.begin(), lvls.end(), [](auto& a, auto& b){ return a.price < b.price; });
    
    double clusterTol = 0.015;
    double clusterSum = lvls[0].price;
    int clusterCount = 1;
    for (size_t i = 1; i < lvls.size(); ++i) {
      double avg = clusterSum / clusterCount;
      double diff = std::fabs(lvls[i].price - avg) / avg;
      if (diff < clusterTol) {
        clusterSum += lvls[i].price;
        clusterCount++;
      } else {
        result.push_back({ clusterSum / clusterCount, clusterCount });
        clusterSum = lvls[i].price;
        clusterCount = 1;
      }
    }
    result.push_back({ clusterSum / clusterCount, clusterCount });
    return result;
  };
  
  auto resistance = clusterLevels(highsDetected);
  auto support    = clusterLevels(lowsDetected);
  
  for (auto& lv : resistance) r.resistanceLevels.push_back(lv.price);
  for (auto& lv : support)    r.supportLevels.push_back(lv.price);
  
  // --- Find nearest support/resistance ---
  r.nearestSupportLevel    = !r.supportLevels.empty() ? r.supportLevels.front() : NAN;
  r.nearestResistanceLevel = !r.resistanceLevels.empty() ? r.resistanceLevels.front() : NAN;
  
  double nearestSupportDist = 1e9, nearestResistanceDist = 1e9;
  for (double sLvl : r.supportLevels)
    nearestSupportDist = std::min(nearestSupportDist, std::fabs(r.lastClose - sLvl));
  for (double rLvl : r.resistanceLevels)
    nearestResistanceDist = std::min(nearestResistanceDist, std::fabs(r.lastClose - rLvl));
  
  // --------------------------------------------------------
  // STEP 2: Volatility & Risk Weighting
  // --------------------------------------------------------
  double atrPct = r.volatility;
  double volatilityPenalty = 1.0;
  if (atrPct > 6.0)      volatilityPenalty = 0.7;
  else if (atrPct > 4.0) volatilityPenalty = 0.85;
  else if (atrPct > 2.5) volatilityPenalty = 0.95;
  bool highVol = (atrPct > 4.0);
  
  // --------------------------------------------------------
  // STEP 3: Technical Scoring
  // --------------------------------------------------------
  double score = 0.0;
  if (r.smaShort > r.smaLong) score += 0.4; else score -= 0.4;
  if (r.macd > r.macdSignal)  score += 0.3; else score -= 0.3;
  if (r.rsi < 30) score += 0.3; else if (r.rsi > 70) score -= 0.3;
  if (r.adx > 25) score *= 1.1;
  score *= volatilityPenalty;
  
  // --- Proximity to S/R influence ---
  if (nearestSupportDist < r.atr)     score += 0.2;
  if (nearestResistanceDist < r.atr)  score -= 0.2;
  
  score = std::clamp(score, -1.0, 1.0);
  r.score = score;
  
  // --------------------------------------------------------
  // STEP 4: Final Recommendation + Reason
  // --------------------------------------------------------
  if (score > 0.3 && !highVol && nearestResistanceDist > r.atr * 1.2) {
    r.recommendation = Recommendation::Buy;
    if (r.rsi < 35)
      r.actionReason = "RSI shows oversold levels with strong upward momentum.";
    else
      r.actionReason = "Momentum and moving averages confirm bullish trend.";
  }
  else if (score < -0.3 || (nearestResistanceDist < r.atr && r.smaShort < r.smaLong)) {
    r.recommendation = Recommendation::Sell;
    if (r.rsi > 70)
      r.actionReason = "RSI indicates overbought conditions : Possible correction.";
    else
      r.actionReason = "Price nearing resistance with weak trend signals.";
  }
  else {
    r.recommendation = Recommendation::Hold;
    r.actionReason = "Mixed technicals : No strong directional bias.";
  }
  
  // --------------------------------------------------------
  // STEP 5: Suggested Quantity Logic
  // --------------------------------------------------------
  double baseQty = 0.0;
  if (r.recommendation == Recommendation::Buy) {
    if (holding) {
      if (holding->qty > 0)
        baseQty = holding->qty * 0.5;
      else
        baseQty = 100;
    } else {
      baseQty = 100.0 * std::clamp(score, 0.3, 1.0);
    }
    
    // Volatility-adaptive position sizing
    double volFactor = std::clamp(1.0 - (r.volatility / 15.0), 0.4, 1.0);
    baseQty *= volFactor;
    
    r.suggestedActionQty = static_cast<int>(std::max(0.0, baseQty));
  }
  else if (r.recommendation == Recommendation::Sell) {
    if (holding && holding->qty > 0)
      r.suggestedActionQty = static_cast<int>(holding->qty);
    else
      r.suggestedActionQty = 0;
  }
  else r.suggestedActionQty = 0;
  
  // --------------------------------------------------------
  // STEP 6: Explanations
  // --------------------------------------------------------
  std::ostringstream summary;
  summary << (r.recommendation == Recommendation::Buy ? "Buy" :
              (r.recommendation == Recommendation::Sell ? "Sell" : "Hold"))
  << " signal — " << r.actionReason;
  if (highVol) summary << " ⚠️ High volatility may impact reliability.";
  summary << "\nSuggested Quantity: " << r.suggestedActionQty;
  
  r.explanation = summary.str();
  
  // --------------------------------------------------------
  // STEP 7: Human-Friendly Explanation
  // --------------------------------------------------------
  std::ostringstream human;
  if (r.recommendation == Recommendation::Buy) {
    human << "The stock shows bullish signs — trend and momentum favor buying. ";
    if (!r.supportLevels.empty() && near(r.lastClose, r.supportLevels.front(), r.atr))
      human << "It's close to a support zone, adding confidence. ";
  } else if (r.recommendation == Recommendation::Sell) {
    human << "The stock appears weak or near resistance — selling pressure may increase. ";
  } else {
    human << "Market signals are neutral — price may consolidate before direction emerges. ";
  }
  
  if (r.volatility > 5.0)
    human << "Volatility is high, so price swings could be large.";
  
  // --- Technical snapshot ---
  std::ostringstream ds;
  ds << std::fixed << std::setprecision(2)
  << "\nClose: " << r.lastClose
  << " | RSI: " << r.rsi
  << " | MACD: " << r.macd << "/" << r.macdSignal
  << " | SMA(20): " << r.smaShort
  << " | SMA(50): " << r.smaLong
  << " | ATR%: " << atrPct
  << " | ADX: " << r.adx
  << " | Score: " << r.score << "\n";
  
  r.detailedExplanation = human.str() + "\n" + ds.str();
  return r;
}
