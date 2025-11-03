//
//  StockAnalyzer.hpp
//  KanVest
//
//  Created by Ashish . on 03/11/25.
//

#pragma once

#include "Stocks/StockData.hpp"
#include "Stocks/StockSummary.hpp"

namespace KanVest
{
  struct IndicatorConfig
  {
    int smaPeriod = 20;
    int emaPeriod = 20;
    int rsiPeriod = 14;
    int atrPeriod = 14;
    int volPeriod = 20;
    
    // Sensitivity: how big EMA-SMA divergence must be to consider "strong"
    double momentumSensitivity = 0.012;
        
    // Volume ratio thresholds
    double volHighFactor = 1.5;
    double volLowFactor = 0.7;
  };

  class StockAnalyzer
  {
  public:
    /// This function returns the stock summary for single time frame
    /// - Parameters:
    ///   - data: Stock data
    ///   - interval: interval of rURL
    ///   - range: Range of URL
    StockSummary AnalyzeSingleTimeframe(const StockData& data, const std::string& interval, const std::string& range);
    /// This function returns the stock summary as hybrid
    /// - Parameters:
    ///   - data: Stock data
    ///   - interval: interval of rURL
    ///   - range: Range of URL
    StockSummary AnalyzeHybrid(const StockData& baseData, const std::string& baseInterval, const std::string& baseRange);
    
  private:
    static double SafeDiv(double a, double b, double fallback = 0.0);
    
    // ----------------- Indicator Computations -----------------
    static double ComputeSMA(const std::vector<StockPoint>& h, int period);
    static double ComputeEMA(const std::vector<StockPoint>& h, int period);
    static double ComputeRSI(const std::vector<StockPoint>& h, int period);
    static double ComputeATR(const std::vector<StockPoint>& h, int period);
    static double ComputeVWAP(const std::vector<StockPoint>& h);
    static double ComputeAverageVolume(const std::vector<StockPoint>& h, int period);
    
    // ----------------- Time Aggregation -----------------
    static time_t DayKey(double ts);
    static time_t WeekKey(double ts);
    static std::vector<StockPoint> Aggregate(const std::vector<StockPoint>& input, const std::string& targetInterval);
    
    // ----------------- Config Selection -----------------
    static IndicatorConfig ChooseConfig(const std::string& interval, const std::string& range);
    
    // ----------------- Analysis Core -----------------
    StockSummary AnalyzeInternal(const std::vector<StockPoint>& history, const std::string& interval, const std::string& range, const IndicatorConfig& cfg);
  };
} // namespace KanVest
