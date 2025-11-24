//
//  UI_MovingAverage.hpp
//  KanVest
//
//  Created by Ashish . on 23/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  class UI_MovingAverage
  {
  public:
    static void ShowSMA(const StockData& stockData, ImTextureID shadowTexture = 0);
    static void ShowEMA(const StockData& stockData, ImTextureID shadowTexture = 0);
    
  private:
    static void ShowMovingAverageData(const StockData& stockData, const std::map<int, double>& maMap, const std::string& maString, ImTextureID shadowTexture);
  };
} // namespace KanVest
