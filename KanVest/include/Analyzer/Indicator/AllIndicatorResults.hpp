//
//  AllIndicatorResults.hpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#pragma once

#include "Indicators.hpp"

namespace KanVest
{
  struct AllIndicatorsResult
  {
    MAResult ma;
    RSISeries rsi;
    MACDResult macd;
    ADXResult adx;
    BBResult bb;
    MFIResult mfi;
    StochasticResult stochastic;
  };
}
