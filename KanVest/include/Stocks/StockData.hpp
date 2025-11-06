//
//  StockData.hpp
//  KanVest
//
//  Created by Ashish . on 06/11/25.
//

#pragma once

namespace KanVest
{
  struct StockPoint
  {
    double open = 0.0;
    double high = 0.0;
    double low = 0.0;
    double close = 0.0;
    uint64_t volume = 0;
    uint64_t timestamp = 0;
    double range = 0.0;
  };
  
  struct StockData
  {
    // --- Basic Info ---
    std::string symbol = "";
    std::string currency = "";
    std::string exchangeName = "";
    std::string shortName = "";
    std::string longName = "";
    std::string instrumentType = "";
    std::string timezone = "";
    std::string range = "";
    std::string dataGranularity = "";
    
    // --- Price Info ---
    double livePrice = -1;
    double prevClose = -1;
    
    // --- Change Info ---
    double change = -1;
    double changePercent = -1;
    
    // --- Volume Info ---
    double volume = -1;
    
    // --- High-Low Info ---
    double fiftyTwoHigh = -1;
    double fiftyTwoLow = -1;
    double dayHigh = -1;
    double dayLow = -1;
    
    // --- Historical Candles ---
    std::vector<StockPoint> history;
    
    StockData(const std::string& name = "") : symbol(name) {}
    bool IsValid() const { return !symbol.empty() && !history.empty(); }
  };
} // namespace KanVest
