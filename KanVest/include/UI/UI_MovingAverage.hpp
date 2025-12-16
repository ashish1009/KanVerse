//
//  UI_MovingAverage.hpp
//  KanVest
//
//  Created by Ashish . on 14/12/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  class UI_MovingAverage
  {
  public:
    static void ShowDMA(const StockData& stockData, ImTextureID shadowTexture = 0);
    static void ShowEMA(const StockData& stockData, ImTextureID shadowTexture = 0);
    
  private:
    static void ShowMovingAverageData(const StockData& stockData, const std::map<int, std::vector<double>>& maMap, const std::string& maString, ImTextureID shadowTexture);
  };
} // namespace KanVest
