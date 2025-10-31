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
    double livePrice = -1; // Should be updated live
    
    StockData(const std::string& name) : symbol(name) {}
  };
} // namespace KanVest
