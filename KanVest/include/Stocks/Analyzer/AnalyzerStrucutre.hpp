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
  } // namespace Analysis
} // namespace KanVest
