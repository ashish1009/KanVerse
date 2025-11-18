//
//  CandleType.hpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#pragma once

namespace KanVest::Candle
{
  struct CandlePattern
  {
    std::string name;
    bool bullish = false;
    bool bearish = false;
    double confidence = 0.0; // 0..1
    std::string description;
  };
}
