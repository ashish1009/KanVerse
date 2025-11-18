//
//  Indicators.hpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#pragma once

#include "IIndicator.hpp"
#include "StockData.hpp"
#include "MovingAverage.hpp"
#include "Momentum.hpp"
#include "MACD.hpp"
#include "ADX.hpp"
#include "BollingerBands.hpp"
#include "MFI.hpp"
#include "Stochastic.hpp"

namespace KanVest
{
  // ------------------ Moving Averages ------------------
  class MAIndicator : public IIndicator
  {
  public:
    MAIndicator(int smaPeriod = 20, int emaPeriod = 20)
    : smaPeriod(smaPeriod), emaPeriod(emaPeriod) {}
    
    MAResult result;
    int smaPeriod;
    int emaPeriod;
    
    void Compute(const StockData& data) override
    {
      result = MovingAverages::Compute(data, "1D");
    }
    
    void Reset() override
    {
      result.smaValues.clear();
      result.emaValues.clear();
    }
  };
  
  // ------------------ RSI ------------------
  class RSIIndicator : public IIndicator
  {
  public:
    explicit RSIIndicator(size_t period = 14) : period(period) {}
    RSISeries result;
    size_t period;
    
    void Compute(const StockData& data) override
    {
      result = RSI::Compute(data, period);
    }
    
    void Reset() override
    {
      result.series.clear();
      result.last = std::numeric_limits<double>::quiet_NaN();
    }
  };
  
  // ------------------ MACD ------------------
  class MACDIndicator : public IIndicator
  {
  public:
    MACDIndicator(int fast = 12, int slow = 26, int signal = 9)
    : fast(fast), slow(slow), signal(signal) {}
    
    MACDResult result;
    int fast, slow, signal;
    
    void Compute(const StockData& data) override
    {
      result = MACD::Compute(data);
    }
    
    void Reset() override
    {
      result.macdLine.clear();
      result.signalLine.clear();
      result.histogram.clear();
    }
  };
  
  // ------------------ ADX ------------------
  class ADXIndicator : public IIndicator
  {
  public:
    explicit ADXIndicator(int period = 14) : period(period) {}
    ADXResult result;
    int period;
    
    void Compute(const StockData& data) override
    {
      result = ADX::Compute(data, period);
    }
    
    void Reset() override
    {
      result.adx.clear();
      result.plusDI.clear();
      result.minusDI.clear();
      result.valid = false;
      result.last = 0.0;
    }
  };
  
  // ------------------ Bollinger Bands ------------------
  class BBIndicator : public IIndicator
  {
  public:
    BBIndicator(int period = 20, double k = 2.0) : period(period), k(k) {}
    BBResult result;
    int period;
    double k;
    
    void Compute(const StockData& data) override
    {
      result = BollingerBands::Compute(data, period, k);
    }
    
    void Reset() override
    {
      result.middle.clear();
      result.upper.clear();
      result.lower.clear();
      result.deviation.clear();
      result.valid = false;
    }
  };
  
  // ------------------ MFI ------------------
  class MFIIndicator : public IIndicator
  {
  public:
    explicit MFIIndicator(int period = 14) : period(period) {}
    MFIResult result;
    int period;
    
    void Compute(const StockData& data) override
    {
      result = MFI::Compute(data, period);
    }
    
    void Reset() override
    {
      result.series.clear();
      result.valid = false;
      result.last = 0.0;
    }
  };
  
  // ------------------ Stochastic ------------------
  class StochasticIndicator : public IIndicator
  {
  public:
    explicit StochasticIndicator(int period = 14) : period(period) {}
    StochasticResult result;
    int period;
    
    void Compute(const StockData& data) override
    {
      result = Stochastic::Compute(data, period);
    }
    
    void Reset() override
    {
      result.kSeries.clear();
      result.dSeries.clear();
      result.lastK = 0.0;
      result.lastD = 0.0;
    }
  };
}

