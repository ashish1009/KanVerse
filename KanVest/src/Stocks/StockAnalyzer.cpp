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
    
  // -------------------------------------------------------------
  // 📊 Simple Moving Average (SMA)
  // -------------------------------------------------------------
  // SMA means “average closing price over the last N days”.
  // It shows the general direction of the market:
  //  - If current price > SMA → price trending up
  //  - If current price < SMA → price trending down
  // -------------------------------------------------------------
  double StockAnalyzer::ComputeSMA(const std::vector<StockPoint>& h, int period)
  {
    if (period <= 0 || h.size() < (size_t)period)
    {
      return -1.0; // Not enough data
    }
    
    double sum = 0.0;
    // Sum up the last 'period' closing prices
    for (size_t i = h.size() - period; i < h.size(); ++i)
    {
      sum += h[i].close;
    }
    // Divide total by how many days (the period)
    return sum / period;
  }
  
  // -------------------------------------------------------------
  // 📈 Exponential Moving Average (EMA)
  // -------------------------------------------------------------
  // EMA is similar to SMA, but gives more weight to recent prices.
  // So it reacts faster when price starts rising or falling.
  // -------------------------------------------------------------
  double StockAnalyzer::ComputeEMA(const std::vector<StockPoint>& h, int period)
  {
    if (period <= 0 || h.size() < (size_t)period)
    {
      return -1.0; // Not enough data
    }
    
    // Start by taking the simple average of the first few prices
    double ema = 0.0;
    size_t start = h.size() - period;
    for (size_t i = start; i < start + (size_t)period; ++i)
    {
      ema += h[i].close;
    }
    
    ema /= period;
    double k = 2.0 / (period + 1.0); // weight factor (recent data gets more importance)
    
    // Gradually adjust EMA based on new prices
    for (size_t i = start + 1; i < h.size(); ++i)
    {
      ema = (h[i].close * k) + (ema * (1.0 - k));
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
  double StockAnalyzer::ComputeRSI(const std::vector<StockPoint>& h, int period)
  {
    if (period <= 0 || h.size() <= (size_t)period)
    {
      return -1.0;
    }
    
    double gain = 0.0, loss = 0.0;
    
    // Look at each price difference and separate up moves (gain)
    // from down moves (loss)
    for (size_t i = h.size() - period + 1; i < h.size(); ++i)
    {
      double diff = h[i].close - h[i - 1].close;
      if (diff > 0)
      {
        gain += diff;
      }
      else
      {
        loss -= diff;
      }
    }
    
    if (gain == 0.0 && loss == 0.0)
    {
      return 50.0; // No movement at all
    }
    if (loss == 0.0)
    {
      return 100.0; // Only gains
    }
    
    double rs = gain / loss; // ratio of up vs down movement
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
  double StockAnalyzer::ComputeATR(const std::vector<StockPoint>& h, int period)
  {
    if (period <= 0 || h.size() < (size_t)period + 1)
    {
      return -1.0;
    }
    
    std::vector<double> tr; // true range values (daily price range measure)
    tr.reserve(h.size() - 1);
    
    for (size_t i = 1; i < h.size(); ++i)
    {
      double hl = h[i].high - h[i].low;                       // high-low range for the day
      double hc = std::fabs(h[i].high - h[i - 1].close);      // high to previous close
      double lc = std::fabs(h[i].low - h[i - 1].close);       // low to previous close
      tr.push_back(std::max({hl, hc, lc}));
    }
    
    double sum = 0.0;
    for (size_t i = tr.size() - period; i < tr.size(); ++i)
    {
      sum += tr[i];
    }
    
    // Average of last N true ranges
    return sum / period;
  }
  
  // -------------------------------------------------------------
  // ⚖️ VWAP (Volume Weighted Average Price)
  // -------------------------------------------------------------
  // VWAP is like the “average price people actually paid,”
  // because it weights each price by the amount traded.
  // So a price with high trading volume counts more.
  //
  // - Price above VWAP → more buyers, strong market
  // - Price below VWAP → more sellers, weak market
  // -------------------------------------------------------------
  double StockAnalyzer::ComputeVWAP(const std::vector<StockPoint>& h)
  {
    if (h.empty())
    {
      return -1.0;
    }
    
    double pv = 0.0;  // total price * volume
    double vol = 0.0; // total volume
    
    for (const auto &p : h)
    {
      double typical = (p.high + p.low + p.close) / 3.0; // average price of the day
      pv += typical * static_cast<double>(p.volume);
      vol += static_cast<double>(p.volume);
    }
    
    // If no volume data, return invalid
    return vol == 0.0 ? -1.0 : pv / vol;
  }
  
  // -------------------------------------------------------------
  // 🔊 Average Volume
  // -------------------------------------------------------------
  // Measures how much trading activity happens over time.
  // Helps detect if recent trading is unusually high or low.
  // -------------------------------------------------------------
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
  double StockAnalyzer::ComputeMACD(const std::vector<StockPoint>& h, int shortPeriod, int longPeriod)
  {
    if (h.size() < static_cast<size_t>(longPeriod))
    {
      return 0.0; // Not enough data
    }
    
    // --- Step 1: Compute short and long EMAs ---
    double emaShort = ComputeEMA(h, shortPeriod);
    double emaLong  = ComputeEMA(h, longPeriod);
    
    // --- Step 2: MACD line = difference between short & long EMAs ---
    double macdLine = emaShort - emaLong;
    
    // --- Step 3: Compute signal line (9-period EMA of MACD values) ---
    // To do this properly, we build a history of MACD values first.
    std::vector<double> macdValues;
    macdValues.reserve(h.size() - longPeriod + 1);
    
    for (size_t i = longPeriod - 1; i < h.size(); ++i)
    {
      std::vector<StockPoint> subset(h.begin(), h.begin() + i + 1);
      double emaS = ComputeEMA(subset, shortPeriod);
      double emaL = ComputeEMA(subset, longPeriod);
      macdValues.push_back(emaS - emaL);
    }
    
    if (macdValues.size() < 9)
      return macdLine; // Not enough MACD data to smooth out
    
    // Smooth MACD with EMA for the signal line
    double k = 2.0 / (9.0 + 1.0);
    double signal = macdValues.front();
    for (size_t i = 1; i < macdValues.size(); ++i)
    {
      signal = macdValues[i] * k + signal * (1 - k);
    }
    
    // Histogram = how far MACD is from signal → strength of shift
    double histogram = macdLine - signal;
    
    // We return histogram here (positive = strong up, negative = down)
    return histogram;
  }
  
  // -------------------------------------------------------------
  // 🕒 Portable UTC time conversion helper
  // -------------------------------------------------------------
  // Converts a 'tm' structure (calendar date) into a time_t value
  // representing seconds since 1970-01-01 UTC.
  // Works on Windows and macOS/Linux.
  // -------------------------------------------------------------
  static time_t portable_timegm(struct tm *tm)
  {
#if defined(_WIN32)
    return _mkgmtime(tm);
#else
    return timegm(tm);
#endif
  }
  
  
  // -------------------------------------------------------------
  // 📅 DayKey — group by calendar day
  // -------------------------------------------------------------
  // Takes a timestamp (in seconds) and returns a new timestamp
  // that represents midnight UTC of that same day.
  // Used to group many minute-based data points into one day.
  // -------------------------------------------------------------
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
  
  
  // -------------------------------------------------------------
  // 📆 WeekKey — group by week (starting Monday)
  // -------------------------------------------------------------
  // Finds which week the timestamp belongs to and returns
  // a timestamp pointing to Monday 00:00 UTC of that week.
  // -------------------------------------------------------------
  time_t StockAnalyzer::WeekKey(double ts)
  {
    time_t t = static_cast<time_t>(ts);
    struct tm tm{};
#if defined(_WIN32)
    gmtime_s(&tm, &t);
#else
    gmtime_r(&t, &tm);
#endif
    int wday = (tm.tm_wday + 6) % 7; // shift so Monday = 0
    tm.tm_mday -= wday;
    tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
    return portable_timegm(&tm);
  }
  
  
  // -------------------------------------------------------------
  // 🧮 Aggregate — combine small intervals into days/weeks
  // -------------------------------------------------------------
  // Takes many small data points (like every minute) and combines
  // them into one value per chosen interval (“1d”, “1wk”, etc.).
  // For each group, we compute:
  //   - open  → first price in the group
  //   - close → last price
  //   - high  → highest price reached
  //   - low   → lowest price reached
  //   - volume→ total shares traded
  // -------------------------------------------------------------
  std::vector<StockPoint> StockAnalyzer::Aggregate(const std::vector<StockPoint>& input,
                                                   const std::string& targetInterval)
  {
    std::vector<StockPoint> out;
    if (input.empty())
      return out;
    
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
      agg.open  = vec.front().open;
      agg.close = vec.back().close;
      agg.high  = vec.front().high;
      agg.low   = vec.front().low;
      agg.volume = 0;
      
      for (const auto &c : vec)
      {
        agg.high   = std::max(agg.high, c.high);
        agg.low    = std::min(agg.low, c.low);
        agg.volume += c.volume;
      }
      
      out.push_back(agg);
    }
    
    std::sort(out.begin(), out.end(),
              [](const StockPoint &a, const StockPoint &b){ return a.timestamp < b.timestamp; });
    
    return out;
  }
  
  
  // -------------------------------------------------------------
  // ⚙️ ChooseConfig — pick indicator periods depending on interval
  // -------------------------------------------------------------
  // Different time intervals need different smoothing levels.
  // Example: minute data changes fast, weekly data moves slow.
  // This function sets default periods for SMA, EMA, RSI, etc.
  // -------------------------------------------------------------
  IndicatorConfig StockAnalyzer::ChooseConfig(const std::string& interval,
                                              const std::string& range)
  {
    IndicatorConfig cfg;
    
    if (interval == "1m" || interval == "2m" || interval == "5m") {
      cfg.smaPeriod = 50;
      cfg.emaPeriod = 50;
      cfg.rsiPeriod = 14;
      cfg.atrPeriod = 14;
      cfg.volPeriod = 50;
      cfg.momentumSensitivity = 0.008;
    }
    else if (interval == "1h" || interval == "15m") {
      cfg.smaPeriod = 30;
      cfg.emaPeriod = 30;
      cfg.rsiPeriod = 14;
      cfg.atrPeriod = 14;
      cfg.volPeriod = 30;
    }
    else if (interval == "1d") {
      cfg.smaPeriod = 20;
      cfg.emaPeriod = 20;
      cfg.rsiPeriod = 14;
      cfg.atrPeriod = 14;
      cfg.volPeriod = 20;
    }
    else if (interval == "1wk") {
      cfg.smaPeriod = 10;
      cfg.emaPeriod = 10;
      cfg.rsiPeriod = 10;
      cfg.atrPeriod = 10;
      cfg.volPeriod = 10;
      cfg.momentumSensitivity = 0.02;
    }
    
    return cfg;
  }
  
  
  // -------------------------------------------------------------
  // 🔍 AnalyzeInternal — main brain of the analyzer
  // -------------------------------------------------------------
  // This function looks at all calculated indicators and decides:
  //  • What the trend looks like (up / down / sideways)
  //  • How strong it is (momentum)
  //  • How much the price moves daily (volatility)
  //  • Whether trading activity is high or low (volume)
  //  • Whether price seems too high or low (RSI valuation)
  // Then it blends everything into a simple score and suggestion.
  // -------------------------------------------------------------
  std::pair<StockSummary, StockTechnicals> StockAnalyzer::AnalyzeInternal(const std::vector<StockPoint>& h,
                                              const IndicatorConfig& cfg,
                                              const std::string& interval)
  {
    StockSummary summaryResult;
    StockTechnicals tecnicalResult;

    if (h.size() < static_cast<size_t>(cfg.smaPeriod))
    {
      summaryResult.suggestion = "Not enough data to analyze.";
      return {summaryResult, tecnicalResult};
    }
    
    // --- Compute indicators ---
    tecnicalResult.close     = h.back().close;
    tecnicalResult.sma       = ComputeSMA(h, cfg.smaPeriod);
    tecnicalResult.ema       = ComputeEMA(h, cfg.emaPeriod);
    tecnicalResult.rsi       = ComputeRSI(h, cfg.rsiPeriod);
    tecnicalResult.atr       = ComputeATR(h, cfg.atrPeriod);
    tecnicalResult.vwap      = ComputeVWAP(h);
    tecnicalResult.macd      = ComputeMACD(h, 12, 26);
    tecnicalResult.avgVol    = ComputeAverageVolume(h, cfg.volPeriod);
    tecnicalResult.latestVol = h.back().volume;
    
    
    // ---------------------------------------------------------
    // 📈 Trend Analysis
    // ---------------------------------------------------------
    // Check if price is generally going up or down by comparing
    // current price vs moving averages and VWAP.
    // ---------------------------------------------------------
    if (tecnicalResult.close > tecnicalResult.ema * 1.01 && tecnicalResult.ema > tecnicalResult.sma && tecnicalResult.close > tecnicalResult.vwap)
    {
      summaryResult.trend.value = "Uptrend";
      summaryResult.trend.reason = "Price is above recent averages : Buyers are stronger.";
    }
    else if (tecnicalResult.close < tecnicalResult.ema * 0.99 && tecnicalResult.ema < tecnicalResult.sma && tecnicalResult.close < tecnicalResult.vwap)
    {
      summaryResult.trend.value = "Downtrend";
      summaryResult.trend.reason = "Price is below recent averages : Sellers are in control.";
    }
    else
    {
      summaryResult.trend.value = "Sideways";
      summaryResult.trend.reason = "Price is moving near its averages : No clear direction.";
    }
    
    
    // ---------------------------------------------------------
    // ⚡ Momentum (trend strength)
    // ---------------------------------------------------------
    double emaDiff = std::fabs(tecnicalResult.ema - tecnicalResult.sma) / tecnicalResult.sma;
    if (emaDiff > cfg.momentumSensitivity * 2)
    {
      summaryResult.momentum.value  = "Strong";
      summaryResult.momentum.reason = "Price is moving quickly in one direction : Strong push from traders.";
    }
    else if (emaDiff > cfg.momentumSensitivity)
    {
      summaryResult.momentum.value  = "Moderate";
      summaryResult.momentum.reason = "Price is trending steadily : Decent strength.";
    }
    else
    {
      summaryResult.momentum.value  = "Weak";
      summaryResult.momentum.reason = "Price barely moving away from average : Market resting.";
    }
    
    
    // ---------------------------------------------------------
    // 🌊 Volatility (how wild the swings are)
    // ---------------------------------------------------------
    double atrPercent = SafeDiv(tecnicalResult.atr, tecnicalResult.close) * 100.0;
    if (atrPercent > 3.0)
    {
      summaryResult.volatility.value  = "High";
      summaryResult.volatility.reason = "Price is jumping around a lot : Unpredictable moves.";
    }
    else if (atrPercent > 1.5)
    {
      summaryResult.volatility.value  = "Medium";
      summaryResult.volatility.reason = "Price moves moderately : Some swings but manageable.";
    }
    else
    {
      summaryResult.volatility.value  = "Low";
      summaryResult.volatility.reason = "Price is calm : Small day to day changes.";
    }
    
    
    // ---------------------------------------------------------
    // 🔊 Volume (trading activity)
    // ---------------------------------------------------------
    double volRatio = SafeDiv(tecnicalResult.latestVol, tecnicalResult.avgVol, 1.0);
    if (volRatio > cfg.volHighFactor)
    {
      summaryResult.volume.value  = "High";
      summaryResult.volume.reason = "Unusually large trading today : Lots of participation.";
    }
    else if (volRatio < cfg.volLowFactor)
    {
      summaryResult.volume.value  = "Low";
      summaryResult.volume.reason = "Fewer trades than normal : Low interest right now.";
    }
    else
    {
      summaryResult.volume.value  = "Normal";
      summaryResult.volume.reason = "Trading volume looks typical for this stock.";
    }
    
    
    // ---------------------------------------------------------
    // 💰 Valuation via RSI
    // ---------------------------------------------------------
    if (tecnicalResult.rsi > 70)
    {
      summaryResult.valuation.value  = "Overbought";
      summaryResult.valuation.reason = "Price rose quickly : May be due for a small pullback.";
    }
    else if (tecnicalResult.rsi < 30)
    {
      summaryResult.valuation.value  = "Oversold";
      summaryResult.valuation.reason = "Price fell sharply : May bounce back soon.";
    }
    else
    {
      summaryResult.valuation.value  = "Fair";
      summaryResult.valuation.reason = "Price moves balanced : Neither too high nor too low.";
    }
    
    
    // ---------------------------------------------------------
    // 📍 VWAP Bias (where price sits relative to real average)
    // ---------------------------------------------------------
    double vwapDiff = SafeDiv(tecnicalResult.close - tecnicalResult.vwap, tecnicalResult.vwap);
    if (vwapDiff > 0.01)
    {
      summaryResult.vwapBias.value  = "Above VWAP";
      summaryResult.vwapBias.reason = "Price stays above the average paid price : Buyers dominant.";
    }
    else if (vwapDiff < -0.01)
    {
      summaryResult.vwapBias.value  = "Below VWAP";
      summaryResult.vwapBias.reason = "Price below the average paid price : Sellers dominant.";
    }
    else
    {
      summaryResult.vwapBias.value  = "Near VWAP";
      summaryResult.vwapBias.reason = "Price is around the fair average : Neutral area.";
    }
    
    
    // ---------------------------------------------------------
    // 🧮 Combine everything into one confidence score
    // ---------------------------------------------------------
    double trendScore = (summaryResult.trend.value == "Uptrend") ? 1.0 :
    (summaryResult.trend.value == "Downtrend") ? -1.0 : 0.0;
    double rsiScore  = (tecnicalResult.rsi - 50.0) / 50.0;
    double macdScore = tecnicalResult.macd / (tecnicalResult.close * 0.01);
    double vwapScore = std::clamp(vwapDiff * 100.0, -1.0, 1.0);
    
    double score = (trendScore * 0.35 +
                    rsiScore   * 0.25 +
                    macdScore  * 0.25 +
                    vwapScore  * 0.15);
    
    summaryResult.score = std::clamp(score, -1.0, 1.0);
    
    
    // ---------------------------------------------------------
    // 💡 Final suggestion based on score
    // ---------------------------------------------------------
    if (summaryResult.score > 0.4)
    {
      if (tecnicalResult.close > tecnicalResult.vwap)
        summaryResult.suggestion = "Buy : Uptrend looks strong and price is above the fair average.";
      else
        summaryResult.suggestion = "Hold : Looks positive but price slightly under average : Wait for confirmation.";
    }
    else if (summaryResult.score < -0.4)
    {
      if (tecnicalResult.close < tecnicalResult.vwap)
        summaryResult.suggestion = "Sell : Downtrend clear and price is below average : Sellers leading.";
      else
        summaryResult.suggestion = "Hold : Downtrend forming but price still high : Caution.";
    }
    else
    {
      summaryResult.suggestion = "Hold : Market is calm or indecisive : No clear signal yet.";
    }
    
    // Short summary for UI
    summaryResult.conclusion = "Trend: " + summaryResult.trend.value +
    " | Momentum: " + summaryResult.momentum.value +
    " | Volatility: " + summaryResult.volatility.value +
    " | RSI: " + std::to_string(static_cast<int>(tecnicalResult.rsi)) +
    " | VWAP: " + std::to_string(tecnicalResult.vwap);
    
    return {summaryResult, tecnicalResult};
  }
  
  
  // -------------------------------------------------------------
  // 🔹 Public helper — single-interval analysis
  // -------------------------------------------------------------
  std::pair<StockSummary, StockTechnicals> StockAnalyzer::AnalyzeSingleTimeframe(const StockData& data,
                                                     const std::string& interval,
                                                     const std::string& range)
  {
    auto cfg = ChooseConfig(interval, range);
    return AnalyzeInternal(data.history, cfg, interval);
  }
  
  
  // -------------------------------------------------------------
  // 🔸 Public helper — hybrid (short + long term) analysis
  // -------------------------------------------------------------
  // Compares two timeframes to confirm bigger picture trend.
  // Example: 15-minute data (short) + daily data (long).
  // -------------------------------------------------------------
  std::pair<StockSummary, StockTechnicals> StockAnalyzer::AnalyzeHybrid(const StockData& shortTerm,
                                            const StockData& longTerm,
                                            const std::string& shortInterval,
                                            const std::string& longInterval)
  {
    auto shortCfg = ChooseConfig(shortInterval, "short");
    auto longCfg  = ChooseConfig(longInterval, "long");
    
    auto [shortSummary, shortTechnicals] = AnalyzeInternal(shortTerm.history, shortCfg, shortInterval);
    auto [longSummary, longTechnicals]  = AnalyzeInternal(longTerm.history,  longCfg,  longInterval);
    
    StockSummary hybrid;
    
    // Combine both results
    hybrid.trend.value  = (shortSummary.trend.value == longSummary.trend.value)
    ? shortSummary.trend.value : "Mixed";
    hybrid.trend.reason = "Short-term: " + shortSummary.trend.value +
    ", Long-term: " + longSummary.trend.value + ".";
    
    hybrid.momentum   = shortSummary.momentum;
    hybrid.volatility = shortSummary.volatility;
    hybrid.volume     = shortSummary.volume;
    hybrid.vwapBias   = shortSummary.vwapBias;
    hybrid.valuation  = shortSummary.valuation;
    
    // Blend scores
    hybrid.score = (shortSummary.score * 0.6 + longSummary.score * 0.4);
    
    // Simplified combined suggestion
    if (shortSummary.vwapBias.value.find("Above") != std::string::npos &&
        longSummary.trend.value == "Uptrend")
    {
      hybrid.suggestion = "Buy : Both short and long trends are rising : Strong bullish setup.";
    }
    else if (shortSummary.vwapBias.value.find("Below") != std::string::npos &&
             longSummary.trend.value == "Downtrend")
    {
      hybrid.suggestion = "Sell : Both timeframes point down : Sellers leading.";
    }
    else if (std::fabs(hybrid.score) < 0.4)
    {
      hybrid.suggestion = "Hold : Mixed signals : Better to wait.";
    }
    
    return {hybrid,shortTechnicals};
  }
} // namespace KanVest
