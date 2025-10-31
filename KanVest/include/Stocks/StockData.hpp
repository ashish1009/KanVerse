//
//  StockData.hpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#pragma once

namespace KanVest
{
  struct Stock
  {
    std::string name = "";
    double livePrice = -1; // Should be updated live
  };
} // namespace KanVest
