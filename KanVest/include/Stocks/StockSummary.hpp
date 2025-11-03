//
//  StockSummary.hpp
//  KanVest
//
//  Created by Ashish . on 03/11/25.
//

#pragma once

namespace KanVest
{
  struct SummaryData
  {
    std::string value;
    std::string reason;
  };
  
  struct StockSummary
  {
    SummaryData trend;
    SummaryData momentum;
    SummaryData volatility;
    SummaryData volume;
    SummaryData valuation;
    SummaryData vwapBias;
    
    std::string conclusion;
    std::string suggestion;
    double score = 50.0;
  };
  
  struct StockTechnicals
  {
    double close;
    double sma;
    double ema;
    double rsi;
    double atr;
    double vwap;
    double macd;
    double avgVol;
    double latestVol;
  };
} // namespace KanVest
