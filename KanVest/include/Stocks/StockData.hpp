//
//  StockData.hpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#pragma once

namespace KanVest
{
  struct StockData
  {
    std::string symbol = "";
    
    double livePrice = -1;
    double prevClose = -1;
    double change = -1;
    double changePercent = -1;

    StockData(const std::string& name) : symbol(name) {}
  };
} // namespace KanVest
