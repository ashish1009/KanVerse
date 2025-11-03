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
    std::string trend;
    std::string momentum;
    std::string volatility;
    std::string volume;
    std::string valuation;
    std::string conclusion;
    double score = 50.0;
  };
} // namespace KanVest
