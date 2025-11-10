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
    TechnicalReport report;
    
    ComputeMovingAverages(stock, report);
    ComputeRSI(stock, report);
    ComputeMACD(stock, report);
    ComputeATR(stock, report);
    ComputeVWAP(stock, report);
    
    return report;
  }
  
  void TechnicalAnalyzer::ComputeMovingAverages(const StockData& stock, TechnicalReport& report)
  {
    std::vector<int> periods = {5, 20, 50, 100, 200};
    
    for (int period : periods)
    {
      double sma = TechnicalUtils::ComputeSMA(stock.history, period);
      double ema = TechnicalUtils::ComputeEMA(stock.history, period);
      
      report.SMA[period] = sma;
      report.EMA[period] = ema;
      
      report.Explanations["SMA_" + std::to_string(period)] =
      "The " + std::to_string(period) + "-day SMA is " + std::to_string(sma) +
      ". This indicates the average closing price over last " + std::to_string(period) + " days.";
      report.Explanations["EMA_" + std::to_string(period)] =
      "The " + std::to_string(period) + "-day EMA is " + std::to_string(ema) +
      ". EMA gives more weight to recent prices and reacts faster to changes.";
    }
  }
  
  void TechnicalAnalyzer::ComputeRSI(const StockData& stock, TechnicalReport& report, int period)
  {
    double rsi = TechnicalUtils::ComputeRSI(stock.history, period);
    report.RSI = rsi;
    
    report.Explanations["RSI"] = "RSI (" + std::to_string(period) + ") is " + std::to_string(rsi) +
    ". RSI above 70 may indicate overbought; below 30 may indicate oversold conditions.";
  }
  
  void TechnicalAnalyzer::ComputeMACD(const StockData& stock, TechnicalReport& report, int fastPeriod, int slowPeriod, int signalPeriod)
  {
    auto [macd, signal] = TechnicalUtils::ComputeMACD(stock.history, fastPeriod, slowPeriod, signalPeriod);
    report.MACD = macd;
    report.MACDSignal = signal;
    
    report.Explanations["MACD"] = "MACD (" + std::to_string(fastPeriod) + "," + std::to_string(slowPeriod) + ") is " + std::to_string(macd) +
    " and Signal line is " + std::to_string(signal) + ". Positive MACD indicates upward momentum; negative indicates downward momentum.";
  }
  
  void TechnicalAnalyzer::ComputeATR(const StockData& stock, TechnicalReport& report, int period)
  {
    double atr = TechnicalUtils::ComputeATR(stock.history, period);
    report.ATR = atr;
    
    report.Explanations["ATR"] = "ATR (" + std::to_string(period) + ") is " + std::to_string(atr) +
    ". ATR measures volatility; higher ATR indicates higher price movement risk.";
  }
  
  void TechnicalAnalyzer::ComputeVWAP(const StockData& stock, TechnicalReport& report)
  {
    double vwap = TechnicalUtils::ComputeVWAP(stock.history);
    report.VWAP = vwap;
    
    report.Explanations["VWAP"] = "VWAP is " + std::to_string(vwap) +
    ". VWAP represents the average price weighted by volume; used by traders to determine market trend intraday.";
  }
  
  std::string TechnicalAnalyzer::DescribeTrend(double value, const std::string& indicator)
  {
    if (indicator == "SMA" || indicator == "EMA")
    {
      if (value > 0) return "Uptrend";
      else if (value < 0) return "Downtrend";
      else return "Neutral";
    }
    return "Neutral";
  }
} // namespace KanVest
