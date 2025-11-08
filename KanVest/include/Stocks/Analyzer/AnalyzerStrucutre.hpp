//
//  AnalyzerStrucutre.hpp
//  KanVest
//
//  Created by Ashish . on 07/11/25.
//

#pragma once

namespace KanVest
{
  struct StockPoint;
  struct StockData;
  
  namespace Analysis
  {
    // Recommendation enum
    enum class Recommendation
    {
      StrongBuy, Buy, Hold, Sell, StrongSell, Unknown
    };
    
    // Holding info
    struct HoldingInfo
    {
      double avgPrice = 0.0;
      int qty = 0;
      std::string symbol;
    };
    
    // Generic detection metadata
    struct PatternHit
    {
      std::string name;
      size_t startIndex = 0; // index into history (oldest->newest)
      size_t endIndex = 0;
      double strength = 0.0; // 0..1
      std::string rationale; // short explanation
    };
    
    namespace PatternRationale
    {
      inline const std::unordered_map<std::string, std::string>& Get()
      {
        static const std::unordered_map<std::string, std::string> rationale = {
          // --- Candlestick Patterns ---
          {"Bullish Engulfing", "A large bullish candle fully engulfs the prior bearish one — signals reversal from downtrend to uptrend."},
          {"Bearish Engulfing", "A large bearish candle fully engulfs the prior bullish one — potential reversal from uptrend to downtrend."},
          {"Hammer", "Small body with long lower shadow after decline — suggests buyers stepping in, possible bottoming."},
          {"Inverted Hammer", "Small body with long upper shadow after decline — possible early sign of reversal upward."},
          {"Shooting Star", "Small body with long upper shadow after uptrend — warns of weakening momentum."},
          {"Doji", "Open and close nearly equal — indecision between buyers and sellers, possible reversal or pause."},
          {"Morning Star", "Three-candle bullish reversal: bearish → indecision → strong bullish candle."},
          {"Evening Star", "Three-candle bearish reversal: bullish → indecision → strong bearish candle."},
          {"Piercing Line", "After a downtrend, a bullish candle closes above midpoint of the previous bearish candle — reversal hint."},
          {"Dark Cloud Cover", "After an uptrend, a bearish candle closes below midpoint of previous bullish — bearish reversal signal."},
          
          // --- Chart Patterns ---
          {"Double Top", "Two peaks at similar price levels — suggests strong resistance and potential bearish reversal."},
          {"Double Bottom", "Two troughs at similar price levels — indicates strong support and potential bullish reversal."},
          {"HeadAndShoulders", "Three peaks with middle one highest — reliable bearish reversal signal."},
          {"InverseHeadAndShoulders", "Three troughs with middle one lowest — bullish reversal formation."},
          {"Rising Wedge", "Converging upward trendlines — typically resolves with bearish breakdown."},
          {"Falling Wedge", "Converging downward trendlines — often precedes bullish breakout."},
          {"Ascending Triangle", "Flat resistance with higher lows — bullish continuation setup."},
          {"Descending Triangle", "Flat support with lower highs — bearish continuation pattern."},
          {"Symmetrical Triangle", "Converging highs and lows — indicates consolidation before a breakout."},
          {"CupAndHandle", "Rounded bottom followed by a small pullback — bullish continuation pattern."},
        };
        return rationale;
      }
      
      inline std::string Describe(const std::string& name)
      {
        const auto& dict = Get();
        auto it = dict.find(name);
        if (it != dict.end()) return it->second;
        return "Technical pattern detected — potential reversal or continuation signal.";
      }
    }
  } // namespace Analysis
} // namespace KanVest
