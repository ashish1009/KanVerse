//
//  StockMetadata.hpp
//  KanVest
//
//  Created by Ashish . on 08/01/26.
//

#pragma once

namespace KanVest
{
  /// This structure stores the candle data
  struct CandleData
  {
    double open, close, high, low;
    double range;
    
    uint32_t volume;
    uint32_t timestamp;
  };
  
  /// This structure stores the stock data extracted by URL
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
    std::vector <CandleData> candleHistory;

    bool IsValid() const { return !symbol.empty() && !candleHistory.empty(); }
  };
} // namespace KanVest
