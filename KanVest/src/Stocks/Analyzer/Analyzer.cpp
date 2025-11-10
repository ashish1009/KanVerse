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

static double clamp01(double v) { return std::max(0.0, std::min(1.0, v)); }
static double clamp(double v, double a, double b) { return std::max(a, std::min(b, v)); }

// simple helper: cluster levels that are within `tol` absolute distance
static std::vector<double> ClusterLevels(const std::vector<double>& levels, double tol = 0.01)
{
  if (levels.empty()) return {};
  std::vector<double> copy = levels;
  std::sort(copy.begin(), copy.end());
  std::vector<double> out;
  double cur = copy[0];
  double count = 1;
  for (size_t i = 1; i < copy.size(); ++i)
  {
    if (std::abs(copy[i] - cur) <= tol * std::max(1.0, std::abs(cur)))
    {
      // merge by average
      cur = (cur * count + copy[i]) / (count + 1);
      ++count;
    }
    else
    {
      out.push_back(cur);
      cur = copy[i];
      count = 1;
    }
  }
  out.push_back(cur);
  return out;
}

// detect simple local extrema (swing highs/lows) - window is lookback on each side
static void FindSwingHighsLows(const std::vector<double>& closes, int window, std::vector<double>& highs, std::vector<double>& lows)
{
  int n = (int)closes.size();
  for (int i = window; i < n - window; ++i)
  {
    bool isHigh = true, isLow = true;
    for (int j = i - window; j <= i + window; ++j)
    {
      if (closes[j] > closes[i]) isLow = false;
      if (closes[j] < closes[i]) isHigh = false;
    }
    if (isHigh) highs.push_back(closes[i]);
    if (isLow) lows.push_back(closes[i]);
  }
}

// BuildReport implementation
AnalysisReport StockAnalyzer::BuildReport(const StockData& s, const StockData* l, const HoldingInfo* holding) const
{
  AnalysisReport rpt;
  
  const auto& h = s.history;
  if (h.empty()) return rpt;
  
  // Extract series
  auto closes = ::closesFrom(h);
  auto highs = ::highsFrom(h);
  auto lows = ::lowsFrom(h);
  auto vols = ::volsFrom(h);
  
  const double lastClose = closes.back();
  rpt.lastClose = lastClose;
  
  // --- Indicators (call into Indicators namespace where available) ---
  rpt.smaShort = Indicators::SMA(closes, m_cfg.sma_short);
  rpt.smaLong  = Indicators::SMA(closes, m_cfg.sma_long);
  rpt.rsi      = Indicators::RSI(closes, m_cfg.rsi_period);
  rpt.atr      = Indicators::ATR(highs, lows, closes, m_cfg.atr_period);
  rpt.vwap     = Indicators::VWAP(h);
  rpt.obv      = Indicators::OBV(closes, vols);
  rpt.adx      = Indicators::ADX(highs, lows, closes, 14);
  std::tie(rpt.macd, rpt.macdSignal) = Indicators::MACD(closes);
  std::tie(rpt.stochasticK, rpt.stochasticD) = Indicators::Stochastic(highs, lows, closes);
  rpt.cci = Indicators::CCI(highs, lows, closes);
  rpt.roc = Indicators::ROC(closes, 12);
  rpt.mfi = Indicators::MFI(highs, lows, closes, vols);
  rpt.bollinger = Indicators::Bollinger(closes);
  
  // obv slope simple numeric derivative (last 5 points)
  if (h.size() >= 6)
  {
    [[maybe_unused]] double prev = rpt.obv;
    // compute slope over last 5 closes of OBV by re-running OBV on tail
    std::vector<double> tailCloses(closes.end()-6, closes.end());
    std::vector<uint64_t> tailVols(vols.end()-6, vols.end());
    double obv_old = Indicators::OBV(tailCloses, tailVols); // returns last value
    rpt.obvSlope = (rpt.obv - obv_old) / std::max(1.0, std::abs(obv_old));
  }
  
  // volatility as percent
  if (!std::isnan(rpt.atr) && lastClose > 0) rpt.volatility = (rpt.atr / lastClose) * 100.0;
  
  // --- Pattern detection ---
  try {
    auto single = Candles::DetectSingleBarPatterns(h);
    auto multi  = Candles::DetectMultiBarPatterns(h);
    rpt.candlePatterns.insert(rpt.candlePatterns.end(), single.begin(), single.end());
    rpt.candlePatterns.insert(rpt.candlePatterns.end(), multi.begin(), multi.end());
  } catch(...) {}
  
  try {
    // broad chart patterns - call multiple detectors and aggregate
    auto dt = ChartPatterns::DetectDoubleTopBottom(h);
    auto tp = ChartPatterns::DetectTripleTopBottom(h);
    auto hs = ChartPatterns::DetectHeadAndShoulders(h);
    auto rb = ChartPatterns::DetectRoundingBottom(h);
    auto ch = ChartPatterns::DetectCupAndHandle(h);
    auto wg = ChartPatterns::DetectWedges(h);
    auto br = ChartPatterns::DetectBreakoutEvents(h);
    auto sr = ChartPatterns::DetectSupportResistance(h);
    
    std::vector<std::vector<PatternHit>> all = {dt, tp, hs, rb, ch, wg, br, sr};
    for (auto &v : all) for (auto &p : v) rpt.chartPatterns.push_back(p);
  } catch(...) {}
  
  // --- Support & Resistance clustering ---
  std::vector<double> rawSupports, rawResists;
  // use detected SR pattern hits if available
  for (auto &p : rpt.chartPatterns)
  {
    if (p.name.find("Support") != std::string::npos || p.name.find("Bottom") != std::string::npos)
      rawSupports.push_back( (p.startIndex < closes.size()) ? closes[p.startIndex] : lastClose );
    if (p.name.find("Resistance") != std::string::npos || p.name.find("Top") != std::string::npos)
      rawResists.push_back( (p.startIndex < closes.size()) ? closes[p.startIndex] : lastClose );
  }
  // fallback to simple swing highs/lows
  if (rawSupports.empty() || rawResists.empty())
  {
    std::vector<double> sh, sl;
    FindSwingHighsLows(closes, 3, sh, sl);
    rawResists.insert(rawResists.end(), sh.begin(), sh.end());
    rawSupports.insert(rawSupports.end(), sl.begin(), sl.end());
  }
  
  // cluster with tolerance 0.5% of price
  double tol = 0.005;
  rpt.supportLevels = ClusterLevels(rawSupports, tol);
  rpt.resistanceLevels = ClusterLevels(rawResists, tol);
  if (!rpt.supportLevels.empty()) rpt.nearestSupportLevel = rpt.supportLevels.back();
  if (!rpt.resistanceLevels.empty()) rpt.nearestResistanceLevel = rpt.resistanceLevels.front();
  
  // textual lists
  {
    std::ostringstream ss;
    for (double x : rpt.supportLevels) { ss << std::fixed << std::setprecision(4) << x << ","; }
    rpt.allSupportLevel = ss.str();
  }
  {
    std::ostringstream ss;
    for (double x : rpt.resistanceLevels) { ss << std::fixed << std::setprecision(4) << x << ","; }
    rpt.allResistanceLevel = ss.str();
  }
  
  // --- Score aggregation ---
  double score = 0.0; // -1..1
  double weightSum = 0.0;
  
  // --- 0) Multi-interval price momentum ---
  double priceChange1D = Indicators::PriceChangePercent(closes, 1);
  double priceChange5D = Indicators::PriceChangePercent(closes, 5);
  double priceChange1M = Indicators::PriceChangePercent(closes, 21);
  double priceChange3M = Indicators::PriceChangePercent(closes, 63);
  
  // normalize changes into -1..1
  auto norm = [](double pct) {
    if (std::isnan(pct)) return 0.0;
    return clamp(pct / 10.0, -1.0, 1.0);
  };
  
  double trendScore = (norm(priceChange1D) + norm(priceChange5D) + norm(priceChange1M) + norm(priceChange3M)) / 4.0;
  double trendWeight = 0.25;
  score += trendScore * trendWeight;
  weightSum += trendWeight;
  
  // --- 1) Moving average crossover (medium weight) ---
  {
    double w = 0.20;
    double val = 0.0;
    if (!std::isnan(rpt.smaShort) && !std::isnan(rpt.smaLong))
    {
      if (rpt.smaShort > rpt.smaLong) val = +1.0;
      else if (rpt.smaShort < rpt.smaLong) val = -1.0;
    }
    score += val * w;
    weightSum += w;
  }
  
  // --- 2) RSI ---
  {
    double w = 0.15;
    if (!std::isnan(rpt.rsi))
    {
      if (rpt.rsi < 30) score += +1.0 * w;
      else if (rpt.rsi > 70) score += -1.0 * w;
      else score += ((rpt.rsi - 50.0) / 50.0) * (w * 0.5);
    }
    weightSum += w;
  }
  
  // --- 3) MACD ---
  {
    double w = 0.10;
    if (!std::isnan(rpt.macd) && !std::isnan(rpt.macdSignal))
    {
      double diff = rpt.macd - rpt.macdSignal;
      score += clamp(diff / std::max(1e-6, std::abs(rpt.macdSignal) + 1.0), -1.0, 1.0) * w;
    }
    weightSum += w;
  }
  
  // --- 4) ADX - trend strength scaling ---
  double adxFactor = 1.0;
  if (!std::isnan(rpt.adx))
  {
    if (rpt.adx >= 25) adxFactor = 1.2;
    else if (rpt.adx >= 20) adxFactor = 1.05;
    else adxFactor = 0.9;
  }
  
  // --- 5) Bollinger ---
  {
    double w = 0.05;
    if (!std::isnan(rpt.bollinger.middle))
    {
      if (lastClose > rpt.bollinger.upper) score += -0.5 * w;
      else if (lastClose < rpt.bollinger.lower) score += +0.5 * w;
      else
      {
        double mid = rpt.bollinger.middle;
        double rel = (lastClose - mid) / std::max(1e-8, mid);
        score += clamp(rel, -0.5, 0.5) * w;
      }
    }
    weightSum += w;
  }
  
  // --- 6) Momentum indicators (ROC, CCI, Stochastic) ---
  {
    double w = 0.08;
    double val = 0.0;
    if (!std::isnan(rpt.roc)) val += clamp(rpt.roc / 5.0, -1.0, 1.0);
    if (!std::isnan(rpt.cci)) val += clamp(rpt.cci / 200.0, -1.0, 1.0);
    if (!std::isnan(rpt.stochasticK)) val += clamp((rpt.stochasticK - rpt.stochasticD) / 100.0, -1.0, 1.0);
    score += (val / 3.0) * w;
    weightSum += w;
  }
  
  // --- 7) Volume confirmation ---
  {
    double w = 0.07;
    if (vols.size() >= 5)
    {
      size_t n = std::min<size_t>(vols.size(), 20);
      double avg = 0.0;
      for (size_t i = vols.size() - n; i < vols.size(); ++i) avg += (double)vols[i];
      avg /= (double)n;
      double volRel = avg > 0 ? (double)vols.back() / avg : 1.0;
      if (volRel > m_cfg.breakoutVolumeMultiplier)
        score += 0.5 * w * (volRel / (volRel + 1.0));
    }
    weightSum += w;
  }
  
  // --- 8) Patterns with trend-aware weighting ---
  {
    double w = 0.2;
    double patternScore = 0.0;
    for (auto &p : rpt.candlePatterns)
    {
      double s = clamp01(p.strength);
      bool bullish = (p.name.find("Bull") != std::string::npos ||
                      p.name.find("Morning") != std::string::npos ||
                      p.name.find("Piercing") != std::string::npos ||
                      p.name.find("Hammer") != std::string::npos);
      bool bearish = (p.name.find("Bear") != std::string::npos ||
                      p.name.find("Evening") != std::string::npos ||
                      p.name.find("Shooting") != std::string::npos ||
                      p.name.find("Dark Cloud") != std::string::npos);
      if (bullish) patternScore += s;
      if (bearish) patternScore -= s;
    }
    for (auto &p : rpt.chartPatterns)
    {
      double s = clamp01(p.strength);
      bool bullish = (p.name.find("Bottom") != std::string::npos ||
                      p.name.find("Cup") != std::string::npos ||
                      p.name.find("Inverse") != std::string::npos ||
                      p.name.find("Falling Wedge") != std::string::npos);
      bool bearish = (p.name.find("Top") != std::string::npos ||
                      p.name.find("HeadAndShoulders") != std::string::npos ||
                      p.name.find("Rising Wedge") != std::string::npos);
      if (bullish) patternScore += 2.0 * s;
      if (bearish) patternScore -= 2.0 * s;
    }
    
    // trend-aware scaling: reduce bullish patterns if trendScore is negative
    double trendFactor = (1.0 + trendScore) / 2.0; // 0 (downtrend) → 1 (uptrend)
    patternScore *= trendFactor;
    
    if (!rpt.candlePatterns.empty() || !rpt.chartPatterns.empty())
      score += clamp(patternScore, -1.0, 1.0) * w;
    
    weightSum += w;
  }
  
  // --- 9) Support/Resistance bias ---
  {
    double w = 0.05;
    bool nearSupport = (!rpt.supportLevels.empty() && std::abs(lastClose - rpt.supportLevels.back()) / lastClose < 0.02);
    bool nearResistance = (!rpt.resistanceLevels.empty() && std::abs(lastClose - rpt.resistanceLevels.front()) / lastClose < 0.02);
    if (nearSupport && trendScore > -0.3) score += 0.2 * w;
    if (nearResistance && trendScore < 0.3) score -= 0.2 * w;
    weightSum += w;
  }
  
  // --- Price trend impact (short-term & medium-term momentum) ---
  {
    double shortChange = PriceChangePercent(closes, 5);    // 5 bars
    double midChange   = PriceChangePercent(closes, 20);   // 20 bars
    
    // strong negative drift should downgrade signal
    if (shortChange <= -3.0) {
      score -= 0.2 * clamp(std::abs(shortChange) / 10.0, 0.0, 1.0);
    }
    else if (shortChange >= 3.0) {
      score += 0.15 * clamp(std::abs(shortChange) / 10.0, 0.0, 1.0);
    }
    
    // reinforce consistent medium-term direction
    if (midChange <= -8.0)
      score -= 0.25 * clamp(std::abs(midChange) / 15.0, 0.0, 1.0);
    else if (midChange >= 8.0)
      score += 0.2 * clamp(std::abs(midChange) / 15.0, 0.0, 1.0);
    
    // volatility-adjusted moderation
    if (!std::isnan(rpt.volatility) && rpt.volatility > 3.0)
      score *= (1.0 - clamp(rpt.volatility / 50.0, 0.0, 0.4)); // reduce confidence in volatile dips
  }
  
  // --- Combine and finalize ---
  score = (weightSum > 0.0) ? score / weightSum : score;
  score *= adxFactor;
  score = clamp(score, -1.0, 1.0);
  rpt.score = score;
  
  // --- Confidence: combine absolute score, ADX, and volatility ---
  double conf = std::abs(score);
  if (!std::isnan(rpt.adx)) conf *= clamp01(rpt.adx / 50.0 + 0.5);
  if (!std::isnan(rpt.volatility)) conf *= (1.0 - clamp(rpt.volatility / 30.0, 0.0, 0.8));
  rpt.confidence = clamp01(conf) * 100.0;
  
  double absChange = std::abs(PriceChangePercent(closes, 5));
  if (absChange > 4.0)
    rpt.confidence *= 0.8; // lower confidence after large move

  // --- Recommendation map ---
  if (score >= 0.6) rpt.recommendation = Recommendation::StrongBuy;
  else if (score >= 0.15) rpt.recommendation = Recommendation::Buy;
  else if (score > -0.15 && score < 0.15) rpt.recommendation = Recommendation::Hold;
  else if (score <= -0.6) rpt.recommendation = Recommendation::StrongSell;
  else rpt.recommendation = Recommendation::Sell;

  // --- Holding / position suggestions ---
  double accountSize = 100000.0; // default if unknown
  double riskPerTradePct = 0.01; // 1% of account risk
  double riskAmount = accountSize * riskPerTradePct;
  double stopDistance = std::max(rpt.atr, 1e-6); // use ATR as natural stop
  double riskPerShare = stopDistance; // approximate dollars at risk per share
  double suggestedQty = 0.0;
  if (riskPerShare > 1e-8) suggestedQty = std::floor(riskAmount / riskPerShare);
  
  if (holding)
  {
    rpt.hasHolding = true;
    rpt.unrealizedPL = (lastClose - holding->avgPrice) * holding->qty;
    rpt.unrealizedPct = holding->qty > 0 ? (lastClose - holding->avgPrice) / holding->avgPrice * 100.0 : 0.0;
    
    // if recommendation is Buy -> suggest to add up to suggestedQty (scaling by score)
    double scale = std::abs(score);
    if (rpt.recommendation == Recommendation::Buy || rpt.recommendation == Recommendation::StrongBuy)
    {
      double add = suggestedQty * scale;
      // don't suggest absurdly large increases relative to current
      add = std::min(add, std::max(0.0, holding->qty * 2.0));
      rpt.suggestedActionQty = std::ceil(add);
      rpt.actionReason = "Add to position based on technicals and risk sizing.";
    }
    else if (rpt.recommendation == Recommendation::Sell || rpt.recommendation == Recommendation::StrongSell)
    {
      double cut = holding->qty * clamp01(scale);
      // if strong sell, exit fully
      if (rpt.recommendation == Recommendation::StrongSell) rpt.suggestedActionQty = -std::ceil(holding->qty);
      else rpt.suggestedActionQty = -std::ceil(cut);
      rpt.actionReason = "Reduce or close position to protect capital against negative signal.";
    }
    else // Hold
    {
      rpt.suggestedActionQty = 0;
      rpt.actionReason = "Hold existing position; no action recommended by current signal.";
    }
  }
  else
  {
    // no holding - propose buy size if buy signal
    if (rpt.recommendation == Recommendation::Buy || rpt.recommendation == Recommendation::StrongBuy)
    {
      rpt.suggestedActionQty = std::ceil(suggestedQty * std::abs(score));
      rpt.actionReason = "Suggested entry size scaled to risk (1% account risk default).";
    }
    else
    {
      rpt.suggestedActionQty = 0;
      rpt.actionReason = "No buy suggested given current signal.";
    }
  }
  
  // --- Explanation text (short + detailed) ---
  {
    std::ostringstream ss;
    ss << ((rpt.recommendation == Recommendation::StrongBuy) ? "Strong Buy" :
           (rpt.recommendation == Recommendation::Buy) ? "Buy" :
           (rpt.recommendation == Recommendation::Hold) ? "Hold" :
           (rpt.recommendation == Recommendation::Sell) ? "Sell" : "Strong Sell");
    ss << " — ";
    ss << std::fixed << std::setprecision(2) << (rpt.score * 100.0) << "% score, ";
    ss << "confidence " << std::setprecision(1) << rpt.confidence << "%";
    rpt.explanation = ss.str();
  }
  
  {
    std::ostringstream ss;
    ss << "Indicators snapshot:\n";
    ss << " Last: " << std::fixed << std::setprecision(4) << lastClose;
    ss << " | SMA(" << m_cfg.sma_short << ")=" << rpt.smaShort << " SMA(" << m_cfg.sma_long << ")=" << rpt.smaLong;
    ss << " | RSI=" << std::setprecision(2) << rpt.rsi << " | ATR=" << rpt.atr << " | Vol%=" << rpt.volatility << "\n";
    ss << " MACD=" << std::setprecision(4) << rpt.macd << " sig=" << rpt.macdSignal << " | ADX=" << rpt.adx << " | OBV=" << rpt.obv << "\n";
    ss << " Patterns detected: " << rpt.candlePatterns.size() << " candles, " << rpt.chartPatterns.size() << " charts.\n";
    ss << " Support: " << rpt.allSupportLevel << " Resistance: " << rpt.allResistanceLevel << "\n";
    ss << " Suggested action qty: " << rpt.suggestedActionQty << " (" << rpt.actionReason << ")\n";
    rpt.detailedExplanation = ss.str();
  }
  
  // --- Tooltips ---
  rpt.tooltips["SMA"] = "Simple moving average. Short SMA vs Long SMA crossover is used to detect trend direction.";
  rpt.tooltips["RSI"] = "Relative Strength Index. Readings <30 oversold, >70 overbought.";
  rpt.tooltips["ATR"] = "Average True Range. Used as volatility measure and stop-distance estimator.";
  rpt.tooltips["MACD"] = "MACD line vs Signal line. Positive crossover implies bullish momentum.";
  rpt.tooltips["ADX"] = "Average Directional Index measures trend strength; higher values increase confidence.";
  rpt.tooltips["Bollinger"] = "Bollinger Bands show volatility envelope; moves outside bands indicate extended price action.";
  rpt.tooltips["SupportResistance"] = "Detected support/resistance cluster levels where price repeatedly stalls or reverses.";
  rpt.tooltips["Patterns"] = "Candlestick and chart patterns found in the recent history influence the recommendation.";
  
  return rpt;
}
