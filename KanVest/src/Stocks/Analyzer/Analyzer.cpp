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
  auto highs = highsFrom(s.history);
  auto lows = lowsFrom(s.history);
  auto vols = volsFrom(s.history);
  
  r.lastClose = closes.empty() ? 0.0 : closes.back();
  r.smaShort = SMA(closes, m_cfg.sma_short);
  r.smaLong = SMA(closes, m_cfg.sma_long);
  r.rsi = RSI(closes, m_cfg.rsi_period);
  auto macd_pair = MACD(closes);
  r.macd = macd_pair.first; r.macdSignal = macd_pair.second;
  r.atr = ATR(highs, lows, closes, m_cfg.atr_period);
  
  r.candlePatterns = DetectSingleBarPatterns(s.history);
  auto multi = DetectMultiBarPatterns(s.history);
  r.candlePatterns.insert(r.candlePatterns.end(), multi.begin(), multi.end());
  
  r.chartPatterns = DetectDoubleTopBottom(s.history);
  auto hs = DetectHeadAndShoulders(s.history);
  r.chartPatterns.insert(r.chartPatterns.end(), hs.begin(), hs.end());
  
  // Compose score
  double score = 0.0;
  if (!std::isnan(r.macd) && !std::isnan(r.macdSignal))
  {
    score += (r.macd > r.macdSignal) ? 0.25 : -0.25;
  }
  if (!std::isnan(r.rsi))
  {
    if (r.rsi < 30)
    {
      score += 0.2;
    }
    else if (r.rsi > 70)
    {
      score -= 0.2;
    }
  }
  if (!std::isnan(r.smaShort) && !std::isnan(r.smaLong))
  {
    if (r.smaShort > r.smaLong)
    {
      score += 0.15;
    }
    else
    {
      score -= 0.15;
    }
  }
  // patterns adjust score
  for (auto &p : r.candlePatterns)
  {
    if (p.name.find("Bull")!=std::string::npos || p.name.find("Bullish")!=std::string::npos)
    {
      score += 0.12;
    }
    if (p.name.find("Bear")!=std::string::npos || p.name.find("Bearish")!=std::string::npos)
    {
      score -= 0.12;
    }
  }
  for (auto &p : r.chartPatterns)
  {
    if (p.name == "DoubleBottom" || p.name=="InverseHeadAndShoulders")
    {
      score += 0.25;
    }
    if (p.name == "DoubleTop" || p.name == "HeadAndShoulders")
    {
      score -= 0.25;
    }
  }
  
  // long-term confirmation
  if (l && l->IsValid())
  {
    auto lcloses = closesFrom(l->history);
    double lSma = SMA(lcloses, 30);
    double lEma = EMA(lcloses, 30);
    if (!std::isnan(lSma) && !std::isnan(lEma))
    {
      score += (lEma > lSma) ? 0.2 : -0.2;
    }
  }
  
  // volatility dampening
  if (!std::isnan(r.atr) && r.lastClose > 0)
  {
    double atrPct = r.atr / r.lastClose;
    if (atrPct > 0.04)
    {
      score *= 0.85;
    }
  }
  
  // bound score & map to recommendation
  r.score = std::max(-1.0, std::min(1.0, score));
  if (r.score >= 0.6)
  {
    r.recommendation = Recommendation::StrongBuy;
  }
  else if (r.score >= 0.2)
  {
    r.recommendation = Recommendation::Buy;
  }
  else if (r.score >= -0.2)
  {
    r.recommendation = Recommendation::Hold;
  }
  else if (r.score >= -0.6)
  {
    r.recommendation = Recommendation::Sell;
  }
  else
  {
    r.recommendation = Recommendation::StrongSell;
  }
  
  // Holding adjustments
  if (holding && holding->qty > 0.0 && holding->avgPrice > 0.0)
  {
    r.hasHolding = true;
    r.unrealizedPL = (r.lastClose - holding->avgPrice) * holding->qty;
    r.unrealizedPct = (r.lastClose - holding->avgPrice) / holding->avgPrice * 100.0;
    // heuristics
    if (r.unrealizedPct < -25.0 && r.score < 0.0)
    {
      r.recommendation = Recommendation::Sell;
    }
    if (r.unrealizedPct > 40.0 && r.score > 0.0)
    {
      r.recommendation = Recommendation::Hold; // take partial profits suggestion in explanation
    }
  }
  
  // build explanation string
  r.explanation = "Score " + std::to_string(r.score) + "; ";
  r.explanation += "SMA" + std::to_string(m_cfg.sma_short) + "=" + std::to_string(r.smaShort) + ", ";
  r.explanation += "RSI=" + std::to_string(r.rsi) + ". ";
  // append detected patterns
  if (!r.candlePatterns.empty())
  {
    r.explanation += "Candlestick patterns: ";
    for (auto &p : r.candlePatterns)
    {
      r.explanation += p.name + "(" + std::to_string(p.strength) + "), ";
    }
  }
  if (!r.chartPatterns.empty())
  {
    r.explanation += "Chart patterns: ";
    for (auto &p : r.chartPatterns)
    {
      r.explanation += p.name + "(" + std::to_string(p.strength) + "), ";
    }
  }
  return r;
}
