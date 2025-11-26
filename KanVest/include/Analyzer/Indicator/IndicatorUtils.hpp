//
//  IndicatorUtils.hpp
//  KanVest
//
//  Created by Ashish . on 26/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest::Indicator::Utils
{
  std::vector<double> BuildDailyCloses(const StockData& data);
} // namespace KanVest::Indicator::Utils
