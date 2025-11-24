//
//  Momentum.cpp
//  KanVest
//
//  Created by Ashish . on 24/11/25.
//

#include "Momentum.hpp"

namespace KanVest
{
  // Helper: safe NaN
  static inline double NaN() { return std::numeric_limits<double>::quiet_NaN(); }
  
  RSISeries RSI::Compute(const StockData& data, size_t period)
  {
    RSISeries out;
    return out;
  }
} // namespace KanVest
