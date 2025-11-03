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
  
  IndicatorConfig StockAnalyzer::ChooseConfig(const std::string& interval, const std::string& range)
  {
    IndicatorConfig cfg; // defaults
    if (interval == "1m" && (range == "1d" || range == "5d"))
    {
      cfg.smaPeriod = 10; cfg.emaPeriod = 10; cfg.rsiPeriod = 7; cfg.atrPeriod = 7; cfg.volPeriod = 10;
      cfg.momentumSensitivity = 0.02;
    }
    else if (interval == "5m")
    {
      cfg.smaPeriod = 20; cfg.emaPeriod = 20; cfg.rsiPeriod = 14; cfg.atrPeriod = 14;
      cfg.momentumSensitivity = 0.015;
    }
    else if (interval == "15m")
    {
      cfg.smaPeriod = 30; cfg.emaPeriod = 30; cfg.rsiPeriod = 14; cfg.atrPeriod = 14;
      cfg.momentumSensitivity = 0.012;
    }
    else if (interval == "1h")
    {
      cfg.smaPeriod = 40; cfg.emaPeriod = 40; cfg.rsiPeriod = 14; cfg.atrPeriod = 14;
      cfg.momentumSensitivity = 0.01;
    }
    else if (interval == "1d")
    {
      if (range == "1mo") { cfg.smaPeriod = 20; cfg.emaPeriod = 20; cfg.rsiPeriod = 14; cfg.atrPeriod = 14; }
      else if (range == "3mo" || range == "6mo") { cfg.smaPeriod = 50; cfg.emaPeriod = 50; cfg.rsiPeriod = 20; cfg.atrPeriod = 20; }
    }
    else if (interval == "1wk")
    {
      cfg.smaPeriod = 20; cfg.emaPeriod = 20; cfg.rsiPeriod = 14; cfg.atrPeriod = 14;
      cfg.volHighFactor = 2.0;
    }
    return cfg;
  }
  
  StockSummary StockAnalyzer::AnalyzeInternal(const std::vector<StockPoint>& history, const std::string& interval, const std::string& range, const IndicatorConfig& cfg)
  {
    StockSummary out;
    if (history.size() < (size_t)cfg.smaPeriod)
    {
      out.conclusion = "Insufficient candles for analysis.";
      return out;
    }
    
    // compute indicators
    double sma = ComputeSMA(history, cfg.smaPeriod);
    double ema = ComputeEMA(history, cfg.emaPeriod);
    double rsi = ComputeRSI(history, cfg.rsiPeriod);
    double atr = ComputeATR(history, cfg.atrPeriod);
    double vwap = ComputeVWAP(history);
    double avgVol = ComputeAverageVolume(history, cfg.volPeriod);
    double latestVol = static_cast<double>(history.back().volume);
    double close = history.back().close;
    
    // adaptive thresholds based on volatility (ATR percent)
    double atrPct = (atr > 0.0 && close != 0.0) ? (atr / close) * 100.0 : 0.0;
    double trendThreshold = 1.0 + (atrPct / 200.0); // adapt to volatility
    
    // short-term confirmation: compare current close with close 3 candles ago when available
    bool recentUp = false, recentDown = false;
    if (history.size() >= 4)
    {
      recentUp = history.back().close > history[history.size() - 4].close;
      recentDown = history.back().close < history[history.size() - 4].close;
    }
    
    // Trend detection (multi-factor)
    if (close > sma * trendThreshold && ema > sma && (vwap < 0.0 || close > vwap) && recentUp)
      out.trend = "Uptrend";
    else if (close < sma / trendThreshold && ema < sma && (vwap < 0.0 || close < vwap) && recentDown)
      out.trend = "Downtrend";
    else
      out.trend = "Sideways";
    
    // Momentum - EMA vs SMA divergence
    double emaDiff = std::fabs(ema - sma) / (sma > 0.0 ? sma : 1.0);
    if (emaDiff > cfg.momentumSensitivity * 2.0) out.momentum = "Strong";
    else if (emaDiff > cfg.momentumSensitivity) out.momentum = "Moderate";
    else out.momentum = "Weak";
    
    // Volatility classification
    if (atrPct > 3.0) out.volatility = "High";
    else if (atrPct > 1.5) out.volatility = "Medium";
    else out.volatility = "Low";
    
    // Volume classification
    double volRatio = SafeDiv(latestVol, avgVol + 1.0, 1.0);
    if (volRatio > cfg.volHighFactor) out.volume = "High";
    else if (volRatio < cfg.volLowFactor) out.volume = "Low";
    else out.volume = "Normal";
    
    // Valuation using RSI + VWAP (if VWAP available)
    if (rsi > 70 && (vwap < 0.0 || close > vwap)) out.valuation = "Overbought";
    else if (rsi < 30 && (vwap < 0.0 || close < vwap)) out.valuation = "Oversold";
    else out.valuation = "Fair";
    
    // Compose human-readable summary
    std::ostringstream oss;
    oss << "Timeframe: " << interval << " / " << range << ". ";
    oss << "Trend: " << out.trend << ", Momentum: " << out.momentum << ", Volatility: " << out.volatility << ", Volume: " << out.volume << ". ";
    if (out.valuation == "Overbought") oss << "Price above VWAP & RSI elevated — caution for pullback.";
    else if (out.valuation == "Oversold") oss << "Price below VWAP & RSI low — possible rebound.";
    else oss << "Price near VWAP — balanced sentiment.";
    
    // Simple scoring heuristic (0..100)
    double score = 50.0;
    if (out.trend == "Uptrend") score += 20.0;
    else if (out.trend == "Downtrend") score -= 20.0;
    if (out.momentum == "Strong") score += 10.0;
    else if (out.momentum == "Weak") score -= 5.0;
    if (out.volume == "High") score += 5.0;
    if (out.valuation == "Overbought") score -= 8.0;
    else if (out.valuation == "Oversold") score += 8.0;
    if (out.volatility == "High") score -= 3.0;
    score = std::max(0.0, std::min(100.0, score));
    
    out.score = score;
    oss << " Score: " << static_cast<int>(score) << "/100.";
    out.conclusion = oss.str();
    
    return out;
  }
  
  StockSummary StockAnalyzer::AnalyzeSingleTimeframe(const StockData& data, const std::string& interval, const std::string& range)
  {
    // Defensive copy: we only need history for analysis
    IndicatorConfig cfg = ChooseConfig(interval, range);
    return AnalyzeInternal(data.history, interval, range, cfg);
  }
  
  StockSummary StockAnalyzer::AnalyzeHybrid(const StockData& baseData, const std::string& baseInterval, const std::string& baseRange)
  {
    // Choose config based on base interval/range
    IndicatorConfig cfg = ChooseConfig(baseInterval, baseRange);
    
    // Aggregate baseData.history to daily and weekly
    std::vector<StockPoint> daily = Aggregate(baseData.history, "1d");
    std::vector<StockPoint> weekly = Aggregate(baseData.history, "1wk");
    
    // If aggregated frames are empty or too short, callers should fetch more data, but we guard inside AnalyzeInternal
    StockSummary shortRes = AnalyzeInternal(baseData.history, baseInterval, baseRange, cfg);
    StockSummary midRes = AnalyzeInternal(daily, "1d", "1mo", cfg);
    StockSummary longRes = AnalyzeInternal(weekly, "1wk", "3mo", cfg);
    
    // Fuse results: weighted scoring (medium heavy)
    double finalScore = 0.0;
    finalScore += midRes.score * 0.5;
    finalScore += longRes.score * 0.3;
    finalScore += shortRes.score * 0.2;
    finalScore = std::max(0.0, std::min(100.0, finalScore));
    
    StockSummary final;
    final.trend = shortRes.trend + " (short) / " + midRes.trend + " (mid) / " + longRes.trend + " (long)";
    final.momentum = shortRes.momentum + " | " + midRes.momentum;
    final.volatility = midRes.volatility;
    final.volume = midRes.volume;
    final.valuation = midRes.valuation;
    std::ostringstream oss;
    oss << "Hybrid summary: short=" << shortRes.trend << ", mid=" << midRes.trend << ", long=" << longRes.trend << ". ";
    if (shortRes.trend == midRes.trend && midRes.trend == longRes.trend)
      oss << "All timeframes agree (" << midRes.trend << ").";
    else
    {
      oss << "Mixed signals — medium-term view: " << midRes.trend << ". ";
      if (shortRes.trend != midRes.trend) oss << "Short-term shows " << shortRes.trend << ". ";
      if (longRes.trend != midRes.trend) oss << "Long-term shows " << longRes.trend << ". ";
    }
    oss << "Overall score: " << static_cast<int>(finalScore) << "/100.";
    final.conclusion = oss.str();
    final.score = finalScore;
    return final;
  }
} // namespace KanVest
