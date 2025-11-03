//
//  StockData.hpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#pragma once

namespace KanVest
{
  struct StockPoint
  {
    double open = 0.0;
    double high = 0.0;
    double low = 0.0;
    double close = 0.0;
    
    uint64_t volume = 0;
    uint64_t timestamp = 0; // UNIX time
    
    double range = 0.0f;
  };
  
  struct StockData
  {
    // Basic Data
    std::string symbol = "";
    std::string currency = "";
    std::string exchangeName = "";
    std::string shortName = "";
    std::string longName = "";
    std::string instrumentType = "";
    std::string timezone = "";
    std::string range = "";
    std::string dataGranularity = "";

    // Price Data
    double livePrice = -1;
    double prevClose = -1;
    
    // Change data
    double change = -1;
    double changePercent = -1;
    
    // Volume
    double volume = -1;
    
    // High low
    // Gives the highest and lowest prices seen during the whole dataset.
    // It shows how “cheap” or “expensive” the current price is relative to its own past.
    double fiftyTwoHigh = -1;
    double fiftyTwoLow = -1;
    double dayHigh = -1;
    double dayLow = -1;
    
    // History
    std::vector<StockPoint> history;
    
    // Moving average
    // SMA = average closing price over N days.
    // It smooths out noise so you can see the trend direction clearly.
    // If price stays above its SMA → uptrend; below → downtrend.
    // Example:
    // Last 5 closes: [100, 102, 104, 103, 105] → SMA(5) = 102.8
    // If today’s price = 107 (above SMA) → market gaining strength.
    double sma = -1;
    
    // Explanation:
    // EMA weights recent prices more, so it reacts faster to momentum shifts.
    // Traders use it to catch trend reversals earlier than SMA.
    // Example:
    // If EMA(10) is rising while SMA(10) is flat → momentum is increasing → buyers active.
    double ema = -1;
    
    // Average True Range
    // Explanation:
    // ATR measures average daily movement.
    // High ATR = large daily price swings → volatile stock.
    // Low ATR = steady, stable stock.
    
    // Example:
    // ATR(14) = ₹4 means on average, price moves ₹4/day.
    // If ATR jumps to ₹8 → volatility doubled → good for day-traders.
    double atr = -1;
    
    // Relative Strength Index
    // Explanation:
    // RSI compares recent gains vs losses on a 0–100 scale.
    // Above 70 → “overbought” (too many recent buyers).
    // Below 30 → “oversold” (too many sellers).
    // It helps identify potential reversals.
    
    // Example:
    // RSI = 78 → price may cool off soon.
    // RSI = 25 → buyers might soon step in.
    double rsi = -1;
    
    // Moving Average Convergence Divergence
    // Explanation:
    // MACD shows the difference between short-term and long-term EMAs.
    // When short-term EMA moves above the long-term → bullish momentum starting.
    
    // Example:
    // EMA12 = ₹102, EMA26 = ₹99 → MACD = +3 → buyers gaining control.
    // When MACD crosses below 0 → sellers back in charge.
    double macd = -1;
    
    // Average Volume
    // Explanation:
    // Shows the average number of shares traded per day.
    // Volume represents interest — the more people trading, the more conviction behind moves.
    
    // Example:
    // Avg Volume(20) = 2M, today Volume = 5M → heavy trading → strong sentiment day.
    double averageVolume = -1;
    
    // Volume-Weighted Average Price
    double vwap = -1;
    
    // Functions
    StockData(const std::string& name);

    double ComputeSMA(int period);
    double ComputeEMA(int period);
    double ComputeATR(int period);
    double ComputeRSI(int period = 14);
    double ComputeAverageVolume(int period);
    double ComputeMACD();
    double ComputeVWAP();

    bool IsValid() const;
  };
} // namespace KanVest
