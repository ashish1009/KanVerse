//
//  MathUtils.cpp
//  KanVest
//
//  Created by Ashish . on 10/11/25.
//

#include "TechnicalUtils.hpp"

namespace KanVest
{
  int TechnicalUtils::GetNumberOfTradingDays(const std::vector<StockPoint>& history)
  {
    IK_PERFORMANCE_FUNC("TechnicalUtils::GetNumberOfTradingDays");
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
  
  bool TechnicalUtils::ResolvePeriods(const std::vector<StockPoint>& history,
                                    int periodInDays,
                                    int &outPeriodBars,
                                    int &outBarsPerDay)
  {
    IK_PERFORMANCE_FUNC("TechnicalUtils::ResolvePeriods");
    if (history.empty()) return false;
    int numDays = GetNumberOfTradingDays(history);
    if (numDays <= 0) return false;
    outBarsPerDay = static_cast<int>(history.size() / numDays);
    outPeriodBars = std::max(1, periodInDays * outBarsPerDay);
    return true;
  }
  
  // --------------------------
  // Simple Moving Average (SMA)
  // --------------------------
  double TechnicalUtils::ComputeSMA(const std::vector<StockPoint>& history, int periodInDays)
  {
    IK_PERFORMANCE_FUNC("TechnicalUtils::ComputeSMA");
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
    IK_PERFORMANCE_FUNC("TechnicalUtils::ComputeEMA");
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
    IK_PERFORMANCE_FUNC("TechnicalUtils::ComputeRSI");
    const size_t n = history.size();
    if (n < 2)
      return 50.0;
    
    // --- Step 1: Detect time interval ---
    uint64_t totalGap = 0;
    int gapCount = 0;
    for (size_t i = 1; i < std::min(n, size_t(100)); ++i)
    {
      uint64_t gap = history[i].timestamp - history[i - 1].timestamp;
      if (gap > 0 && gap < 86400 * 7) // ignore missing week gaps
      {
        totalGap += gap;
        ++gapCount;
      }
    }
    
    double avgGap = (gapCount > 0) ? (double)totalGap / gapCount : 86400.0;
    bool isIntraday = (avgGap < 12 * 3600); // less than 12 hours = intraday
    
    // --- Step 2: If intraday, use bar-based RSI ---
    if (isIntraday)
    {
      int period = periodInDays; // RSI(14) means 14 bars for intraday
      if (n <= static_cast<size_t>(period))
        return 50.0;
      
      double avgGain = 0.0, avgLoss = 0.0;
      for (int i = 1; i <= period; ++i)
      {
        double diff = history[i].close - history[i - 1].close;
        if (diff > 0)
          avgGain += diff;
        else
          avgLoss -= diff;
      }
      avgGain /= period;
      avgLoss /= period;
      
      for (size_t i = period + 1; i < n; ++i)
      {
        double diff = history[i].close - history[i - 1].close;
        double gain = diff > 0 ? diff : 0.0;
        double loss = diff < 0 ? -diff : 0.0;
        
        avgGain = ((avgGain * (period - 1)) + gain) / period;
        avgLoss = ((avgLoss * (period - 1)) + loss) / period;
      }
      
      if (avgLoss == 0.0)
        return 100.0;
      
      double rs = avgGain / avgLoss;
      return 100.0 - (100.0 / (1.0 + rs));
    }
    
    // --- Step 3: If daily/long interval, aggregate by day ---
    std::vector<std::pair<uint64_t, double>> dailyCloses;
    uint64_t currentDay = 0;
    
    for (const auto& p : history)
    {
      uint64_t day = p.timestamp / 86400;
      if (dailyCloses.empty() || day != currentDay)
      {
        dailyCloses.emplace_back(day, p.close);
        currentDay = day;
      }
      else
      {
        // update last close for same day
        dailyCloses.back().second = p.close;
      }
    }
    
    const size_t dn = dailyCloses.size();
    int period = periodInDays;
    if (dn <= static_cast<size_t>(period))
      return 50.0;
    
    double avgGain = 0.0, avgLoss = 0.0;
    for (int i = 1; i <= period; ++i)
    {
      double diff = dailyCloses[i].second - dailyCloses[i - 1].second;
      if (diff > 0)
        avgGain += diff;
      else
        avgLoss -= diff;
    }
    avgGain /= period;
    avgLoss /= period;
    
    for (size_t i = period + 1; i < dn; ++i)
    {
      double diff = dailyCloses[i].second - dailyCloses[i - 1].second;
      double gain = diff > 0 ? diff : 0.0;
      double loss = diff < 0 ? -diff : 0.0;
      
      avgGain = ((avgGain * (period - 1)) + gain) / period;
      avgLoss = ((avgLoss * (period - 1)) + loss) / period;
    }
    
    if (avgLoss == 0.0)
      return 100.0;
    
    double rs = avgGain / avgLoss;
    return 100.0 - (100.0 / (1.0 + rs));
  }

  std::vector<double> ComputeEMASeries(const std::vector<double>& data, int period)
  {
    IK_PERFORMANCE_FUNC("TechnicalUtils::ComputeEMASeries");
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
    IK_PERFORMANCE_FUNC("TechnicalUtils::ComputeMACD");
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
    IK_PERFORMANCE_FUNC("TechnicalUtils::ComputeATR");
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
    IK_PERFORMANCE_FUNC("TechnicalUtils::ComputeVWAP");
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
    IK_PERFORMANCE_FUNC("TechnicalUtils::ComputeAwesomeOscillator");
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
    IK_PERFORMANCE_FUNC("TechnicalUtils::ComputeStochasticRSI");
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
    IK_PERFORMANCE_FUNC("TechnicalUtils::ComputeCCI");
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

  double TechnicalUtils::ComputeADX(const std::vector<StockPoint>& history, int periodInDays, double &outPlusDI, double &outMinusDI)
  {
    IK_PERFORMANCE_FUNC("TechnicalUtils::ComputeADX");
    outPlusDI = 0.0;
    outMinusDI = 0.0;
    if (history.size() < 2) return 0.0;
    
    int barsPerDay = 1, period = 0;
    if (!ResolvePeriods(history, periodInDays, period, barsPerDay)) return 0.0;
    if (history.size() <= static_cast<size_t>(period)) return 0.0;
    
    // step 1: compute TR, +DM, -DM for each bar (starting from i=1)
    std::vector<double> tr(history.size(), 0.0);
    std::vector<double> plusDM(history.size(), 0.0);
    std::vector<double> minusDM(history.size(), 0.0);
    
    for (size_t i = 1; i < history.size(); ++i)
    {
      double high = history[i].high;
      double low = history[i].low;
      double prevHigh = history[i - 1].high;
      double prevLow = history[i - 1].low;
      double prevClose = history[i - 1].close;
      
      double highLow = high - low;
      double highClose = std::abs(high - prevClose);
      double lowClose = std::abs(low - prevClose);
      tr[i] = std::max({highLow, highClose, lowClose});
      
      double upMove = high - prevHigh;
      double downMove = prevLow - low;
      
      plusDM[i] = (upMove > downMove && upMove > 0) ? upMove : 0.0;
      minusDM[i] = (downMove > upMove && downMove > 0) ? downMove : 0.0;
    }
    
    // step 2: Wilder smoothing of TR, +DM, -DM
    // initial smoothed values = sum of first 'period' values (from i=1..period)
    double trSum = 0.0, plusDMSum = 0.0, minusDMSum = 0.0;
    for (int i = 1; i <= period; ++i)
    {
      trSum += tr[i];
      plusDMSum += plusDM[i];
      minusDMSum += minusDM[i];
    }
    double smTR = trSum;
    double smPlusDM = plusDMSum;
    double smMinusDM = minusDMSum;
    
    std::vector<double> plusDIhist;
    std::vector<double> minusDIhist;
    std::vector<double> dxhist;
    
    // compute subsequent smoothed values and DI, DX
    for (size_t i = period + 1; i < tr.size(); ++i)
    {
      // Wilder smoothing: sm = (prevSm * (period - 1) + value) / period
      smTR = (smTR * (period - 1) + tr[i]) / period;
      smPlusDM = (smPlusDM * (period - 1) + plusDM[i]) / period;
      smMinusDM = (smMinusDM * (period - 1) + minusDM[i]) / period;
      
      double pDI = (smTR == 0.0) ? 0.0 : 100.0 * (smPlusDM / smTR);
      double mDI = (smTR == 0.0) ? 0.0 : 100.0 * (smMinusDM / smTR);
      
      plusDIhist.push_back(pDI);
      minusDIhist.push_back(mDI);
      
      double denom = pDI + mDI;
      double dx = (denom == 0.0) ? 0.0 : 100.0 * std::abs(pDI - mDI) / denom;
      dxhist.push_back(dx);
    }
    
    if (dxhist.empty())
    {
      outPlusDI = plusDIhist.empty() ? 0.0 : plusDIhist.back();
      outMinusDI = minusDIhist.empty() ? 0.0 : minusDIhist.back();
      return 0.0;
    }
    
    // step 3: ADX = Wilder smoothed average of DX (use period)
    // initial ADX = average of first 'period' DX values
    if (dxhist.size() < static_cast<size_t>(period))
    {
      // average whatever we have
      double s = std::accumulate(dxhist.begin(), dxhist.end(), 0.0);
      outPlusDI = plusDIhist.back();
      outMinusDI = minusDIhist.back();
      return s / dxhist.size();
    }
    
    double adx = 0.0;
    // first ADX seed: average of first 'period' DXs
    for (size_t i = 0; i < static_cast<size_t>(period); ++i) adx += dxhist[i];
    adx /= period;
    
    // then Wilder smoothing for remaining DXs
    for (size_t i = period; i < dxhist.size(); ++i)
      adx = ( (adx * (period - 1)) + dxhist[i] ) / period;
    
    outPlusDI = plusDIhist.back();
    outMinusDI = minusDIhist.back();
    
    return adx;
  }
  
  // ---------------- MFI ----------------
  double TechnicalUtils::ComputeMFI(const std::vector<StockPoint>& history, int periodInDays)
  {
    IK_PERFORMANCE_FUNC("TechnicalUtils::ComputeMFI");
    if (history.size() < 2) return 50.0;
    
    int barsPerDay = 1, period = 0;
    if (!ResolvePeriods(history, periodInDays, period, barsPerDay)) return 50.0;
    if (history.size() <= static_cast<size_t>(period)) return 50.0;
    
    // compute typical price and raw money flow
    std::vector<double> tp(history.size(), 0.0);
    std::vector<double> moneyFlow(history.size(), 0.0);
    
    for (size_t i = 0; i < history.size(); ++i)
    {
      tp[i] = (history[i].high + history[i].low + history[i].close) / 3.0;
      moneyFlow[i] = tp[i] * static_cast<double>(history[i].volume);
    }
    
    // now compute positive and negative money flow over the last 'period' bars
    double positiveFlow = 0.0, negativeFlow = 0.0;
    for (size_t i = history.size() - period + 1; i < history.size(); ++i)
    {
      // compare typical price to previous bar's typical price
      size_t prev = i - 1;
      if (tp[i] > tp[prev]) positiveFlow += moneyFlow[i];
      else if (tp[i] < tp[prev]) negativeFlow += moneyFlow[i];
      // if equal, neither side increments
    }
    
    if (negativeFlow == 0.0)
    {
      return 100.0;
    }
    
    double moneyRatio = positiveFlow / negativeFlow;
    double mfi = 100.0 - (100.0 / (1.0 + moneyRatio));
    return std::clamp(mfi, 0.0, 100.0);
  }
  
  // ---------------- OBV ----------------
  double TechnicalUtils::ComputeOBV(const std::vector<StockPoint>& history)
  {
    IK_PERFORMANCE_FUNC("TechnicalUtils::ComputeOBV");
    if (history.empty()) return 0.0;
    // Use 64-bit accumulator because volume can be large
    long long obv = 0;
    for (size_t i = 1; i < history.size(); ++i)
    {
      if (history[i].close > history[i - 1].close) obv += static_cast<long long>(history[i].volume);
      else if (history[i].close < history[i - 1].close) obv -= static_cast<long long>(history[i].volume);
      // equal -> no change
    }
    return static_cast<double>(obv);
  }

} // namespace KanVest
