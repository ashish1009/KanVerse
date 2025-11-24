//
//  UI_Pivot.hpp
//  KanVest
//
//  Created by Ashish . on 24/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

namespace KanVest
{
  class UI_Pivot
  {
  public:
    static void Show(const StockData& stockData, ImTextureID shadowTexture);
  };
} // namespace KanVest
