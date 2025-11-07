//
//  CandleSticks.cpp
//  KanVest
//
//  Created by Ashish . on 07/11/25.
//

#include "CandleSticks.hpp"

using namespace KanVest::Analysis::Candles;
using namespace KanVest::Analysis;
using namespace KanVest;

// Helper small functions
static double body(const StockPoint& p)
{
  return std::fabs(p.close - p.open);
}
static double upper_wick(const StockPoint& p)
{
  return p.high - std::max(p.open, p.close);
}
static double lower_wick(const StockPoint& p)
{
  return std::min(p.open, p.close) - p.low;
}

std::vector<PatternHit> DetectSingleBarPatterns(const std::vector<StockPoint>& history)
{
  std::vector<PatternHit> out;
  if (history.empty())
  {
    return out;
  }
  
  const StockPoint& last = history.back();
  double b = body(last);
  double candleRange = last.high - last.low;
  if (candleRange <= 0)
  {
    return out;
  }
  
  // Doji (body tiny relative to range)
  if (b / candleRange < 0.1)
  {
    PatternHit ph; ph.name = "Doji"; ph.startIndex = history.size()-1; ph.endIndex = history.size()-1;
    ph.strength = 0.8; ph.rationale = "Body << range";
    out.push_back(ph);
  }
  
  // Hammer / Hanging Man
  if (lower_wick(last) > 2.0 * b && upper_wick(last) < 0.25 * b)
  {
    PatternHit ph; ph.name = "Hammer/HangingMan"; ph.startIndex = history.size()-1; ph.endIndex = history.size()-1;
    ph.strength = 0.8; ph.rationale = "Big lower wick and small upper wick";
    out.push_back(ph);
  }
  
  // Shooting star / Inverted hammer
  if (upper_wick(last) > 2.0 * b && lower_wick(last) < 0.25 * b)
  {
    PatternHit ph; ph.name = "ShootingStar/InvertedHammer"; ph.startIndex = history.size()-1; ph.endIndex = history.size()-1;
    ph.strength = 0.8; ph.rationale = "Big upper wick and small lower wick";
    out.push_back(ph);
  }
  
  // Marubozu (no shadows)
  if (upper_wick(last) < 1e-6 && lower_wick(last) < 1e-6)
  {
    PatternHit ph; ph.name = "Marubozu"; ph.startIndex = history.size()-1; ph.endIndex = history.size()-1;
    ph.strength = 0.9; ph.rationale = "No wicks";
    out.push_back(ph);
  }
  
  // Add other single-bar heuristics as needed.
  
  return out;
}

std::vector<PatternHit> DetectMultiBarPatterns(const std::vector<StockPoint>& history)
{
  std::vector<PatternHit> out;
  if (history.size() < 3)
  {
    return out;
  }
  
  size_t n = history.size();
  // Bullish Engulfing
  {
    const auto &a = history[n-2], &b = history[n-1];
    if ( (a.close < a.open) && (b.close > b.open) )
    {
      // b body engulfs a body
      double aBody = std::fabs(a.close - a.open);
      double bBody = std::fabs(b.close - b.open);
      if (bBody > aBody && b.close > a.open && b.open < a.close)
      {
        PatternHit ph; ph.name = "Bullish Engulfing"; ph.startIndex = n-2; ph.endIndex = n-1; ph.strength = 0.85;
        ph.rationale = "Green candle engulfs prior red";
        out.push_back(ph);
      }
    }
  }
  // Bearish Engulfing
  {
    const auto &a = history[n-2], &b = history[n-1];
    if ((a.close > a.open) && (b.close < b.open))
    {
      double aBody = std::fabs(a.close - a.open);
      double bBody = std::fabs(b.close - b.open);
      if (bBody > aBody && b.open > a.close && b.close < a.open)
      {
        PatternHit ph; ph.name = "Bearish Engulfing"; ph.startIndex = n-2; ph.endIndex = n-1; ph.strength = 0.85;
        ph.rationale = "Red candle engulfs prior green";
        out.push_back(ph);
      }
    }
  }
  
  // Morning/Evening Star (3 bar)
  if (history.size() >= 3)
  {
    const auto &p1 = history[n-3], &p2 = history[n-2], &p3 = history[n-1];
    // morning star: p1 large red, p2 small body gap down, p3 large green closing > mid p1
    if ((p1.close < p1.open) && (std::fabs(p2.close - p2.open) < 0.4 * std::fabs(p1.close - p1.open))
        && (p3.close > p3.open) && (p3.close > (p1.open + p1.close)/2.0))
    {
      PatternHit ph; ph.name = "Morning Star"; ph.startIndex = n-3; ph.endIndex = n-1; ph.strength = 0.8;
      ph.rationale = "3-bar reversal - large red, small indecision, large green";
      out.push_back(ph);
    }
    // evening star (opposite)
    if ((p1.close > p1.open) && (std::fabs(p2.close - p2.open) < 0.4 * std::fabs(p1.close - p1.open))
        && (p3.close < p3.open) && (p3.close < (p1.open + p1.close)/2.0))
    {
      PatternHit ph; ph.name = "Evening Star"; ph.startIndex = n-3; ph.endIndex = n-1; ph.strength = 0.8;
      ph.rationale = "3-bar reversal - large green, small indecision, large red";
      out.push_back(ph);
    }
  }
  
  // Add more multi-bar patterns (Tweezers, Three soldiers/crows, Harami) similarly.
  
  return out;
}
