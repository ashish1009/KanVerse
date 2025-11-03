//
//  StockAnalyzer.cpp
//  KanVest
//
//  Created by Ashish . on 03/11/25.
//

#include "StockAnalyzer.hpp"

namespace KanVest
{
  double StockAnalyzer::SafeDiv(double a, double b, double fallback)
  {
    return (b == 0.0) ? fallback : (a / b);
  }
  
  double StockAnalyzer::ComputeSMA(const std::vector<StockPoint>& h, int period)
  {
    if (period <= 0 || h.size() < (size_t)period)
    {
      return -1.0;
    }
    double sum = 0.0;
    for (size_t i = h.size() - period; i < h.size(); ++i)
    {
      sum += h[i].close;
    }
    return sum / period;
  }
  
  double StockAnalyzer::ComputeEMA(const std::vector<StockPoint>& h, int period)
  {
    if (period <= 0 || h.size() < (size_t)period)
    {
      return -1.0;
    }
    
    // seed EMA with SMA of the first period window
    double ema = 0.0;
    size_t start = h.size() - period;
    for (size_t i = start; i < start + (size_t)period; ++i)
    {
      ema += h[i].close;
    }
    
    ema /= period;
    double k = 2.0 / (period + 1.0);
    for (size_t i = start + 1; i < h.size(); ++i)
    {
      ema = (h[i].close * k) + (ema * (1.0 - k));
    }
    return ema;
  }
  
  double StockAnalyzer::ComputeRSI(const std::vector<StockPoint>& h, int period)
  {
    if (period <= 0 || h.size() <= (size_t)period)
    {
      return -1.0;
    }
    
    double gain = 0.0, loss = 0.0;
    // Wilder-style simple initial average (not smoothed carry-forward)
    for (size_t i = h.size() - period + 1; i < h.size(); ++i)
    {
      double diff = h[i].close - h[i - 1].close;
      if (diff > 0) gain += diff; else loss -= diff;
    }
    
    if (gain == 0.0 && loss == 0.0)
    {
      return 50.0;
    }
    if (loss == 0.0)
    {
      return 100.0;
    }
    
    double rs = gain / loss;
    return 100.0 - (100.0 / (1.0 + rs));
  }
  
  double StockAnalyzer::ComputeATR(const std::vector<StockPoint>& h, int period)
  {
    if (period <= 0 || h.size() < (size_t)period + 1)
    {
      return -1.0;
    }
    std::vector<double> tr;
    tr.reserve(h.size() - 1);
    for (size_t i = 1; i < h.size(); ++i)
    {
      double hl = h[i].high - h[i].low;
      double hc = std::fabs(h[i].high - h[i - 1].close);
      double lc = std::fabs(h[i].low - h[i - 1].close);
      tr.push_back(std::max({hl, hc, lc}));
    }
    double sum = 0.0;
    for (size_t i = tr.size() - period; i < tr.size(); ++i)
    {
      sum += tr[i];
    }
    return sum / period;
  }
  
  double StockAnalyzer::ComputeVWAP(const std::vector<StockPoint>& h)
  {
    if (h.empty())
    {
      return -1.0;
    }
    double pv = 0.0;
    double vol = 0.0;
    for (const auto &p : h)
    {
      double typical = (p.high + p.low + p.close) / 3.0;
      pv += typical * static_cast<double>(p.volume);
      vol += static_cast<double>(p.volume);
    }
    return vol == 0.0 ? -1.0 : pv / vol;
  }
  
  double StockAnalyzer::ComputeAverageVolume(const std::vector<StockPoint>& h, int period)
  {
    if (period <= 0 || h.size() < (size_t)period)
    {
      return -1.0;
    }
    
    double sum = 0.0;
    for (size_t i = h.size() - period; i < h.size(); ++i)
    {
      sum += static_cast<double>(h[i].volume);
    }
    return sum / period;
  }
  
  double StockAnalyzer::ComputeMACD(const std::vector<StockPoint>& h, int shortPeriod, int longPeriod)
  {
    if (h.size() < static_cast<size_t>(longPeriod))
    {
      return 0.0; // Not enough data
    }
    
    // --- Step 1: Compute short and long EMAs ---
    double emaShort = ComputeEMA(h, shortPeriod);
    double emaLong  = ComputeEMA(h, longPeriod);
    
    // --- Step 2: Compute MACD line ---
    double macdLine = emaShort - emaLong;
    
    // --- Step 3: Compute signal line (9-period EMA of MACD values) ---
    // To do this properly, we need recent MACD values — we’ll generate them.
    std::vector<double> macdValues;
    macdValues.reserve(h.size() - longPeriod + 1);
    
    for (size_t i = longPeriod - 1; i < h.size(); ++i)
    {
      std::vector<StockPoint> subset(h.begin(), h.begin() + i + 1);
      double emaS = ComputeEMA(subset, shortPeriod);
      double emaL = ComputeEMA(subset, longPeriod);
      macdValues.push_back(emaS - emaL);
    }
    
    // Apply EMA to MACD values to get signal line
    if (macdValues.size() < 9) return macdLine; // fallback
    double k = 2.0 / (9.0 + 1.0);
    double signal = macdValues.front();
    for (size_t i = 1; i < macdValues.size(); ++i)
    {
      signal = macdValues[i] * k + signal * (1 - k);
    }
    
    // --- Step 4: Histogram (momentum strength indicator) ---
    double histogram = macdLine - signal;
    
    // Optionally: you can store these into StockSummary later
    // For now, we just return the MACD histogram for directional strength
    return histogram;
  }

  static time_t portable_timegm(struct tm *tm)
  {
#if defined(_WIN32)
    // Windows has _mkgmtime
    return _mkgmtime(tm);
#else
#ifdef __USE_BSD
    return timegm(tm);
#else
    // fallback: adjust mktime using timezone (less perfect; acceptable for grouping)
    // create local tm then reverse? Simpler fallback: use mktime (assumes system tz is UTC)
    return timegm(tm); // on posix should exist; if not, build a safe alternative
#endif
#endif
  }
  
  time_t StockAnalyzer::DayKey(double ts)
  {
    time_t t = static_cast<time_t>(ts);
    struct tm tm{};
#if defined(_WIN32)
    gmtime_s(&tm, &t);
#else
    gmtime_r(&t, &tm);
#endif
    tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
    return portable_timegm(&tm);
  }
  
  time_t StockAnalyzer::WeekKey(double ts)
  {
    time_t t = static_cast<time_t>(ts);
    struct tm tm{};
#if defined(_WIN32)
    gmtime_s(&tm, &t);
#else
    gmtime_r(&t, &tm);
#endif
    // convert to Monday = 0
    int wday = (tm.tm_wday + 6) % 7;
    tm.tm_mday -= wday;
    tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
    return portable_timegm(&tm);
  }
  
  std::vector<StockPoint> StockAnalyzer::Aggregate(const std::vector<StockPoint>& input, const std::string& targetInterval)
  {
    std::vector<StockPoint> out;
    if (input.empty())
    {
      return out;
    }
    
    std::function<time_t(double)> keyFn = (targetInterval == "1wk") ? WeekKey : DayKey;
    std::map<time_t, std::vector<StockPoint>> groups;
    for (const auto &p : input)
    {
      groups[keyFn(static_cast<double>(p.timestamp))].push_back(p);
    }
    
    for (auto &kv : groups)
    {
      const auto &vec = kv.second;
      if (vec.empty()) continue;
      StockPoint agg{};
      agg.timestamp = static_cast<uint64_t>(kv.first);
      agg.open = vec.front().open;
      agg.close = vec.back().close;
      agg.high = vec.front().high;
      agg.low = vec.front().low;
      agg.volume = 0;
      for (const auto &c : vec)
      {
        agg.high = std::max(agg.high, c.high);
        agg.low = std::min(agg.low, c.low);
        agg.volume += c.volume;
      }
      out.push_back(agg);
    }
    std::sort(out.begin(), out.end(), [](const StockPoint &a, const StockPoint &b){ return a.timestamp < b.timestamp; });
    return out;
  }
  
  IndicatorConfig StockAnalyzer::ChooseConfig(const std::string& interval,
                                              const std::string& range)
  {
    IndicatorConfig cfg;
    
    if (interval == "1m" || interval == "2m" || interval == "5m") {
      // Intraday
      cfg.smaPeriod = 50;  // more smoothing
      cfg.emaPeriod = 50;
      cfg.rsiPeriod = 14;
      cfg.atrPeriod = 14;
      cfg.volPeriod = 50;
      cfg.momentumSensitivity = 0.008;
    }
    else if (interval == "1h" || interval == "15m") {
      // Short-term swing
      cfg.smaPeriod = 30;
      cfg.emaPeriod = 30;
      cfg.rsiPeriod = 14;
      cfg.atrPeriod = 14;
      cfg.volPeriod = 30;
    }
    else if (interval == "1d") {
      // Standard daily analysis
      cfg.smaPeriod = 20;
      cfg.emaPeriod = 20;
      cfg.rsiPeriod = 14;
      cfg.atrPeriod = 14;
      cfg.volPeriod = 20;
    }
    else if (interval == "1wk") {
      // Long-term
      cfg.smaPeriod = 10;
      cfg.emaPeriod = 10;
      cfg.rsiPeriod = 10;
      cfg.atrPeriod = 10;
      cfg.volPeriod = 10;
      cfg.momentumSensitivity = 0.02;
    }
    
    return cfg;
  }
  StockSummary StockAnalyzer::AnalyzeInternal(const std::vector<StockPoint>& h,
                                              const IndicatorConfig& cfg,
                                              const std::string& interval)
  {
    StockSummary result;
    if (h.size() < static_cast<size_t>(cfg.smaPeriod))
    {
      result.suggestion = "Not enough data.";
      return result;
    }
    
    double close = h.back().close;
    double sma = ComputeSMA(h, cfg.smaPeriod);
    double ema = ComputeEMA(h, cfg.emaPeriod);
    double rsi = ComputeRSI(h, cfg.rsiPeriod);
    double atr = ComputeATR(h, cfg.atrPeriod);
    double vwap = ComputeVWAP(h);
    double macd = ComputeMACD(h, 12, 26);
    double avgVol = ComputeAverageVolume(h, cfg.volPeriod);
    double latestVol = h.back().volume;
    
    // Trend
    if (close > ema * 1.01 && ema > sma)
      result.trend = "Uptrend";
    else if (close < ema * 0.99 && ema < sma)
      result.trend = "Downtrend";
    else
      result.trend = "Sideways";
    
    // Momentum
    double emaDiff = std::fabs(ema - sma) / sma;
    if (emaDiff > cfg.momentumSensitivity * 2)
      result.momentum = "Strong";
    else if (emaDiff > cfg.momentumSensitivity)
      result.momentum = "Moderate";
    else
      result.momentum = "Weak";
    
    // Volatility
    double atrPercent = (atr / close) * 100.0;
    if (atrPercent > 3.0)
      result.volatility = "High";
    else if (atrPercent > 1.5)
      result.volatility = "Medium";
    else
      result.volatility = "Low";
    
    // Volume
    double volRatio = SafeDiv(latestVol, avgVol, 1.0);
    if (volRatio > cfg.volHighFactor)
      result.volume = "High";
    else if (volRatio < cfg.volLowFactor)
      result.volume = "Low";
    else
      result.volume = "Normal";
    
    // RSI
    if (rsi > 70)
      result.valuation = "Overbought";
    else if (rsi < 30)
      result.valuation = "Oversold";
    else
      result.valuation = "Fair";
    
    // score score
    double trendScore = (result.trend == "Uptrend") ? 1.0 :
    (result.trend == "Downtrend") ? -1.0 : 0.0;
    double rsiScore = (rsi - 50.0) / 50.0;
    double macdScore = macd / (close * 0.01);
    
    double score = (trendScore * 0.4 + rsiScore * 0.3 + macdScore * 0.3);
    result.score = std::clamp(score, -1.0, 1.0);
    
    if (result.score > 0.4)
      result.suggestion = "Buy — bullish momentum.";
    else if (result.score < -0.4)
      result.suggestion = "Sell — bearish pressure.";
    else
      result.suggestion = "Hold — uncertain phase.";
    
    return result;
  }
  
  // ---------------- Public APIs ----------------
  StockSummary StockAnalyzer::AnalyzeSingleTimeframe(const StockData& data,
                                                     const std::string& interval,
                                                     const std::string& range)
  {
    auto cfg = ChooseConfig(interval, range);
    return AnalyzeInternal(data.history, cfg, interval);
  }
  
  StockSummary StockAnalyzer::AnalyzeHybrid(const StockData& shortTerm,
                                            const StockData& longTerm,
                                            const std::string& shortInterval,
                                            const std::string& longInterval)
  {
    auto shortCfg = ChooseConfig(shortInterval, "short");
    auto longCfg  = ChooseConfig(longInterval, "long");
    
    auto shortSummary = AnalyzeInternal(shortTerm.history, shortCfg, shortInterval);
    auto longSummary  = AnalyzeInternal(longTerm.history, longCfg, longInterval);
    
    StockSummary hybrid;
    
    // Blend conclusions
    hybrid.trend =
    (shortSummary.trend == longSummary.trend)
    ? shortSummary.trend
    : "Mixed";
    
    hybrid.momentum = shortSummary.momentum;
    hybrid.volatility = shortSummary.volatility;
    hybrid.volume = shortSummary.volume;
    hybrid.valuation = shortSummary.valuation;
    
    // score fusion
    hybrid.score = (shortSummary.score * 0.6 + longSummary.score * 0.4);
    
    // Suggestion
    if (hybrid.score > 0.5)
      hybrid.suggestion = "Buy — both short and long trends bullish.";
    else if (hybrid.score < -0.5)
      hybrid.suggestion = "Sell — both trends bearish.";
    else
      hybrid.suggestion = "Hold — timeframes disagree.";
    
    return hybrid;
  }
} // namespace KanVest
