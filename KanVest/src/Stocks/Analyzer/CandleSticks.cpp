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

std::vector<PatternHit> Candles::DetectSingleBarPatterns(const std::vector<StockPoint>& history)
{
  std::vector<PatternHit> out;
  if (history.empty())
  {
    return out;
  }
  
  const StockPoint& c = history.back();
  double b = std::fabs(c.close - c.open);
  double candleRange = c.high - c.low;
  if (candleRange <= 0.0)
  {
    return out;
  }
  
  double upper = c.high - std::max(c.close, c.open);
  double lower = std::min(c.close, c.open) - c.low;
  double bodyMid = (c.open + c.close) * 0.5;
  
  auto add = [&](std::string name, double s, std::string why)
  {
    PatternHit ph;
    ph.name = name;
    ph.startIndex = history.size() - 1;
    ph.endIndex = history.size() - 1;
    ph.strength = s;
    ph.rationale = why;
    out.push_back(ph);
  };
  
  bool bullish = c.close > c.open;
  bool bearish = c.close < c.open;
  
  // -----------------------------------------------------------
  // DOJI FAMILY
  // -----------------------------------------------------------
  double bodyRatio = b / candleRange;
  if (bodyRatio < 0.1)
  {
    add("Doji", 0.8, "Small body relative to total range");
    // subtypes
    if (upper < 0.1 * candleRange && lower > 0.7 * candleRange)
      add("Dragonfly Doji", 0.9, "Long lower shadow, open=close near high");
    else if (lower < 0.1 * candleRange && upper > 0.7 * candleRange)
      add("Gravestone Doji", 0.9, "Long upper shadow, open=close near low");
    else if (upper > 0.4 * candleRange && lower > 0.4 * candleRange)
      add("Long-Legged Doji", 0.9, "Long upper and lower shadows, indecision");
  }
  
  // -----------------------------------------------------------
  // SPINNING TOP (indecision)
  // -----------------------------------------------------------
  if (bodyRatio > 0.1 && bodyRatio < 0.4 && upper > 0.25 * candleRange && lower > 0.25 * candleRange)
    add("Spinning Top", 0.7, "Moderate body, long wicks both sides");
  
  // -----------------------------------------------------------
  // MARUBOZU FAMILY
  // -----------------------------------------------------------
  if (upper < 0.05 * candleRange && lower < 0.05 * candleRange)
  {
    if (bullish)
      add("Bullish Marubozu", 0.9, "Long body, no shadows (strong buying)");
    else if (bearish)
      add("Bearish Marubozu", 0.9, "Long body, no shadows (strong selling)");
  }
  
  // -----------------------------------------------------------
  // HAMMER FAMILY
  // -----------------------------------------------------------
  if (lower >= 2.0 * b && upper <= 0.25 * b)
  {
    if (bullish)
      add("Hammer", 0.9, "Long lower wick, small upper wick — bullish reversal");
    else
      add("Hanging Man", 0.8, "Long lower wick after rally — possible bearish reversal");
  }
  
  // -----------------------------------------------------------
  // INVERTED HAMMER / SHOOTING STAR
  // -----------------------------------------------------------
  if (upper >= 2.0 * b && lower <= 0.25 * b)
  {
    if (bullish)
      add("Inverted Hammer", 0.85, "Long upper wick, small body — bullish reversal after downtrend");
    else
      add("Shooting Star", 0.85, "Long upper wick after uptrend — bearish reversal");
  }
  
  // -----------------------------------------------------------
  // BELT HOLD (MARUBOZU OPEN)
  // -----------------------------------------------------------
  if (bullish && lower < 0.05 * candleRange && upper > 0.3 * candleRange && b / candleRange > 0.6)
    add("Bullish Belt Hold", 0.85, "No lower shadow, opens low, closes high");
  if (bearish && upper < 0.05 * candleRange && lower > 0.3 * candleRange && b / candleRange > 0.6)
    add("Bearish Belt Hold", 0.85, "No upper shadow, opens high, closes low");
  
  // -----------------------------------------------------------
  // HIGH WAVE CANDLE (strong indecision, volatile)
  // -----------------------------------------------------------
  if (upper > 0.4 * candleRange && lower > 0.4 * candleRange && bodyRatio < 0.3)
    add("High Wave Candle", 0.7, "Long upper/lower shadows, small body — uncertainty");
  
  // -----------------------------------------------------------
  // SHAVEN HEAD / SHAVEN BOTTOM (no upper/lower shadow)
  // -----------------------------------------------------------
  if (upper < 0.05 * candleRange && lower > 0.2 * candleRange)
    add("Shaven Head", 0.75, "No upper shadow");
  if (lower < 0.05 * candleRange && upper > 0.2 * candleRange)
    add("Shaven Bottom", 0.75, "No lower shadow");
  
  // -----------------------------------------------------------
  // LONG DAY / SHORT DAY
  // -----------------------------------------------------------
  if (bodyRatio > 0.6)
    add(bullish ? "Long Bullish Candle" : "Long Bearish Candle", 0.7, "Large real body, strong directional move");
  else if (bodyRatio < 0.25)
    add("Short Candle", 0.6, "Small real body, low momentum");
  
  return out;
}

std::vector<PatternHit> Candles::DetectMultiBarPatterns(const std::vector<StockPoint>& history)
{
  std::vector<PatternHit> out;
  size_t n = history.size();
  if (n < 2)
    return out;
  
  auto body = [](const StockPoint& p) { return std::fabs(p.close - p.open); };
  auto isBull = [](const StockPoint& p) { return p.close > p.open; };
  auto isBear = [](const StockPoint& p) { return p.close < p.open; };
  auto midpoint = [](const StockPoint& p) { return (p.open + p.close) * 0.5; };
  
  // --- 2-bar patterns ---
  const auto& a = history[n-2];
  const auto& b = history[n-1];
  
  double aBody = body(a);
  double bBody = body(b);
  
  // Bullish Engulfing
  if (isBear(a) && isBull(b) && b.open < a.close && b.close > a.open && bBody > aBody)
  {
    out.push_back({"Bullish Engulfing", n-2, n-1, 0.9, "Green candle fully engulfs red body"});
  }
  // Bearish Engulfing
  if (isBull(a) && isBear(b) && b.open > a.close && b.close < a.open && bBody > aBody)
  {
    out.push_back({"Bearish Engulfing", n-2, n-1, 0.9, "Red candle fully engulfs green body"});
  }
  
  // Bullish Harami (small green inside large red)
  if (isBear(a) && isBull(b) && b.open > b.close && aBody > 2.0 * bBody &&
      b.open > a.close && b.close < a.open)
  {
    out.push_back({"Bullish Harami", n-2, n-1, 0.8, "Small green inside large red body"});
  }
  
  // Bearish Harami
  if (isBull(a) && isBear(b) && aBody > 2.0 * bBody &&
      b.open < a.close && b.close > a.open)
  {
    out.push_back({"Bearish Harami", n-2, n-1, 0.8, "Small red inside large green body"});
  }
  
  // Harami Cross (small doji inside)
  if (aBody > 2.0 * bBody && std::fabs(b.close - b.open) < 0.1 * (b.high - b.low))
  {
    std::string dir = isBear(a) ? "Bullish Harami Cross" : "Bearish Harami Cross";
    out.push_back({dir, n-2, n-1, 0.8, "Doji within previous body"});
  }
  
  // Tweezer Top / Bottom
  if (std::fabs(a.high - b.high) < 0.002 * a.high && isBull(a) && isBear(b))
  {
    out.push_back({"Tweezer Top", n-2, n-1, 0.75, "Equal highs, potential reversal"});
  }
  if (std::fabs(a.low - b.low) < 0.002 * a.low && isBear(a) && isBull(b))
  {
    out.push_back({"Tweezer Bottom", n-2, n-1, 0.75, "Equal lows, potential reversal"});
  }
  
  // Piercing Pattern
  if (isBear(a) && isBull(b) && b.open < a.low && b.close > midpoint(a) && b.close < a.open)
  {
    out.push_back({"Piercing Pattern", n-2, n-1, 0.85, "Gap down green closing > 50% into red"});
  }
  
  // Dark Cloud Cover
  if (isBull(a) && isBear(b) && b.open > a.high && b.close < midpoint(a) && b.close > a.open)
  {
    out.push_back({"Dark Cloud Cover", n-2, n-1, 0.85, "Gap up red closing below midpoint"});
  }
  
  // Kicking (Marubozu gap)
  if (aBody > 0.9 * (a.high - a.low) && bBody > 0.9 * (b.high - b.low))
  {
    if (isBear(a) && isBull(b) && b.open > a.high)
      out.push_back({"Kicking (Bullish)", n-2, n-1, 0.9, "Bullish marubozu gap up"});
    else if (isBull(a) && isBear(b) && b.open < a.low)
      out.push_back({"Kicking (Bearish)", n-2, n-1, 0.9, "Bearish marubozu gap down"});
  }
  
  // --- 3-bar patterns ---
  if (n >= 3)
  {
    const auto& p1 = history[n-3];
    const auto& p2 = history[n-2];
    const auto& p3 = history[n-1];
    
    // Morning Star
    if (isBear(p1) && std::fabs(p2.close - p2.open) < 0.3 * body(p1)
        && isBull(p3) && p3.close > midpoint(p1))
    {
      out.push_back({"Morning Star", n-3, n-1, 0.9, "3-bar bullish reversal"});
    }
    
    // Evening Star
    if (isBull(p1) && std::fabs(p2.close - p2.open) < 0.3 * body(p1)
        && isBear(p3) && p3.close < midpoint(p1))
    {
      out.push_back({"Evening Star", n-3, n-1, 0.9, "3-bar bearish reversal"});
    }
    
    // Three White Soldiers
    if (isBull(p1) && isBull(p2) && isBull(p3) &&
        p2.open > midpoint(p1) && p3.open > midpoint(p2))
    {
      out.push_back({"Three White Soldiers", n-3, n-1, 0.95, "Strong bullish continuation"});
    }
    
    // Three Black Crows
    if (isBear(p1) && isBear(p2) && isBear(p3) &&
        p2.open < midpoint(p1) && p3.open < midpoint(p2))
    {
      out.push_back({"Three Black Crows", n-3, n-1, 0.95, "Strong bearish continuation"});
    }
    
    // Rising Three Methods (bullish continuation)
    if (n >= 5)
    {
      const auto& c1 = history[n-5];
      const auto& c2 = history[n-4];
      const auto& c3 = history[n-3];
      const auto& c4 = history[n-2];
      const auto& c5 = history[n-1];
      
      if (isBull(c1) && isBull(c5) &&
          isBear(c2) && isBear(c3) && isBear(c4) &&
          c2.high < c1.high && c4.low > c1.low && c5.close > c1.close)
      {
        out.push_back({"Rising Three Methods", n-5, n-1, 0.9, "Bullish continuation pattern"});
      }
      
      // Falling Three Methods
      if (isBear(c1) && isBear(c5) &&
          isBull(c2) && isBull(c3) && isBull(c4) &&
          c2.low > c1.low && c4.high < c1.high && c5.close < c1.close)
      {
        out.push_back({"Falling Three Methods", n-5, n-1, 0.9, "Bearish continuation pattern"});
      }
    }
  }
  
  // Inside Bar / Outside Bar
  if (b.high < a.high && b.low > a.low)
    out.push_back({"Inside Bar", n-2, n-1, 0.7, "Range inside previous bar"});
  if (b.high > a.high && b.low < a.low)
    out.push_back({"Outside Bar", n-2, n-1, 0.7, "Engulfing range, volatility expansion"});
  
  return out;
}
