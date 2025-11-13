//
//  TechnicalAnalyzer.cpp
//  KanVest
//
//  Created by Ashish . on 10/11/25.
//

#include "TechnicalAnalyzer.hpp"

#include "Stocks/Analyzer/TechnicalUtils.hpp"

namespace KanVest
{
  TechnicalReport TechnicalAnalyzer::Analyze(const StockData& stock)
  {
    IK_PERFORMANCE_FUNC("TechnicalAnalyzer::Analyze");
    TechnicalReport report;
    
    ComputeMovingAverages(stock, report);
    ComputeRSI(stock, report);
//    ComputeMACD(stock, report);
//    ComputeATR(stock, report);
//    ComputeVWAP(stock, report);
//    
//    ComputeAwesomeOscillator(stock, report);
//    ComputeStochasticRSI(stock, report);
//    ComputeCCI(stock, report);
//    
//    ComputeADX(stock, report);   // default period 14
//    ComputeMFI(stock, report);   // default period 14
//    ComputeOBV(stock, report);
    
    return report;
  }

  void TechnicalAnalyzer::ComputeMovingAverages(const StockData& stock, TechnicalReport& report)
  {
    IK_PERFORMANCE_FUNC("TechnicalAnalyzer::ComputeMovingAverages");
    static std::mutex reportMutex;
    const auto& data = stock.history;
    if (data.empty())
      return;
    
    // Extract closing prices
    std::vector<double> closes;
    closes.reserve(data.size());
    for (const auto& p : data)
      closes.push_back(p.close);
    
    // Common SMA/EMA periods (in days)
    std::vector<int> basePeriods = {5, 10, 20, 30, 50, 100, 150, 200};
    std::vector<std::future<void>> tasks;
    
    // Determine bars per trading day and ensure valid resolution
    int barsPerDay = 1;
    int dummyPeriodBars = 0;
    if (!TechnicalUtils::ResolvePeriods(data, 1, dummyPeriodBars, barsPerDay))
      return;
    
    for (int days : basePeriods)
    {
      int periodBars = 0;
      if (!TechnicalUtils::ResolvePeriods(data, days, periodBars, barsPerDay))
        continue;
      
      if (closes.size() < static_cast<size_t>(periodBars))
        continue; // not enough data
      
      tasks.emplace_back(std::async(std::launch::async, [&, days, periodBars]() {
        // --- Simple Moving Average ---
        double sma = std::accumulate(closes.end() - periodBars, closes.end(), 0.0) / periodBars;
        
        // --- Exponential Moving Average ---
        double alpha = 2.0 / (periodBars + 1.0);
        double ema = closes.front();
        for (size_t i = 1; i < closes.size(); ++i)
          ema = alpha * closes[i] + (1.0 - alpha) * ema;
        
        // --- Thread-safe update ---
        {
          std::lock_guard<std::mutex> lock(reportMutex);
          report.SMA[days] = sma;
          report.EMA[days] = ema;
          
          report.Explanations["SMA_" + std::to_string(days)] =
          "The " + std::to_string(days) + "-day SMA (" + std::to_string(periodBars) +
          " bars) = " + std::to_string(sma) + ", showing average price over the last " +
          std::to_string(days) + " trading days.";
          
          report.Explanations["EMA_" + std::to_string(days)] =
          "The " + std::to_string(days) + "-day EMA (" + std::to_string(periodBars) +
          " bars) = " + std::to_string(ema) +
          ", reacting faster to recent price movements.";
        }
      }));
    }
    
    for (auto& t : tasks)
      t.wait();
  }
  
  void TechnicalAnalyzer::ComputeRSI(const StockData& stock, TechnicalReport& report, int period)
  {
    IK_PERFORMANCE_FUNC("TechnicalAnalyzer::ComputeRSI");
    double rsi = TechnicalUtils::ComputeRSI(stock.history, period);
    report.RSI = rsi;
    
    report.Explanations["RSI"] = "RSI (" + std::to_string(period) + ") is " + std::to_string(rsi) +
    ". RSI above 70 may indicate overbought; below 30 may indicate oversold conditions.";
  }
  
//  void TechnicalAnalyzer::ComputeMACD(const StockData& stock, TechnicalReport& report, int fastPeriod, int slowPeriod, int signalPeriod)
//  {
//    IK_PERFORMANCE_FUNC("TechnicalAnalyzer::ComputeMACD");
//    auto [macd, signal] = TechnicalUtils::ComputeMACD(stock.history, fastPeriod, slowPeriod, signalPeriod);
//    report.MACD = macd;
//    report.MACDSignal = signal;
//    
//    report.Explanations["MACD"] = "MACD (" + std::to_string(fastPeriod) + "," + std::to_string(slowPeriod) + ") is " + std::to_string(macd) +
//    " and Signal line is " + std::to_string(signal) + ". Positive MACD indicates upward momentum; negative indicates downward momentum.";
//  }
//  
//  void TechnicalAnalyzer::ComputeATR(const StockData& stock, TechnicalReport& report, int period)
//  {
//    IK_PERFORMANCE_FUNC("TechnicalAnalyzer::ComputeATR");
//    double atr = TechnicalUtils::ComputeATR(stock.history, period);
//    report.ATR = atr;
//    
//    report.Explanations["ATR"] = "ATR (" + std::to_string(period) + ") is " + std::to_string(atr) +
//    ". ATR measures volatility; higher ATR indicates higher price movement risk.";
//  }
//  
//  void TechnicalAnalyzer::ComputeVWAP(const StockData& stock, TechnicalReport& report)
//  {
//    IK_PERFORMANCE_FUNC("TechnicalAnalyzer::ComputeVWAP");
//    double vwap = TechnicalUtils::ComputeVWAP(stock.history);
//    report.VWAP = vwap;
//    
//    report.Explanations["VWAP"] = "VWAP is " + std::to_string(vwap) +
//    ". VWAP represents the average price weighted by volume; used by traders to determine market trend intraday.";
//  }
//
//  void TechnicalAnalyzer::ComputeAwesomeOscillator(const StockData& stock, TechnicalReport& report, int fastPeriodInDays, int slowPeriodInDays)
//  {
//    IK_PERFORMANCE_FUNC("TechnicalAnalyzer::ComputeAwesomeOscillator");
//    double asmOsc = TechnicalUtils::ComputeAwesomeOscillator(stock.history, fastPeriodInDays, slowPeriodInDays);
//    report.AwesomeOscillator = asmOsc;
//    
//    report.Explanations["AwesomeOscillator"] =
//    "AO = " + std::to_string(asmOsc) +
//    ". It compares short-term and long-term momentum. "
//    "Positive → bullish, Negative → bearish.";
//  }
//
//  void TechnicalAnalyzer::ComputeStochasticRSI(const StockData& stock, TechnicalReport& report, int period)
//  {
//    IK_PERFORMANCE_FUNC("TechnicalAnalyzer::ComputeStochasticRSI");
//    double stochasticRSI = TechnicalUtils::ComputeStochasticRSI(stock.history,period);
//    report.StochasticRSI = stochasticRSI;
//    
//    report.Explanations["StochasticRSI"] =
//    "StochRSI = " + std::to_string(stochasticRSI) +
//    ". Shows RSI’s position within its recent range — >0.8 overbought, <0.2 oversold.";
//    
//  }
//
//  void TechnicalAnalyzer::ComputeCCI(const StockData& stock, TechnicalReport& report, int period)
//  {
//    IK_PERFORMANCE_FUNC("TechnicalAnalyzer::ComputeCCI");
//    double cci = TechnicalUtils::ComputeCCI(stock.history,period);
//    report.CCI = cci;
//    
//    report.Explanations["CCI"] =
//    "CCI = " + std::to_string(cci) +
//    ". Measures price deviation from its average — >+100 overbought, <-100 oversold.";
//  }
//
//  std::string TechnicalAnalyzer::DescribeTrend(double value, const std::string& indicator)
//  {
//    IK_PERFORMANCE_FUNC("TechnicalAnalyzer::DescribeTrend");
//    if (indicator == "SMA" || indicator == "EMA")
//    {
//      if (value > 0) return "Uptrend";
//      else if (value < 0) return "Downtrend";
//      else return "Neutral";
//    }
//    return "Neutral";
//  }
//  
//  void TechnicalAnalyzer::ComputeADX(const StockData& stock, TechnicalReport& report, int period)
//  {
//    IK_PERFORMANCE_FUNC("TechnicalAnalyzer::ComputeADX");
//    double plusDI = 0.0, minusDI = 0.0;
//    double adx = TechnicalUtils::ComputeADX(stock.history, period, plusDI, minusDI);
//    report.ADX = adx;
//    report.PlusDI = plusDI;
//    report.MinusDI = minusDI;
//    
//    report.Explanations["ADX"] =
//    "ADX (" + std::to_string(period) + ") = " + std::to_string(adx) +
//    ". +DI = " + std::to_string(plusDI) + ", -DI = " + std::to_string(minusDI) +
//    ". ADX measures trend strength (higher = stronger trend).";
//  }
//  
//  void TechnicalAnalyzer::ComputeMFI(const StockData& stock, TechnicalReport& report, int period)
//  {
//    IK_PERFORMANCE_FUNC("TechnicalAnalyzer::ComputeMFI");
//    double mfi = TechnicalUtils::ComputeMFI(stock.history, period);
//    report.MFI = mfi;
//    report.Explanations["MFI"] = "MFI (" + std::to_string(period) + ") = " + std::to_string(mfi) +
//    ". Money Flow Index uses price+volume to detect buying/selling pressure (0-100).";
//  }
//  
//  void TechnicalAnalyzer::ComputeOBV(const StockData& stock, TechnicalReport& report)
//  {
//    IK_PERFORMANCE_FUNC("TechnicalAnalyzer::ComputeOBV");
//    double obv = TechnicalUtils::ComputeOBV(stock.history);
//    report.OBV = obv;
//    report.Explanations["OBV"] = "On-Balance Volume (OBV) accumulative value = " + std::to_string(obv) +
//    ". OBV confirms price moves using volume flow.";
//  }
} // namespace KanVest
