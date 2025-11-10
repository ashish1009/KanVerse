//
//  MathUtils.cpp
//  KanVest
//
//  Created by Ashish . on 10/11/25.
//

#include "TechnicalUtils.hpp"

namespace KanVest
{
  int GetNumberOfTradingDays(const std::vector<StockPoint>& history)
  {
    std::set<std::time_t> uniqueDays;
    
    for (const auto& point : history)
    {
      std::time_t ts = point.timestamp; // UNIX timestamp
      std::tm* t = std::localtime(&ts);
      // Normalize to date only (ignore time)
      std::tm dateOnly = *t;
      dateOnly.tm_hour = 0;
      dateOnly.tm_min = 0;
      dateOnly.tm_sec = 0;
      
      std::time_t dayStart = std::mktime(&dateOnly);
      uniqueDays.insert(dayStart);
    }
    
    return static_cast<int>(uniqueDays.size());
  }
  
  // --------------------------
  // Simple Moving Average (SMA)
  // --------------------------
  double TechnicalUtils::ComputeSMA(const std::vector<StockPoint>& history, int periodInDays)
  {
    int barsPerDay = static_cast<int>(history.size() / GetNumberOfTradingDays(history));

    int requiredBars = periodInDays * barsPerDay;
    if (history.size() < requiredBars) return 0.0; // or partial SMA
    
    double sum = 0.0;
    for (size_t i = history.size() - requiredBars; i < history.size(); ++i)
    {
      sum += history[i].close;
    }
    return sum / requiredBars;
  }

  // --------------------------
  // Exponential Moving Average (EMA)
  // --------------------------
  double TechnicalUtils::ComputeEMA(const std::vector<StockPoint>& history, int periodInDays)
  {
    if (history.empty()) return 0.0;
    
    int numDays = GetNumberOfTradingDays(history);
    if (numDays == 0) return 0.0;
    
    int barsPerDay = static_cast<int>(history.size() / numDays);
    int requiredBars = periodInDays * barsPerDay;
    
    if (history.size() < requiredBars)
      return 0.0; // not enough data
    
    // Initial EMA = SMA of the first "requiredBars"
    double ema = 0.0;
    for (size_t i = history.size() - requiredBars; i < history.size() - requiredBars + periodInDays; ++i)
      ema += history[i].close;
    ema /= periodInDays;
    
    double multiplier = 2.0 / (periodInDays + 1.0);
    
    for (size_t i = history.size() - requiredBars + periodInDays; i < history.size(); ++i)
    {
      ema = ((history[i].close - ema) * multiplier) + ema;
    }
    
    return ema;
  }

  // --------------------------
  // Relative Strength Index (RSI)
  // --------------------------
  double TechnicalUtils::ComputeRSI(const std::vector<StockPoint>& history, int periodInDays)
  {
    if (history.size() < 2) return 50.0; // neutral
    
    // --- Step 1: Determine bars per day ---
    int numDays = GetNumberOfTradingDays(history);
    if (numDays == 0) return 50.0;
    
    int barsPerDay = static_cast<int>(history.size() / numDays);
    int periodInBars = std::max(1, periodInDays * barsPerDay);
    if (history.size() <= static_cast<size_t>(periodInBars)) return 50.0;
    
    // --- Step 2: Compute initial average gain/loss over first period ---
    double avgGain = 0.0, avgLoss = 0.0;
    for (int i = 1; i <= periodInBars; ++i)
    {
      double diff = history[i].close - history[i - 1].close;
      if (diff > 0) avgGain += diff;
      else avgLoss -= diff;
    }
    avgGain /= periodInBars;
    avgLoss /= periodInBars;
    
    // --- Step 3: Smooth the averages (Wilder's method) ---
    for (size_t i = periodInBars + 1; i < history.size(); ++i)
    {
      double diff = history[i].close - history[i - 1].close;
      double gain = diff > 0 ? diff : 0.0;
      double loss = diff < 0 ? -diff : 0.0;
      
      avgGain = ((avgGain * (periodInBars - 1)) + gain) / periodInBars;
      avgLoss = ((avgLoss * (periodInBars - 1)) + loss) / periodInBars;
    }
    
    // --- Step 4: Compute RSI ---
    if (avgLoss == 0) return 100.0;
    double rs = avgGain / avgLoss;
    double rsi = 100.0 - (100.0 / (1.0 + rs));
    
    return rsi;
  }

  std::vector<double> ComputeEMASeries(const std::vector<double>& data, int period)
  {
    std::vector<double> result;
    if (data.size() < static_cast<size_t>(period)) return result;
    
    double multiplier = 2.0 / (period + 1);
    double ema = std::accumulate(data.begin(), data.begin() + period, 0.0) / period; // initial SMA
    result.push_back(ema);
    
    for (size_t i = period; i < data.size(); ++i)
    {
      ema = ((data[i] - ema) * multiplier) + ema;
      result.push_back(ema);
    }
    return result;
  }

  // --------------------------
  // MACD and Signal line
  // --------------------------
  std::tuple<double, double> TechnicalUtils::ComputeMACD(
                                                         const std::vector<StockPoint>& history,
                                                         int fastPeriodInDays,
                                                         int slowPeriodInDays,
                                                         int signalPeriodInDays)
  {
    if (history.size() < slowPeriodInDays) return {0.0, 0.0};
    
    // --- Step 1: Handle intraday scaling ---
    int numDays = GetNumberOfTradingDays(history);
    if (numDays == 0) return {0.0, 0.0};
    
    int barsPerDay = static_cast<int>(history.size() / numDays);
    int fastBars = fastPeriodInDays * barsPerDay;
    int slowBars = slowPeriodInDays * barsPerDay;
    int signalBars = signalPeriodInDays * barsPerDay;
    
    // --- Step 2: Extract closes ---
    std::vector<double> closes;
    closes.reserve(history.size());
    for (auto &h : history)
      closes.push_back(h.close);
    
    // --- Step 3: Compute EMA series (single pass, efficient) ---
    std::vector<double> emaFast = ComputeEMASeries(closes, fastBars);
    std::vector<double> emaSlow = ComputeEMASeries(closes, slowBars);
    
    // Align sizes
    size_t start = slowBars - fastBars;
    if (emaFast.size() <= start) return {0.0, 0.0};
    
    std::vector<double> macdLine;
    macdLine.reserve(emaSlow.size());
    for (size_t i = 0; i < emaSlow.size(); ++i)
    {
      size_t idxFast = i + start;
      if (idxFast < emaFast.size())
        macdLine.push_back(emaFast[idxFast] - emaSlow[i]);
    }
    
    if (macdLine.empty()) return {0.0, 0.0};
    
    // --- Step 4: Compute Signal Line (EMA of MACD line) ---
    std::vector<double> signalLine = ComputeEMASeries(macdLine, signalBars);
    
    // --- Step 5: Return latest values ---
    double macd = macdLine.back();
    double signal = signalLine.back();
    
    return {macd, signal};
  }

  // --------------------------
  // Average True Range (ATR)
  // --------------------------
  double TechnicalUtils::ComputeATR(const std::vector<StockPoint>& history, int periodInDays)
  {
    if (history.size() < 2) return 0.0;
    
    // --- Step 1: Handle intraday scaling ---
    int numDays = GetNumberOfTradingDays(history);
    if (numDays == 0) return 0.0;
    
    int barsPerDay = static_cast<int>(history.size() / numDays);
    int period = std::max(1, periodInDays * barsPerDay);
    
    if (history.size() <= static_cast<size_t>(period)) return 0.0;
    
    // --- Step 2: Compute True Range (TR) for each bar ---
    std::vector<double> trValues;
    trValues.reserve(history.size() - 1);
    for (size_t i = 1; i < history.size(); ++i)
    {
      double highLow   = history[i].high - history[i].low;
      double highClose = std::abs(history[i].high - history[i - 1].close);
      double lowClose  = std::abs(history[i].low - history[i - 1].close);
      double tr = std::max({highLow, highClose, lowClose});
      trValues.push_back(tr);
    }
    
    // --- Step 3: Initial ATR = SMA of first 'period' TRs ---
    if (trValues.size() < static_cast<size_t>(period)) return 0.0;
    double atr = std::accumulate(trValues.begin(), trValues.begin() + period, 0.0) / period;
    
    // --- Step 4: Wilder’s smoothing for remaining TRs ---
    double alpha = 1.0 / period;
    for (size_t i = period; i < trValues.size(); ++i)
    {
      atr = atr + alpha * (trValues[i] - atr);
    }

    return atr;
  }

  // --------------------------
  // Volume Weighted Average Price (VWAP)
  // --------------------------
  double TechnicalUtils::ComputeVWAP(const std::vector<StockPoint>& history)
  {
    if (history.empty()) return 0.0;
    
    double cumulativePriceVolume = 0.0;
    double cumulativeVolume = 0.0;
    
    // Detect current day for session VWAP
    time_t lastDay = 0;
    if (history.back().timestamp > 0)
    {
      // Align to midnight (truncate time to day)
      time_t ts = static_cast<time_t>(history.back().timestamp);
      tm* t = std::gmtime(&ts);
      t->tm_hour = 0; t->tm_min = 0; t->tm_sec = 0;
      lastDay = std::mktime(t);
    }
    
    for (auto it = history.rbegin(); it != history.rend(); ++it)
    {
      const auto& point = *it;
      
      // Detect if this bar belongs to the current (latest) day
      if (point.timestamp > 0)
      {
        time_t ts = static_cast<time_t>(point.timestamp);
        tm* p = std::gmtime(&ts);
        p->tm_hour = 0; p->tm_min = 0; p->tm_sec = 0;
        time_t currentBarDay = std::mktime(p);
        
        if (currentBarDay < lastDay)
          break; // stop when we reach previous day (VWAP resets daily)
      }
      
      if (point.volume <= 0) continue; // skip zero-volume bars
      
      double typicalPrice = (point.high + point.low + point.close) / 3.0;
      cumulativePriceVolume += typicalPrice * point.volume;
      cumulativeVolume += point.volume;
    }
    
    if (cumulativeVolume == 0.0)
      return 0.0;
    
    return cumulativePriceVolume / cumulativeVolume;
  }
  
  // =============================
  // Additional Technical Indicators
  // Awesome Oscillator, Stochastic RSI, CCI
  // =============================
  
  double TechnicalUtils::ComputeAwesomeOscillator(const std::vector<StockPoint>& history, int fastPeriodInDays, int slowPeriodInDays)
  {
    if (history.size() < 2) return 0.0;
    
    int numDays = GetNumberOfTradingDays(history);
    if (numDays == 0) return 0.0;
    
    int barsPerDay = static_cast<int>(history.size() / numDays);
    int fastPeriod = std::max(1, fastPeriodInDays * barsPerDay);
    int slowPeriod = std::max(1, slowPeriodInDays * barsPerDay);
    
    if (history.size() < static_cast<size_t>(slowPeriod)) return 0.0;
    
    std::vector<double> medianPrices;
    medianPrices.reserve(history.size());
    for (const auto& h : history)
      medianPrices.push_back((h.high + h.low) / 2.0);
    
    double smaFast = 0.0, smaSlow = 0.0;
    for (size_t i = history.size() - fastPeriod; i < history.size(); ++i) smaFast += medianPrices[i];
    for (size_t i = history.size() - slowPeriod; i < history.size(); ++i) smaSlow += medianPrices[i];
    
    smaFast /= fastPeriod;
    smaSlow /= slowPeriod;
    
    return smaFast - smaSlow;
  }
  
  
  // =============================
  // Stochastic RSI
  // =============================
  double TechnicalUtils::ComputeStochasticRSI(const std::vector<StockPoint>& history, int periodInDays)
  {
    if (history.size() < 2) return 0.0;
    
    int numDays = GetNumberOfTradingDays(history);
    if (numDays == 0) return 0.0;
    
    int barsPerDay = static_cast<int>(history.size() / numDays);
    int period = std::max(1, periodInDays * barsPerDay);
    if (history.size() < static_cast<size_t>(period)) return 0.0;
    
    // Compute RSI series
    std::vector<double> rsiSeries;
    for (size_t i = period; i < history.size(); ++i)
    {
      std::vector<StockPoint> sub(history.begin() + i - period, history.begin() + i);
      rsiSeries.push_back(ComputeRSI(sub, period));
    }
    
    if (rsiSeries.size() < static_cast<size_t>(period)) return 50.0; // neutral
    
    double recentRSI = rsiSeries.back();
    auto [minRSI, maxRSI] = std::minmax_element(rsiSeries.end() - period, rsiSeries.end());
    if (*maxRSI - *minRSI == 0) return 50.0;
    
    double stochasticRSI = (recentRSI - *minRSI) / (*maxRSI - *minRSI) * 100.0;
    return std::clamp(stochasticRSI, 0.0, 100.0);
  }
  
  
  // =============================
  // Commodity Channel Index (CCI)
  // =============================
  double TechnicalUtils::ComputeCCI(const std::vector<StockPoint>& history, int periodInDays)
  {
    if (history.size() < 2) return 0.0;
    
    int numDays = GetNumberOfTradingDays(history);
    if (numDays == 0) return 0.0;
    
    int barsPerDay = static_cast<int>(history.size() / numDays);
    int period = std::max(1, periodInDays * barsPerDay);
    
    if (history.size() < static_cast<size_t>(period)) return 0.0;
    
    std::vector<double> typicalPrices;
    typicalPrices.reserve(history.size());
    for (const auto& h : history)
      typicalPrices.push_back((h.high + h.low + h.close) / 3.0);
    
    double sma = std::accumulate(typicalPrices.end() - period, typicalPrices.end(), 0.0) / period;
    
    double meanDeviation = 0.0;
    for (size_t i = history.size() - period; i < history.size(); ++i)
      meanDeviation += std::abs(typicalPrices[i] - sma);
    
    meanDeviation /= period;
    if (meanDeviation == 0.0) return 0.0;
    
    double cci = (typicalPrices.back() - sma) / (0.015 * meanDeviation);
    return cci;
  }

} // namespace KanVest
