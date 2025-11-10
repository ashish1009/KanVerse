//
//  TechnicalAnalyzer.hpp
//  KanVest
//
//  Created by Ashish . on 10/11/25.
//

#pragma once
#include "Stocks/StockData.hpp"

namespace KanVest
{
  struct TechnicalReport
  {
    std::map<int, double> SMA;      // key = period, value = SMA
    std::map<int, double> EMA;      // key = period, value = EMA
    double RSI = 0.0;
    double MACD = 0.0;
    double MACDSignal = 0.0;
    double ATR = 0.0;
    double VWAP = 0.0;
    
    std::map<std::string, std::string> Explanations; // Human-readable explanations
  };
  
  class TechnicalAnalyzer
  {
  public:
    /// Analyze all major technical indicators for given stock
    static TechnicalReport Analyze(const StockData& stock);
  private:
    static void ComputeMovingAverages(const StockData& stock, TechnicalReport& report);
    static void ComputeRSI(const StockData& stock, TechnicalReport& report, int period = 14);
    static void ComputeMACD(const StockData& stock, TechnicalReport& report, int fastPeriod = 12, int slowPeriod = 26, int signalPeriod = 9);
    static void ComputeATR(const StockData& stock, TechnicalReport& report, int period = 14);
    static void ComputeVWAP(const StockData& stock, TechnicalReport& report);
    static std::string DescribeTrend(double value, const std::string& indicator);
  };
} // namespace KanVest
