//
//  StockData.hpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
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
    uint64_t timestamp = 0; // UNIX time
  };
  
  struct StockData
  {
    // Basic Data
    std::string symbol = "";
    std::string currency = "";
    std::string exchangeName = "";
    std::string shortName = "";
    std::string longName = "";
    std::string instrumentType = "";
    std::string timezone = "";
    std::string range = "";
    std::string dataGranularity = "";

    // Price Data
    double livePrice = -1;
    double prevClose = -1;
    
    // Change data
    double change = -1;
    double changePercent = -1;
    
    // Volume
    double volume = -1;
    
    // High low
    double fiftyTwoHigh = -1;
    double fiftyTwoLow = -1;
    double dayHigh = -1;
    double dayLow = -1;
    
    // History
    std::vector<StockPoint> history;
    
    // SMA (Simple)
    

    StockData(const std::string& name) : symbol(name) {}
    bool IsValid() const { return !symbol.empty() && !history.empty(); }
  };
} // namespace KanVest
