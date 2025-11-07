//
//  Indicators.cpp
//  KanVest
//
//  Created by Ashish . on 07/11/25.
//

#include "Indicators.hpp"

namespace KanVest::Analysis::Indicators
{
  static double nan()
  {
    return std::numeric_limits<double>::quiet_NaN();
  }
  
  // -------------------------------------------------------------
  // 📊 Simple Moving Average (SMA)
  // -------------------------------------------------------------
  // SMA means “average closing price over the last N days”.
  // It shows the general direction of the market:
  //  - If current price > SMA → price trending up
  //  - If current price < SMA → price trending down
  // -------------------------------------------------------------
  double SMA(const std::vector<double>& series, size_t n)
  {
    if (series.size() < n || n == 0)
    {
      return nan();
    }
    
    double sum = 0.0;
    for (size_t i = series.size()-n; i < series.size(); ++i)
    {
      sum += series[i];
    }
    return sum / static_cast<double>(n);
  }
  
  // -------------------------------------------------------------
  // 📈 Exponential Moving Average (EMA)
  // -------------------------------------------------------------
  // EMA is similar to SMA, but gives more weight to recent prices.
  // So it reacts faster when price starts rising or falling.
  // -------------------------------------------------------------
  double EMA(const std::vector<double>& series, size_t n)
  {
    if (series.empty() || n == 0)
    {
      return nan();
    }
    
    double alpha = 2.0 / (static_cast<double>(n) + 1.0);
    double ema = series.front(); // seed
    
    // prefer seed as SMA if enough points
    if (series.size() >= n)
    {
      ema = 0.0;
      for (size_t i = series.size()-n; i < series.size(); ++i)
      {
        ema += series[i];
      }
      
      ema /= static_cast<double>(n);
      for (size_t i = series.size()-n+1; i < series.size(); ++i)
      {
        ema = alpha * series[i] + (1.0 - alpha) * ema;
      }
      return ema;
    }
    for (size_t i = 1; i < series.size(); ++i)
    {
      ema = alpha * series[i] + (1.0 - alpha) * ema;
    }
    return ema;
  }
  
  // -------------------------------------------------------------
  // 💪 Relative Strength Index (RSI)
  // -------------------------------------------------------------
  // RSI shows how strong or weak recent price movements are.
  //
  // - RSI > 70 → price has gone up too fast (overbought)
  // - RSI < 30 → price has dropped too fast (oversold)
  // - RSI ≈ 50 → balanced movement, no strong direction
  // -------------------------------------------------------------
  double RSI(const std::vector<double>& closes, size_t period)
  {
    if (closes.size() < period + 1)
    {
      return nan();
    }
    
    double gain = 0.0, loss = 0.0;
    size_t start = closes.size() - period - 1;
    
    for (size_t i = start; i + 1 < closes.size(); ++i)
    {
      double d = closes[i+1] - closes[i];
      if (d > 0) gain += d; else loss += -d;
    }
    
    if (gain + loss <= 0.0)
    {
      return 50.0;
    }
    
    double avgGain = gain / static_cast<double>(period);
    double avgLoss = loss / static_cast<double>(period);
    
    if (avgLoss == 0.0)
    {
      return 100.0;
    }
    double rs = avgGain / avgLoss;
    return 100.0 - (100.0 / (1.0 + rs));
  }
  
  // -------------------------------------------------------------
  // 🌊 Average True Range (ATR)
  // -------------------------------------------------------------
  // ATR tells us how much prices move (on average) each day.
  // It doesn’t care whether the price went up or down,
  // just how *big* the daily changes were.
  //
  // Higher ATR → more volatility (price jumping around)
  // Lower ATR → calmer price movement
  // -------------------------------------------------------------
  double ATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, size_t period)
  {
    if (highs.size() < period + 1 || lows.size() < period + 1 || closes.size() < period + 1)
    {
      return nan();
    }
    size_t start = highs.size() - period;
    std::vector<double> trs;
    trs.reserve(period);
    for (size_t i = start; i + 1 < highs.size(); ++i)
    {
      double high = highs[i];
      double low = lows[i];
      double prevClose = closes[i == 0 ? 0 : i - 1];
      double tr = std::max({high - low, std::fabs(high - prevClose), std::fabs(low - prevClose)});
      trs.push_back(tr);
    }
    double sum = std::accumulate(trs.begin(), trs.end(), 0.0);
    return sum / static_cast<double>(trs.size());
  }
  
  // -------------------------------------------------------------
  // 📉 MACD (Moving Average Convergence Divergence)
  // -------------------------------------------------------------
  // MACD compares a short-term EMA vs a long-term EMA.
  // It shows how momentum (speed of price changes) is shifting.
  //
  // If short-term average rises above long-term → uptrend starting.
  // If short-term falls below long-term → downtrend starting.
  //
  // MACD also calculates a "signal line" (a smoothed version)
  // and a histogram that shows how strong the shift is.
  // -------------------------------------------------------------
  std::pair<double,double> MACD(const std::vector<double>& closes, size_t fast, size_t slow, size_t signal)
  {
    if (closes.size() < slow)
    {
      return {nan(), nan()};
    }
    
    double emaFast = EMA(closes, fast);
    double emaSlow = EMA(closes, slow);
    
    if (std::isnan(emaFast) || std::isnan(emaSlow))
    {
      return {nan(), nan()};
    }
    
    double macd = emaFast - emaSlow;
    
    // compute macd series for signal calculation
    std::vector<double> macdSeries;
    macdSeries.reserve(closes.size());
    for (size_t i = 0; i < closes.size(); ++i)
    {
      std::vector<double> prefix(closes.begin(), closes.begin() + i + 1);
      if (prefix.size() < slow)
      {
        continue;
      }
      
      double f = EMA(prefix, fast);
      double s = EMA(prefix, slow);
      if (!std::isnan(f) && !std::isnan(s))
      {
        macdSeries.push_back(f - s);
      }
    }
    double signalVal = nan();
    if (macdSeries.size() >= signal)
    {
      signalVal = EMA(macdSeries, signal);
    }
    return {macd, signalVal};
  }
  
  BB Bollinger(const std::vector<double>& closes, size_t period, double k)
  {
    BB out{nan(), nan(), nan()};
    if (closes.size() < period)
    {
      return out;
    }
    
    double m = SMA(closes, period);
    double sumsq = 0.0;
    for (size_t i = closes.size() - period; i < closes.size(); ++i)
    {
      sumsq += (closes[i] - m)*(closes[i] - m);
    }
    double sd = std::sqrt(sumsq / static_cast<double>(period));
    out.middle = m;
    out.upper = m + k * sd;
    out.lower = m - k * sd;
    return out;
  }
  
  double OBV(const std::vector<double>& closes, const std::vector<uint64_t>& volumes)
  {
    if (closes.empty() || volumes.empty() || closes.size() != volumes.size())
    {
      return nan();
    }
    
    double obv = 0.0;
    for (size_t i = 1; i < closes.size(); ++i)
    {
      if (closes[i] > closes[i-1])
      {
        obv += static_cast<double>(volumes[i]);
      }
      else if (closes[i] < closes[i-1])
      {
        obv -= static_cast<double>(volumes[i]);
      }
    }
    return obv;
  }
  
  // ADX, Stochastic stubs (complex). Provide simple versions or call external refined library later.
  double ADX(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, size_t period)
  {
    // Full ADX is long to implement; return NAN as placeholder or simple slope-based proxy if desired.
    return nan();
  }
  
  std::pair<double,double> Stochastic(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, size_t kperiod, size_t dperiod)
  {
    if (closes.size() < kperiod)
    {
      return {nan(), nan()};
    }
    
    size_t start = closes.size() - kperiod;
    double highest = *std::max_element(highs.begin() + start, highs.end());
    double lowest = *std::min_element(lows.begin() + start, lows.end());
    double k = (closes.back() - lowest) / (highest - lowest) * 100.0;
    
    // D is SMA of last dperiod of k values — approximate with k itself for single value
    return {k, k};
  }
  
  // --- CCI (Commodity Channel Index) ---
  inline double CCI(const std::vector<double>& highs,
                    const std::vector<double>& lows,
                    const std::vector<double>& closes,
                    size_t period ) {
    if (closes.size() < period) return nan();
    size_t start = closes.size() - period;
    std::vector<double> tp;
    tp.reserve(period);
    for (size_t i = start; i < closes.size(); ++i) tp.push_back((highs[i] + lows[i] + closes[i]) / 3.0);
    double ma = std::accumulate(tp.begin(), tp.end(), 0.0) / static_cast<double>(period);
    double meanDev = 0.0;
    for (auto v : tp) meanDev += std::abs(v - ma);
    meanDev /= static_cast<double>(period);
    if (meanDev < 1e-12) return nan();
    return (tp.back() - ma) / (0.015 * meanDev);
  }
  
  // --- ROC (Rate of Change) ---
  inline double ROC(const std::vector<double>& closes, size_t period) {
    if (closes.size() < period + 1) return nan();
    double prev = closes[closes.size() - period - 1];
    if (std::abs(prev) < 1e-12) return nan();
    return (closes.back() - prev) / prev * 100.0;
  }
  
  // --- MFI (Money Flow Index) ---
  inline double MFI(const std::vector<double>& highs,
                    const std::vector<double>& lows,
                    const std::vector<double>& closes,
                    const std::vector<uint64_t>& volumes,
                    size_t period ) {
    if (closes.size() < period + 1 || volumes.size() != closes.size()) return nan();
    size_t start = closes.size() - period - 1;
    std::vector<double> posFlow, negFlow;
    for (size_t i = start; i + 1 < closes.size(); ++i) {
      double typicalPrev = (highs[i] + lows[i] + closes[i]) / 3.0;
      double typical = (highs[i+1] + lows[i+1] + closes[i+1]) / 3.0;
      double rawFlow = typical * static_cast<double>(volumes[i+1]);
      if (typical > typicalPrev) posFlow.push_back(rawFlow);
      else negFlow.push_back(rawFlow);
    }
    double pos = std::accumulate(posFlow.begin(), posFlow.end(), 0.0);
    double neg = std::accumulate(negFlow.begin(), negFlow.end(), 0.0);
    if (neg + pos <= 1e-12) return 50.0;
    double mfr = pos / std::max(1e-12, neg);
    return 100.0 - (100.0 / (1.0 + mfr));
  }
  
  double VWAP(const std::vector<KanVest::StockPoint>& h) {
    if (h.empty()) return nan();
    double pv = 0.0, v = 0.0;
    for (const auto& p : h) {
      double typical = (p.high + p.low + p.close) / 3.0;
      pv += typical * static_cast<double>(p.volume);
      v += static_cast<double>(p.volume);
    }
    return (v == 0.0) ? nan() : (pv / v);
  }
} //  namespace KanVest::Analysis::Indicators

