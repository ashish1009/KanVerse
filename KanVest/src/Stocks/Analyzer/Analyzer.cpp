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
  r.volatility = (r.atr / r.lastClose) * 100.0; // ATR% of price
  r.obv = OBV(closes, vols);
  r.adx = ADX(highs, lows, closes);
  
  // --- Helper ---
  auto near = [](double a, double b, double tol) {
    return std::fabs(a - b) <= tol;
  };
  
  // --------------------------------------------------------
  // STEP 1: Improved Support and Resistance Detection
  // --------------------------------------------------------
  struct Level { double price; int touches; };
  std::vector<Level> highsDetected;
  std::vector<Level> lowsDetected;
  
  // Identify local pivots
  for (size_t i = 2; i < closes.size() - 2; ++i) {
    bool isHigh = highs[i] > highs[i-1] && highs[i] > highs[i-2] &&
    highs[i] > highs[i+1] && highs[i] > highs[i+2];
    bool isLow  = lows[i]  < lows[i-1]  && lows[i]  < lows[i-2]  &&
    lows[i]  < lows[i+1]  && lows[i]  < lows[i+2];
    if (isHigh) highsDetected.push_back({ highs[i], 1 });
    if (isLow)  lowsDetected.push_back({ lows[i], 1 });
  }
  
  // Merge nearby pivots into zones (improved clustering)
  auto clusterLevels = [](std::vector<Level>& lvls) {
    std::vector<Level> result;
    if (lvls.empty()) return result;
    std::sort(lvls.begin(), lvls.end(), [](auto& a, auto& b){ return a.price < b.price; });
    
    double clusterTol = 0.015; // 1.5% zone width
    double clusterSum = lvls[0].price;
    int clusterCount = 1;
    for (size_t i = 1; i < lvls.size(); ++i) {
      double diff = std::fabs(lvls[i].price - (clusterSum / clusterCount)) / (clusterSum / clusterCount);
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
  
  // --------------------------------------------------------
  // STEP 2: Volatility & Risk Scoring
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
  
  // Trend bias
  if (r.smaShort > r.smaLong) score += 0.4; else score -= 0.4;
  
  // Momentum
  if (r.macd > r.macdSignal) score += 0.3; else score -= 0.3;
  
  // RSI zones
  if (r.rsi < 30) score += 0.3;
  else if (r.rsi > 70) score -= 0.3;
  
  // Trend strength
  if (r.adx > 25) score *= 1.1;
  
  // Apply volatility dampening
  score *= volatilityPenalty;
  
  // --------------------------------------------------------
  // STEP 4: Proximity Influence (Support/Resistance)
  // --------------------------------------------------------
  double proximityBias = 0.0;
  double nearestSupportDist = 1e9;
  double nearestResistanceDist = 1e9;
  
  for (auto& sLvl : support)
    nearestSupportDist = std::min(nearestSupportDist, std::fabs(r.lastClose - sLvl.price));
  for (auto& rLvl : resistance)
    nearestResistanceDist = std::min(nearestResistanceDist, std::fabs(r.lastClose - rLvl.price));
  
  if (nearestSupportDist < r.atr) proximityBias += 0.2;     // near support → bullish tilt
  if (nearestResistanceDist < r.atr) proximityBias -= 0.2;  // near resistance → bearish tilt
  
  score += proximityBias;
  
  // Clamp score
  score = std::clamp(score, -1.0, 1.0);
  r.score = score;
  
  // --------------------------------------------------------
  // STEP 5: Final Recommendation (integrated logic)
  // --------------------------------------------------------
  if (score > 0.3 && !highVol && nearestResistanceDist > r.atr * 1.2)
    r.recommendation = Recommendation::Buy;
  else if (score < -0.3 || (nearestResistanceDist < r.atr && r.smaShort < r.smaLong))
    r.recommendation = Recommendation::Sell;
  else
    r.recommendation = Recommendation::Hold;
  
  // --------------------------------------------------------
  // STEP 6: Explanations
  // --------------------------------------------------------
  std::ostringstream ss;
  if (r.recommendation == Recommendation::Buy)
    ss << "Buy signal: bullish trend with technical confirmation.";
  else if (r.recommendation == Recommendation::Sell)
    ss << "Sell signal: weakening trend or resistance pressure.";
  else
    ss << "Hold: mixed signals or uncertain zone.";
  
  if (highVol)
    ss << " ⚠️ High volatility — signals less reliable.";
  
  r.explanation = ss.str();
  
  // --- Detailed summary (unchanged) ---
  std::ostringstream ds;
  ds << std::fixed << std::setprecision(2);
  ds << "Close: " << r.lastClose
  << " | RSI: " << r.rsi
  << " | MACD: " << r.macd << "/" << r.macdSignal
  << " | SMA(20): " << r.smaShort
  << " | SMA(50): " << r.smaLong
  << " | ATR%: " << atrPct << "%"
  << " | ADX: " << r.adx
  << " | Score: " << score << "\n";
  
  if (!r.resistanceLevels.empty()) {
    ds << "Resistance: ";
    for (auto v : r.resistanceLevels) ds << v << " ";
    ds << "\n";
  }
  if (!r.supportLevels.empty()) {
    ds << "Support: ";
    for (auto v : r.supportLevels) ds << v << " ";
    ds << "\n";
  }
  
  std::string technicalSummary = ds.str();
  
  // --- Layman explanation (unchanged) ---
  std::ostringstream human;
  if (r.recommendation == Recommendation::Buy) {
    if (r.rsi < 30)
      human << "The stock looks oversold and could rebound soon. ";
    else if (r.smaShort > r.smaLong && r.macd > r.macdSignal)
      human << "The short-term trend is strong and momentum is positive. ";
    else
      human << "Overall buying pressure seems to be building. ";
  } else if (r.recommendation == Recommendation::Sell) {
    if (r.rsi > 70)
      human << "The stock appears overbought — it might correct soon. ";
    else if (r.smaShort < r.smaLong && r.macd < r.macdSignal)
      human << "The trend is weakening and momentum has turned bearish. ";
    else
      human << "Sellers are gaining control. ";
  } else {
    human << "The signals are mixed — the stock may be consolidating. ";
  }
  
  if (!r.supportLevels.empty() && near(r.lastClose, r.supportLevels.front(), r.atr))
    human << "Price is near a support zone, which might act as a floor. ";
  if (!r.resistanceLevels.empty() && near(r.lastClose, r.resistanceLevels.front(), r.atr))
    human << "Price is near a resistance level, which could slow gains. ";
  if (r.volatility > 5.0)
    human << "Volatility is high, so expect larger price swings.";
  
  r.detailedExplanation = human.str() + "\n\n" + technicalSummary;
  
  return r;
}
