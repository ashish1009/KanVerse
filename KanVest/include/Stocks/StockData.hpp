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
    std::string shortName = "";
    std::string longName = "";
    std::string currency = "";
    std::string exchangeName = "";
    
    double livePrice = -1;
    double prevClose = -1;
    
    double change = -1;
    double changePercent = -1;
    
    double volume = -1;
    
    double fiftyTwoHigh = -1;
    double fiftyTwoLow = -1;
    double dayHigh = -1;
    double dayLow = -1;

    StockData(const std::string& name) : symbol(name) {}
  };
} // namespace KanVest
