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

// helpers for StockPoint geometry
static double body(const StockPoint& p) { return std::fabs(p.close - p.open); }
static double candleRange(const StockPoint& p) { return std::max(1e-12, p.high - p.low); }
static double upper_wick(const StockPoint& p) { return p.high - std::max(p.open, p.close); }
static double lower_wick(const StockPoint& p) { return std::min(p.open, p.close) - p.low; }
static double midpoint(const StockPoint& p) { return (p.open + p.close) * 0.5; }

// Build PatternHit quickly for single bar
static PatternHit MakeSingle(const std::string& name, size_t idx, double strength, const std::string& rationale) {
  PatternHit ph; ph.name = name; ph.startIndex = idx; ph.endIndex = idx; ph.strength = std::max(0.0, std::min(1.0, strength)); ph.rationale = rationale; ph.rationale = PatternRationale::Describe(ph.name);
 return ph;
}

// Build PatternHit for ranges
static PatternHit MakeRange(const std::string& name, size_t s, size_t e, double strength, const std::string& rationale) {
  PatternHit ph; ph.name = name; ph.startIndex = s; ph.endIndex = e; ph.strength = std::max(0.0, std::min(1.0, strength)); ph.rationale = rationale; ph.rationale = PatternRationale::Describe(ph.name);
  return ph;
}

// Utility: safe subtraction for size_t
[[maybe_unused]] static size_t SafeSub(size_t a, size_t b) { return (a > b) ? (a - b) : 0; }

// single-bar comprehensive detection
std::vector<PatternHit> Candles::DetectSingleBarPatterns(const std::vector<StockPoint>& history)
{
  std::vector<PatternHit> out;
  if (history.empty()) return out;
  
  size_t idx = history.size() - 1;
  const StockPoint& c = history.back();
  double b = body(c);
  double range = candleRange(c);
  if (range <= 0.0) return out;
  
  double up = upper_wick(c);
  double low = lower_wick(c);
  double bodyRatio = b / range;
  bool bull = c.close > c.open;
  bool bear = c.close < c.open;
  
  auto add = [&](const std::string &n, double s, const std::string &r) {
    out.push_back(MakeSingle(n, idx, s, r));
  };
  
  // ----------------- DOJI family -----------------
  if (bodyRatio < 0.08) {
    add("Doji", 0.9, "Tiny real body relative to range");
    // subtypes by shadows
    if (up < 0.08*range && low > 0.6*range) add("Dragonfly Doji", 0.95, "Open=Close near high; long lower shadow");
    else if (low < 0.08*range && up > 0.6*range) add("Gravestone Doji", 0.95, "Open=Close near low; long upper shadow");
    else if (up > 0.35*range && low > 0.35*range) add("Long-Legged Doji", 0.95, "Long upper & lower shadows");
  }
  
  // ----------------- Spinning / High-wave -----------------
  if (bodyRatio >= 0.08 && bodyRatio <= 0.35 && up > 0.25*range && low > 0.25*range) {
    add("Spinning Top", 0.75, "Small body with long wicks both sides (indecision)");
  }
  if (up > 0.4*range && low > 0.4*range && bodyRatio < 0.25) {
    add("High Wave Candle", 0.72, "Very long shadows & small body (volatile indecision)");
  }
  
  // ----------------- Marubozu family -----------------
  // Opening/Closing marubozu variants: no shadows or tiny shadows
  if (up < 0.02*range && low < 0.02*range && bodyRatio > 0.75) {
    add(bull ? "Bullish Marubozu" : "Bearish Marubozu", 0.98, "Long body and negligible shadows (strong momentum)");
  } else {
    // Opening Marubozu: no upper shadow for bearish? define approx:
    if (bull && low < 0.02*range && bodyRatio > 0.6) add("Bullish Opening Marubozu", 0.85, "Opened near low and closed strong");
    if (bear && up < 0.02*range && bodyRatio > 0.6) add("Bearish Opening Marubozu", 0.85, "Opened near high and closed weak");
    // Closing Marubozu: no lower shadow
    if (bull && up < 0.02*range && bodyRatio > 0.6) add("Bullish Closing Marubozu", 0.85, "Closed at/near high after rally");
    if (bear && low < 0.02*range && bodyRatio > 0.6) add("Bearish Closing Marubozu", 0.85, "Closed at/near low after drop");
  }
  
  // ----------------- Hammer family -----------------
  if (low > 2.0 * b && up < 0.25 * b) {
    if (bull) add("Hammer", 0.92, "Long lower shadow, small body near top (bullish reversal)");
    else add("Hanging Man", 0.85, "Long lower shadow after an up-move (possible bearish reversal)");
  }
  
  // Inverted hammer / Shooting Star
  if (up > 2.0 * b && low < 0.25 * b) {
    if (bull) add("Inverted Hammer", 0.88, "Long upper wick after downtrend (bullish reversal)");
    else add("Shooting Star", 0.88, "Long upper wick after uptrend (bearish reversal)");
  }
  
  // ----------------- Belt Hold -----------------
  if (bull && low < 0.05*range && bodyRatio > 0.6) add("Bullish Belt Hold", 0.86, "Opened at low and closed strongly (no lower shadow)");
  if (bear && up < 0.05*range && bodyRatio > 0.6) add("Bearish Belt Hold", 0.86, "Opened at high and closed weakly (no upper shadow)");
  
  // ----------------- Shaven head / bottom -----------------
  if (up < 0.05*range && low > 0.2*range) add("Shaven Head", 0.7, "Little/no upper shadow");
  if (low < 0.05*range && up > 0.2*range) add("Shaven Bottom", 0.7, "Little/no lower shadow");
  
  // ----------------- Long/Short Days -----------------
  if (bodyRatio > 0.6) add(bull ? "Long Bullish Candle" : "Long Bearish Candle", 0.75, "Large real body (strong directional day)");
  if (bodyRatio < 0.12) add("Short Candle", 0.6, "Tiny body - low directional conviction");
  
  // ----------------- Long Shadow variants -----------------
  if (low > 3.0 * b && up < 0.5 * b) add("Long Lower Shadow Candle", 0.7, "Very long lower wick vs body");
  if (up > 3.0 * b && low < 0.5 * b) add("Long Upper Shadow Candle", 0.7, "Very long upper wick vs body");
  
  // ----------------- Kicking (Marubozu gap) -----------------
  if (history.size() >= 2) {
    const StockPoint& prev = history[history.size()-2];
    double prevRange = candleRange(prev);
    double prevBody = body(prev);
    // bullish kicking: prev bearish marubozu and current bullish marubozu with gap
    if ((prev.high < c.low) && (prevBody > 0.8*prevRange) && (bodyRatio > 0.8)) {
      if (bull) add("Kicking Bullish", 0.95, "Strong gap up with marubozu-like candles - bullish reversal/continuation");
      else add("Kicking Bearish", 0.95, "Strong gap down with marubozu-like candles - bearish");
    }
  }
  
  // ----------------- Counterattack Lines -----------------
  if (history.size() >= 2) {
    const StockPoint& prev = history[history.size()-2];
    // bullish counterattack: prev bearish day followed by bullish day closing near prev close
    if ((prev.close < prev.open) && (bull) && (std::fabs(c.close - prev.close) / std::max(1e-9, prev.close) < 0.015)) {
      add("Bullish Counterattack", 0.78, "Green candle closes near prior close after red day - potential reversal");
    }
    // bearish counterattack
    if ((prev.close > prev.open) && (bear) && (std::fabs(c.close - prev.close) / std::max(1e-9, prev.close) < 0.015)) {
      add("Bearish Counterattack", 0.78, "Red candle closes near prior close after green day - potential reversal");
    }
  }
  
  // ----------------- Stick Sandwich / Matching Low/High / On Neck/In Neck/Thrusting -----------------
  if (history.size() >= 3) {
    // Stick Sandwich: green between two reds with close similar to first red's close
    if (history.size() >= 3) {
      const StockPoint &p2 = history[history.size()-3], &p1 = history[history.size()-2], &p0 = history[history.size()-1];
      if ((p2.close < p2.open) && (p1.close > p1.open) && (p0.close < p0.open) &&
          (std::fabs(p0.close - p2.close) / std::max(1e-9, p2.close) < 0.02)) {
        add("Stick Sandwich", 0.75, "Bear-Green-Bear with close similar to first red - bullish sign");
      }
    }
    // Matching Low/High
    const StockPoint &pA = history[history.size()-2];
    if (std::fabs(pA.low - c.low) / std::max(1e-9, pA.low) < 0.01) add("Matching Low", 0.65, "Lows match over two bars - potential support");
    if (std::fabs(pA.high - c.high) / std::max(1e-9, pA.high) < 0.01) add("Matching High", 0.65, "Highs match over two bars - potential resistance");
  }
  
  // ----------------- Deliberation / Thrusting -----------------
  if (history.size() >= 3) {
    const StockPoint &p2 = history[history.size()-3], &p1 = history[history.size()-2];
    // Deliberation: 4-bar variation often after uptrend; detect roughly
    // Thrusting: red day opens above prior green close and closes inside prior body but not fully reversing
    if ((p2.close > p2.open) && (p1.close < p1.open) && (c.close > c.open) && (c.open > p1.close) && (c.close < p1.close)) {
      add("Thrusting Pattern", 0.72, "Partial penetration of prior candle's body - continuation");
    }
  }
  
  // ----------------- Tri-Star / Abandoned Baby (rare multi-doji) -----------------
  if (history.size() >= 3) {
    const StockPoint &p2 = history[history.size()-3], &p1 = history[history.size()-2], &p0 = history[history.size()-1];
    double br2 = body(p2)/candleRange(p2), br1 = body(p1)/candleRange(p1), br0 = body(p0)/candleRange(p0);
    if (br2 < 0.08 && br1 < 0.08 && br0 < 0.08) {
      add("Tri-Star", 0.95, "Three consecutive doji - strong indecision/possible reversal");
    }
    // Abandoned Baby: doji gaps isolated
    if (br1 < 0.08 && (p2.high < p1.low) && (p0.low > p1.high)) {
      add("Abandoned Baby", 0.9, "Doji isolated by gaps - reversal signal");
    }
  }
  
  // ----------------- Tasuki Gaps (Upside/Downside) -----------------
  if (history.size() >= 3) {
    const StockPoint &p2 = history[history.size()-3], &p1 = history[history.size()-2], &p0 = history[history.size()-1];
    // Upside tasuki: strong up gap, then small red inside gap not closing gap
    if ((p2.close < p2.open) && (p1.close > p1.open) && (p0.close < p0.open)) {
      if (p1.low > p2.high && p0.close > p2.high && p0.close < p1.close) add("Upside Tasuki Gap", 0.78, "Gap up continuation pattern");
    }
    // Downside tasuki
    if ((p2.close > p2.open) && (p1.close < p1.open) && (p0.close > p0.open)) {
      if (p1.high < p2.low && p0.close < p2.low && p0.close > p1.close) add("Downside Tasuki Gap", 0.78, "Gap down continuation pattern");
    }
  }
  
  return out;
}

// multi-bar comprehensive detection
std::vector<PatternHit> Candles::DetectMultiBarPatterns(const std::vector<StockPoint>& history)
{
  std::vector<PatternHit> out;
  size_t n = history.size();
  if (n < 2) return out;
  
  auto addRange = [&](const std::string &name, size_t s, size_t e, double strength, const std::string &r) {
    out.push_back(MakeRange(name, s, e, strength, r));
  };
  
  // Prepare pointers to last indices safely
  if (n >= 2) {
    const StockPoint &a = history[n-2], &b = history[n-1];
    double aBody = body(a), bBody = body(b);
    bool aBull = a.close > a.open, bBull = b.close > b.open;
    bool aBear = a.close < a.open, bBear = b.close < b.open;
    
    // Engulfing
    if (aBear && bBull && b.open < a.close && b.close > a.open && bBody > aBody)
      addRange("Bullish Engulfing", n-2, n-1, 0.94, "Green engulfs previous red");
    if (aBull && bBear && b.open > a.close && b.close < a.open && bBody > aBody)
      addRange("Bearish Engulfing", n-2, n-1, 0.94, "Red engulfs previous green");
    
    // Harami / Harami Cross
    if (((aBear && bBull) || (aBull && bBear)) && bBody * 2.0 < aBody) {
      addRange("Harami", n-2, n-1, 0.80, "Small body inside larger previous body");
      if (std::fabs(b.close - b.open) / (b.high - b.low + 1e-12) < 0.12) addRange("Harami Cross", n-2, n-1, 0.85, "Doji inside prior body");
    }
    
    // Piercing / Dark Cloud (two-bar)
    if (aBear && bBull && b.open < a.low && b.close > midpoint(a)) addRange("Piercing", n-2, n-1, 0.86, "Gap down then >50% recovery into prior body");
    if (aBull && bBear && b.open > a.high && b.close < midpoint(a)) addRange("Dark Cloud Cover", n-2, n-1, 0.86, "Gap up then >50% fill into prior body");
    
    // Tweezer Tops/Bottoms (approx equal highs/lows)
    if (std::fabs(a.high - b.high) / std::max(1e-9, a.high) < 0.002 && aBull && bBear) addRange("Tweezer Top", n-2, n-1, 0.78, "Equal highs over two bars");
    if (std::fabs(a.low - b.low) / std::max(1e-9, a.low) < 0.002 && aBear && bBull) addRange("Tweezer Bottom", n-2, n-1, 0.78, "Equal lows over two bars");
    
    // Inside / Outside bars
    if (b.high < a.high && b.low > a.low) addRange("Inside Bar", n-2, n-1, 0.72, "Range within previous bar (consolidation)");
    if (b.high > a.high && b.low < a.low) addRange("Outside Bar", n-2, n-1, 0.72, "Range engulfs previous bar (volatility expansion)");
  }
  
  // Three-bar patterns
  if (n >= 3) {
    const StockPoint &p1 = history[n-3], &p2 = history[n-2], &p3 = history[n-1];
    
    // Morning/Evening Star variants and Abandoned Baby (if gaps)
    if (p1.close < p1.open && std::fabs(p2.close - p2.open) < 0.4 * body(p1) && p3.close > p3.open && p3.close > midpoint(p1))
      addRange("Morning Star", n-3, n-1, 0.9, "Three-bar bullish reversal");
    if (p1.close > p1.open && std::fabs(p2.close - p2.open) < 0.4 * body(p1) && p3.close < p3.open && p3.close < midpoint(p1))
      addRange("Evening Star", n-3, n-1, 0.9, "Three-bar bearish reversal");
    
    // Three inside / outside patterns
    // Three Inside Up: bearish first, small body inside, then bullish close above first open
    if (p1.close < p1.open && body(p2) < body(p1) && p3.close > p3.open && p3.close > p1.open)
      addRange("Three Inside Up", n-3, n-1, 0.82, "Bullish reversal variant");
    if (p1.close > p1.open && body(p2) < body(p1) && p3.close < p3.open && p3.close < p1.open)
      addRange("Three Inside Down", n-3, n-1, 0.82, "Bearish reversal variant");
    
    // Three Outside Up / Down (engulfing then follow-through)
    if (p1.close < p1.open && p2.close > p2.open && body(p2) > body(p1) && p3.close > p3.open)
      addRange("Three Outside Up", n-3, n-1, 0.86, "Bullish engulfing then continuation");
    if (p1.close > p1.open && p2.close < p2.open && body(p2) > body(p1) && p3.close < p3.open)
      addRange("Three Outside Down", n-3, n-1, 0.86, "Bearish engulfing then continuation");
    
    // Piercing is already added as two-bar; also detect 'On Neck', 'In Neck', 'Thrusting' - partial penetrations
    if (p1.close > p1.open && p2.close < p2.open && p3.close > p3.open && p3.close < p1.close && p3.close > p2.close)
      addRange("On Neck / In Neck / Thrusting", n-3, n-1, 0.7, "Partial penetration patterns - continuation/weak reversal cues");
    
    // Tri-Star (three doji) handled in single-bar tri-star detection but we can also add as multi-bar
    double br1 = body(p1)/candleRange(p1), br2 = body(p2)/candleRange(p2), br3 = body(p3)/candleRange(p3);
    if (br1 < 0.08 && br2 < 0.08 && br3 < 0.08) addRange("Tri-Star (3 Doji)", n-3, n-1, 0.95, "Three doji in a row - extreme indecision");
  }
  
  // Five-bar patterns: Three Methods (rising/falling) + Mat Hold, Ladder Bottom/Top
  if (n >= 5) {
    const StockPoint &c1 = history[n-5], &c2 = history[n-4], &c3 = history[n-3], &c4 = history[n-2], &c5 = history[n-1];
    
    // Rising Three Methods (bullish continuation)
    if (c1.close > c1.open && c5.close > c5.open &&
        c2.close < c2.open && c3.close < c3.open && c4.close < c4.open &&
        c2.open > c1.open && c3.open > c2.open && c4.open > c3.open)
      addRange("Rising Three Methods", n-5, n-1, 0.9, "Bullish continuation pattern");
    
    // Falling Three Methods (bearish)
    if (c1.close < c1.open && c5.close < c5.open &&
        c2.close > c2.open && c3.close > c3.open && c4.close > c4.open &&
        c2.open < c1.open && c3.open < c2.open && c4.open < c3.open)
      addRange("Falling Three Methods", n-5, n-1, 0.9, "Bearish continuation pattern");
    
    // Mat Hold - bullish variant: strong bull, small counter pullbacks, then continuation
    bool matHold = (c1.close > c1.open) && (c2.close < c2.open) && (c3.close < c3.open) && (c4.close > c4.open) && (c5.close > c5.open)
    && (body(c2) < 0.5*body(c1)) && (body(c3) < 0.5*body(c1));
    if (matHold) addRange("Mat Hold", n-5, n-1, 0.78, "Bullish Mat Hold variation - continuation");
    
    // Ladder Bottom / Ladder Top (rare, multi-bar)
    // Ladder Bottom: downtrend then ascending small candles then large bullish reversal
    bool ladderBottom = (c1.close < c1.open && c2.close < c2.open && c3.close < c3.open && c4.close > c4.open && c5.close > c5.open
                         && c5.close > c4.close && c4.close > c3.close);
    if (ladderBottom) addRange("Ladder Bottom", n-5, n-1, 0.75, "Multi-bar reversal - Ladder Bottom");
  }
  
  // Patterns that reference pivots across a longer span (Tweezers, Abandoned Baby, etc.) often already detected above; duplicates okay.
  
  // Additional multi-bar: Abandoned Baby as 3-bar, already in single; ensure it's captured:
  if (n >= 3) {
    const StockPoint &p2 = history[n-3], &p1 = history[n-2], &p0 = history[n-1];
    [[maybe_unused]] double b2 = body(p2)/candleRange(p2);
    double b1 = body(p1)/candleRange(p1);
    [[maybe_unused]] double b0 = body(p0)/candleRange(p0);
    if (b1 < 0.08 && (p2.high < p1.low) && (p0.low > p1.high)) addRange("Abandoned Baby", n-3, n-1, 0.92, "Doji isolated by gaps - strong reversal");
  }
  
  // Three soldiers/crows already covered in previous file version; add detection if missing:
  if (n >= 3) {
    const StockPoint &x1 = history[n-3], &x2 = history[n-2], &x3 = history[n-1];
    if (x1.close > x1.open && x2.close > x2.open && x3.close > x3.open &&
        x2.open > x1.open && x3.open > x2.open && x2.close > x1.close && x3.close > x2.close) {
      addRange("Three White Soldiers", n-3, n-1, 0.92, "Three strong bullish candles in succession");
    }
    if (x1.close < x1.open && x2.close < x2.open && x3.close < x3.open &&
        x2.open < x1.open && x3.open < x2.open && x2.close < x1.close && x3.close < x2.close) {
      addRange("Three Black Crows", n-3, n-1, 0.92, "Three strong bearish candles in succession");
    }
  }
  
  // Side-by-side white lines (two same color candles with gap)
  if (n >= 2) {
    const StockPoint &pA = history[n-2], &pB = history[n-1];
    if (pA.close > pA.open && pB.close > pB.open && std::fabs(pA.close - pB.close) / std::max(1e-9, pA.close) < 0.02)
      addRange("Side-by-Side White Lines", n-2, n-1, 0.65, "Two adjacent bullish bars with similar closes");
    if (pA.close < pA.open && pB.close < pB.open && std::fabs(pA.close - pB.close) / std::max(1e-9, pA.close) < 0.02)
      addRange("Side-by-Side Black Lines", n-2, n-1, 0.65, "Two adjacent bearish bars with similar closes");
  }
  
  // Three Outside / Inside patterns already partially detected above; ensure "Concealing Baby Swallow" detection (rare)
  if (n >= 3) {
    // Concealing Baby Swallow: black marubozu followed by gap down and multiple doji that "swallow" previous range
    const StockPoint &p2 = history[n-3], &p1 = history[n-2], &p0 = history[n-1];
    if ((p2.open - p2.close) / std::max(1e-9, candleRange(p2)) > 0.8 && // p2 bearish marubozu-like
        (p1.low < p2.low && p0.low < p1.low) &&
        (body(p1)/candleRange(p1) < 0.12 || body(p0)/candleRange(p0) < 0.12))
      addRange("Concealing Baby Swallow", n-3, n-1, 0.8, "Rare reversal/continuation pattern (doji + marubozu interplay)");
  }
  
  // Plenty more exotic patterns exist, but above covers the canonical exhaustive set used in professional TA libraries.
  
  return out;
}
