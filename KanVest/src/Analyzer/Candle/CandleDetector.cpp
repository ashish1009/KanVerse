//
//  CandleDetector.cpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#include "CandleDetector.hpp"

#include "Analyzer/Candle/CandleUtils.hpp"

namespace KanVest::Candle
{
  CandlePattern DetectDoji(const StockPoint& p)
  {
    CandlePattern out;
    if (CandleUtils::IsDoji(p, 0.15))
    {
      out.name = "Doji";
      out.confidence = 0.8;
      out.description = "Indecision candle: open ~ close.";
    }
    return out;
  }
  
  
  CandlePattern DetectHammer(const StockPoint& p)
  {
    CandlePattern out;
    double body = CandleUtils::BodySize(p);
    double lower = CandleUtils::LowerWick(p);
    double upper = CandleUtils::UpperWick(p);
    if (lower > body * 2.5 && upper < body * 0.5 && body > 0)
    {
      out.name = "Hammer";
      out.bullish = CandleUtils::IsBull(p);
      out.confidence = 0.85;
      out.description = "Long lower wick, small body — potential bullish reversal.";
    }
    return out;
  }
  
  CandlePattern DetectShootingStar(const StockPoint& p)
  {
    CandlePattern out;
    double body = CandleUtils::BodySize(p);
    double lower = CandleUtils::LowerWick(p);
    double upper = CandleUtils::UpperWick(p);
    if (upper > body * 2.5 && lower < body * 0.5 && body > 0)
    {
      out.name = "Shooting Star";
      out.bearish = CandleUtils::IsBear(p);
      out.confidence = 0.85;
      out.description = "Long upper wick, small body — potential bearish reversal.";
    }
    return out;
  }
  
  
  CandlePattern DetectMarubozu(const StockPoint& p)
  {
    CandlePattern out;
    double body = CandleUtils::BodySize(p);
    double range = p.high - p.low;
    if (range <= 0) return out;
    if (body / range > 0.9)
    {
      out.name = "Marubozu";
      out.bullish = CandleUtils::IsBull(p);
      out.bearish = CandleUtils::IsBear(p);
      out.confidence = 0.8;
      out.description = "Marubozu: strong directional candle with little/no wicks.";
    }
    return out;
  }
  
  CandlePattern DetectBullishEngulfing(const StockPoint& prev, const StockPoint& cur)
  {
    CandlePattern out;
    double prevBody = std::fabs(prev.close - prev.open);
    double curBody = std::fabs(cur.close - cur.open);
    if (prev.close < prev.open && cur.close > cur.open)
    {
      // current bullish engulfs previous bearish if body is larger and covers open/close
      if (cur.open <= prev.close && cur.close >= prev.open && curBody > prevBody * 0.8)
      {
        out.name = "Bullish Engulfing";
        out.bullish = true;
        out.confidence = 0.9;
        out.description = "Bullish engulfing: strong reversal signal.";
      }
    }
    return out;
  }
  
  
  CandlePattern DetectBearishEngulfing(const StockPoint& prev, const StockPoint& cur)
  {
    CandlePattern out;
    double prevBody = std::fabs(prev.close - prev.open);
    double curBody = std::fabs(cur.close - cur.open);
    if (prev.close > prev.open && cur.close < cur.open)
    {
      if (cur.open >= prev.close && cur.close <= prev.open && curBody > prevBody * 0.8)
      {
        out.name = "Bearish Engulfing";
        out.bearish = true;
        out.confidence = 0.9;
        out.description = "Bearish engulfing: strong reversal signal.";
      }
    }
    return out;
  }
  
  CandlePattern DetectMorningStar(const std::vector<StockPoint>& w)
  {
    CandlePattern out;
    if (w.size() < 3) return out;
    const auto &c1 = w[w.size()-3];
    const auto &c2 = w[w.size()-2];
    const auto &c3 = w[w.size()-1];
    
    
    // c1 bearish, c2 small body (gap), c3 bullish closing into c1 body
    if (c1.close < c1.open && CandleUtils::BodySize(c2) < CandleUtils::BodySize(c1)*0.5 && c3.close > c3.open)
    {
      if (c3.close > (c1.open + c1.close)/2)
      {
        out.name = "Morning Star";
        out.bullish = true;
        out.confidence = 0.9;
        out.description = "Morning Star: 3-candle bullish reversal.";
      }
    }
    return out;
  }
  
  
  CandlePattern DetectEveningStar(const std::vector<StockPoint>& w)
  {
    CandlePattern out;
    if (w.size() < 3) return out;
    const auto &c1 = w[w.size()-3];
    const auto &c2 = w[w.size()-2];
    const auto &c3 = w[w.size()-1];
    
    
    if (c1.close > c1.open && CandleUtils::BodySize(c2) < CandleUtils::BodySize(c1)*0.5 && c3.close < c3.open)
    {
      if (c3.close < (c1.open + c1.close)/2)
      {
        out.name = "Evening Star";
        out.bearish = true;
        out.confidence = 0.9;
        out.description = "Evening Star: 3-candle bearish reversal.";
      }
    }
    return out;
  }
}
