//
//  CandleEngine.cpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
//

#include "CandleEngine.hpp"

#include "Stocks/Analyzer/Candle/CandleDetector.hpp"

namespace KanVest::Candle
{
  std::vector<std::pair<size_t, CandlePattern>> CandlePatternEngine::Analyze(const StockData& stock, size_t lookback)
  {
    std::vector<std::pair<size_t, CandlePattern>> out;
    if (!stock.IsValid()) return out;
    
    
    size_t N = stock.history.size();
    if (lookback == 0 || lookback > N) lookback = N;
    
    
    size_t start = N - lookback;
    
    
    for (size_t i = start; i < N; ++i)
    {
      const auto& cur = stock.history[i];
      
      
      // single candle
      auto d1 = DetectDoji(cur);
      if (!d1.name.empty()) out.emplace_back(i, d1);
      
      
      auto h = DetectHammer(cur);
      if (!h.name.empty()) out.emplace_back(i, h);
      
      
      auto s = DetectShootingStar(cur);
      if (!s.name.empty()) out.emplace_back(i, s);
      
      
      auto m = DetectMarubozu(cur);
      if (!m.name.empty()) out.emplace_back(i, m);
      
      
      // two-candle
      if (i >= 1)
      {
        const auto& prev = stock.history[i-1];
        auto be = DetectBearishEngulfing(prev, cur);
        if (!be.name.empty()) out.emplace_back(i, be);
        
        
        auto bu = DetectBullishEngulfing(prev, cur);
        if (!bu.name.empty()) out.emplace_back(i, bu);
      }
      
      
      // three-candle
      if (i >= 2)
      {
        std::vector<StockPoint> w = { stock.history[i-2], stock.history[i-1], stock.history[i] };
        auto ms = DetectMorningStar(w);
        if (!ms.name.empty()) out.emplace_back(i, ms);
        
        
        auto es = DetectEveningStar(w);
        if (!es.name.empty()) out.emplace_back(i, es);
      }
    }
    return out;
  }
}
