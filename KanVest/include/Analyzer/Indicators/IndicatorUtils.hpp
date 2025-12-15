//
//  IndicatorUtils.hpp
//  KanVest
//
//  Created by Ashish . on 15/12/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest::Indicator::Utils
{
  std::vector<double> BuildDailyCloses(const StockData& data);
} // namespace KanVest::Indicator::Utils
