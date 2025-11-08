//
//  Analyzer.cpp
//  KanVest
//
//  Created by Ashish . on 07/11/25.
//

#include "Analyzer.hpp"

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
  if (!s.IsValid())
  {
    r.explanation = "Short term data invalid";
    return r;
  }
  
  auto closes = closesFrom(s.history);
  auto highs  = highsFrom(s.history);
  auto lows   = lowsFrom(s.history);
  auto vols   = volsFrom(s.history);
  
  r.lastClose = closes.empty() ? 0.0 : closes.back();
  
  // Primary indicators (config-driven)
  r.smaShort = SMA(closes, m_cfg.sma_short);
  r.smaLong  = SMA(closes, m_cfg.sma_long);
  r.rsi      = RSI(closes, m_cfg.rsi_period);
  auto macd_pair = MACD(closes);
  r.macd = macd_pair.first;
  r.macdSignal = macd_pair.second;
  r.atr = ATR(highs, lows, closes, m_cfg.atr_period);
  
  // VWAP using OHLCV StockPoint overload
  r.vwap = VWAP(s.history);
  
  // Additional indicators
  r.bollinger = Bollinger(closes); // returns BB {upper,middle,lower}
  r.obv = OBV(closes, vols);
  
  // OBV slope (delta over recent window)
  {
    size_t look = std::min<size_t>(std::max<size_t>(10, m_cfg.sma_short), closes.size()-1);
    if (closes.size() > look + 1)
    {
      std::vector<double> prevCloses(closes.end() - (look+1), closes.end() - 1);
      std::vector<uint64_t> prevVols(vols.end() - (look+1), vols.end() - 1);
      double prevObv = OBV(prevCloses, prevVols);
      if (!std::isnan(prevObv))
        r.obvSlope = r.obv - prevObv;
    }
  }
  
  r.adx = ADX(highs, lows, closes);
  auto st = Stochastic(highs, lows, closes);
  r.stochasticK = st.first;
  r.stochasticD = st.second;
  r.cci = CCI(highs, lows, closes);
  r.roc = ROC(closes, 12); // 12-period ROC as default short-term momentum
  r.mfi = MFI(highs, lows, closes, vols);
  
  // Candlestick & chart patterns
  r.candlePatterns = DetectSingleBarPatterns(s.history);
  {
    auto multi = DetectMultiBarPatterns(s.history);
    r.candlePatterns.insert(r.candlePatterns.end(), multi.begin(), multi.end());
  }
  
  r.chartPatterns = DetectDoubleTopBottom(s.history);
  {
    auto hs = DetectHeadAndShoulders(s.history);
    r.chartPatterns.insert(r.chartPatterns.end(), hs.begin(), hs.end());
  }
  
  // === Compose score (base + confirmations) ===
  double score = 0.0;
  
  // MACD cross
  if (!std::isnan(r.macd) && !std::isnan(r.macdSignal))
    score += (r.macd > r.macdSignal) ? 0.20 : -0.20;
  
  // RSI extremes
  if (!std::isnan(r.rsi))
  {
    if (r.rsi < 30) score += 0.12;
    else if (r.rsi > 70) score -= 0.12;
  }
  
  // SMA crossover bias
  if (!std::isnan(r.smaShort) && !std::isnan(r.smaLong))
    score += (r.smaShort > r.smaLong) ? 0.12 : -0.12;
  
  // VWAP intraday bias
  if (!std::isnan(r.vwap) && r.lastClose > 0)
    score += (r.lastClose > r.vwap) ? 0.08 : -0.08;
  
  // Bollinger band position
  if (!std::isnan(r.bollinger.middle) && r.bollinger.upper > r.bollinger.lower)
  {
    if (r.lastClose > r.bollinger.upper) score += 0.12;         // breakout above upper band
    else if (r.lastClose < r.bollinger.lower) score -= 0.12;    // breakdown below lower band
    else
    {
      double bandPos = (r.lastClose - r.bollinger.lower) / (r.bollinger.upper - r.bollinger.lower);
      if (bandPos > 0.85) score -= 0.04;   // near top → mild caution
      if (bandPos < 0.15) score += 0.04;   // near bottom → mild support
    }
  }
  
  // OBV confirmation (volume-based)
  if (!std::isnan(r.obv))
  {
    if (r.obvSlope > 0.0) score += 0.10;    // rising OBV supports bullish moves
    else if (r.obvSlope < 0.0) score -= 0.10;
  }
  
  // ADX (trend strength). If trend strong, amplify trend signals
  if (!std::isnan(r.adx))
  {
    if (r.adx > 25.0) // strong trend
    {
      // if price above SMA and MACD bullish, boost score; else penalize
      if (r.smaShort > r.smaLong && r.macd > r.macdSignal) score += 0.08;
      else if (r.smaShort < r.smaLong && r.macd < r.macdSignal) score -= 0.08;
    }
  }
  
  // Stochastic extremes (fast)
  if (!std::isnan(r.stochasticK) && !std::isnan(r.stochasticD))
  {
    if (r.stochasticK < 20 && r.stochasticD < 20) score += 0.06;
    else if (r.stochasticK > 80 && r.stochasticD > 80) score -= 0.06;
  }
  
  // CCI / ROC / MFI minor confirmations
  if (!std::isnan(r.cci))
  {
    if (r.cci < -100) score += 0.03;
    else if (r.cci > 100) score -= 0.03;
  }
  if (!std::isnan(r.roc))
  {
    if (r.roc > 5.0) score += 0.03;   // positive momentum
    else if (r.roc < -5.0) score -= 0.03;
  }
  if (!std::isnan(r.mfi))
  {
    if (r.mfi < 30) score += 0.03;
    else if (r.mfi > 80) score -= 0.03;
  }
  
  // patterns adjust score (candles and chart patterns)
  for (auto &p : r.candlePatterns)
  {
    if (p.name.find("Bull") != std::string::npos || p.name.find("Bullish") != std::string::npos) score += 0.08;
    if (p.name.find("Bear") != std::string::npos || p.name.find("Bearish") != std::string::npos) score -= 0.08;
  }
  
  for (auto &p : r.chartPatterns)
  {
    if (p.name == "DoubleBottom" || p.name == "InverseHeadAndShoulders") score += 0.18;
    if (p.name == "DoubleTop" || p.name == "HeadAndShoulders") score -= 0.18;
    // measured moves / breakouts could further adjust — keep conservative
  }
  
  // long-term confirmation using 'l' (if provided)
  if (l && l->IsValid())
  {
    auto lcloses = closesFrom(l->history);
    double lSma = SMA(lcloses, 30);
    double lEma = EMA(lcloses, 30);
    if (!std::isnan(lSma) && !std::isnan(lEma))
      score += (lEma > lSma) ? 0.12 : -0.12;
    
    // long-term VWAP (if available) — gentle confirmation
    double lvwap = VWAP(l->history);
    if (!std::isnan(lvwap))
      score += (r.lastClose > lvwap) ? 0.04 : -0.04;
  }
  
  // volatility dampening (reduce score when very volatile)
  if (!std::isnan(r.atr) && r.lastClose > 0)
  {
    double atrPct = r.atr / r.lastClose;
    if (atrPct > 0.04) score *= 0.85;
  }
  
  // Bound score & map to recommendation
  r.score = std::max(-1.0, std::min(1.0, score));
  if (r.score >= 0.6) r.recommendation = Recommendation::StrongBuy;
  else if (r.score >= 0.2) r.recommendation = Recommendation::Buy;
  else if (r.score >= -0.2) r.recommendation = Recommendation::Hold;
  else if (r.score >= -0.6) r.recommendation = Recommendation::Sell;
  else r.recommendation = Recommendation::StrongSell;
  
  // === Position / Holding-aware suggestions ===
  if (holding && holding->qty > 0.0 && holding->avgPrice > 0.0)
  {
    r.hasHolding = true;
    r.unrealizedPL = (r.lastClose - holding->avgPrice) * holding->qty;
    r.unrealizedPct = (r.lastClose - holding->avgPrice) / holding->avgPrice * 100.0;
    
    // override / adjust recommendation on extreme unrealized P/L
    if (r.unrealizedPct < -25.0 && r.score < 0.0) r.recommendation = Recommendation::Sell;
    if (r.unrealizedPct > 40.0 && r.score > 0.0) r.recommendation = Recommendation::Hold;
    
    // Suggest qty adjustments (same logic as before but include OBV/ADX confirmation)
    double riskFactor = std::clamp(r.atr / std::max(1.0, r.lastClose), 0.005, 0.05);
    if (r.recommendation == Recommendation::Sell || r.recommendation == Recommendation::StrongSell)
    {
      if (r.unrealizedPct > 15.0)
      {
        r.suggestedActionQty = -holding->qty * 0.5;
        r.actionReason = "Large unrealized gain (" + std::to_string(r.unrealizedPct) +
        "%). Suggest selling 50% to lock profits.";
      }
      else if (r.unrealizedPct < -10.0 || (r.obvSlope < 0 && r.adx > 20))
      {
        r.suggestedActionQty = -holding->qty * 0.75;
        r.actionReason = "Loss exceeding -10% or volume+trend weakening. Suggest cutting 75% of position.";
      }
      else
      {
        r.suggestedActionQty = -holding->qty * 0.25;
        r.actionReason = "Bearish technicals, suggest trimming 25% of position.";
      }
    }
    else if (r.recommendation == Recommendation::Buy || r.recommendation == Recommendation::StrongBuy)
    {
      double addRatio = (r.score > 0.8 ? 0.5 : 0.25) * (1.0 - riskFactor / 0.05);
      // require OBV confirmation for bigger add
      if (r.obvSlope > 0 && r.adx > 15) addRatio *= 1.2;
      r.suggestedActionQty = holding->qty * addRatio;
      r.actionReason = "Bullish signal (score " + std::to_string(r.score) +
      "). Suggest adding " + std::to_string((int)(addRatio * 100)) + "% to position.";
    }
    else
    {
      r.suggestedActionQty = 0.0;
      r.actionReason = "Neutral conditions — hold existing position.";
    }
  }
  else
  {
    // No holding — entry suggestion uses new indicators
    if (r.recommendation == Recommendation::Buy || r.recommendation == Recommendation::StrongBuy)
    {
      double baseLot = 100.0;
      double scale = (r.score > 0.8 ? 1.0 : 0.5);
      if (r.obvSlope > 0 && r.adx > 15) scale *= 1.2; // extra conviction
      r.suggestedActionQty = baseLot * scale;
      r.actionReason = "No existing holding. Entry opportunity detected (volume/trend confirmed).";
    }
  }
  
  // === Build explanation string (human readable) ===
  {
    std::ostringstream ss;
    ss.setf(std::ios::fixed); ss.precision(2);
    ss << "Score " << r.score << "; ";
    ss << "SMA" << m_cfg.sma_short << "=" << r.smaShort << ", RSI=" << r.rsi << ", ATR=" << r.atr << "; ";
    ss << "VWAP=" << r.vwap << ", OBV=" << r.obv << " (Δ=" << r.obvSlope << "), ADX=" << r.adx << "; ";
    ss << "Bollinger(mid=" << r.bollinger.middle << " up=" << r.bollinger.upper << " low=" << r.bollinger.lower << "); ";
    ss << "Stoch K/D=" << r.stochasticK << "/" << r.stochasticD << "; ";
    ss << "CCI=" << r.cci << ", ROC=" << r.roc << ", MFI=" << r.mfi << ". ";
    
    if (!r.candlePatterns.empty())
    {
      ss << "Candles: ";
      for (auto &p : r.candlePatterns) ss << p.name << "(" << p.strength << "), ";
    }
    if (!r.chartPatterns.empty())
    {
      ss << "Charts: ";
      for (auto &p : r.chartPatterns) ss << p.name << "(" << p.strength << "), ";
    }
    
    if (r.hasHolding)
    {
      ss << "Holding P/L " << r.unrealizedPL << " (" << r.unrealizedPct << "%). ";
      ss << "SuggestedActionQty=" << r.suggestedActionQty << ". Reason: " << r.actionReason;
    }
    else
    {
      if (r.suggestedActionQty != 0.0)
        ss << "Suggested entry qty: " << r.suggestedActionQty << ". Reason: " << r.actionReason;
    }
    
    r.explanation = ss.str();
  }
  
  return r;
}
