//
//  MathUtils.hpp
//  KanVest
//
//  Created by Ashish . on 10/11/25.
//

#pragma once

#include "Stocks/StockData.hpp"

namespace KanVest
{
  class TechnicalUtils
  {
  public:
    static double ComputeSMA(const std::vector<StockPoint>& history, int period);
    static double ComputeEMA(const std::vector<StockPoint>& history, int period);
    static double ComputeRSI(const std::vector<StockPoint>& history, int period);
    static std::tuple<double,double> ComputeMACD(const std::vector<StockPoint>& history, int fastPeriod, int slowPeriod, int signalPeriod);
    static double ComputeATR(const std::vector<StockPoint>& history, int period);
    static double ComputeVWAP(const std::vector<StockPoint>& history);
  };
} // namespace KanVest
