//
//  Indicators.hpp
//  KanVest
//
//  Created by Ashish . on 07/11/25.
//

#pragma once

namespace KanVest::Analysis::Indicators
{  
  // Simple SMA for last n bars; returns NAN if insufficient
  double SMA(const std::vector<double>& series, size_t n);
  
  // EMA of entire series (last value); returns NAN if empty or n==0
  double EMA(const std::vector<double>& series, size_t n);
  
  // RSI last value (period)
  double RSI(const std::vector<double>& closes, size_t period);
  
  // ATR last value
  double ATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, size_t period);
  
  // MACD (returns pair {macd_last, signal_last}) using standard fast/slow/signal
  std::pair<double,double> MACD(const std::vector<double>& closes, size_t fast=12, size_t slow=26, size_t signal=9);
  
  // Bollinger Bands (upper, middle, lower) for last bar
  struct BB { double upper; double middle; double lower; };
  BB Bollinger(const std::vector<double>& closes, size_t period = 20, double k = 2.0);
  
  // On-Balance Volume (last value)
  double OBV(const std::vector<double>& closes, const std::vector<uint64_t>& volumes);
  
  // ADX (returns ADX last value) - stub complex but included
  double ADX(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, size_t period = 14);
  
  // Stochastic %K/%D last values
  std::pair<double,double> Stochastic(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, size_t kperiod=14, size_t dperiod=3);
  
} // namespace KanVest::Analysis::Indicators
