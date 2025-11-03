//
//  StockSummary.hpp
//  KanVest
//
//  Created by Ashish . on 03/11/25.
//

#pragma once

namespace KanVest
{
  struct StockSummary
  {
    std::string trend;        // Uptrend / Downtrend / Sideways
    std::string momentum;     // Strong / Weak / Neutral
    std::string volatility;   // High / Medium / Low
    std::string volume;       // High / Normal / Low
    std::string valuation;    // Overbought / Oversold / Fair
    std::string conclusion;   // Final AI-like sentence summary
  };
} // namespace KanVest
