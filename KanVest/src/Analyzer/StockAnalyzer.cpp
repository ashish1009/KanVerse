//
//  StockAnalyzer.cpp
//  KanVest
//
//  Created by Ashish . on 23/11/25.
//

#include "StockAnalyzer.hpp"

namespace KanVest
{
  // Normalize raw score (-count to +count) to -1 to +1
  inline double NormalizeScore(int raw, int count)
  {
    return static_cast<double>(raw) / static_cast<double>(count);
  }
  
  struct ScoreResult
  {
    float score;
    std::string explanation;
  };
  
  ScoreResult GetSMAScore(double price, const std::map<int, double>& smaValues, double weight = 10.0)
  {
    ScoreResult result { 0.0f, "" };
    
    if (smaValues.empty())
    {
      result.explanation = "No SMA values available to calculate score.";
      return result;
    }
    
    int rawScore = 0;
    std::ostringstream oss;
    
    oss << "SMA Score Explanation:\n";
    oss << "Price: " << price << "\n\n";
    
    for (const auto& kv : smaValues)
    {
      int period = kv.first;
      double sma = kv.second;
      
      oss << "SMA(" << period << ") = " << sma;
      
      if (price > sma)
      {
        oss << " → Price is ABOVE SMA → Bullish\n";
        rawScore += 1;
      }
      else
      {
        oss << " → Price is BELOW SMA → Bearish\n";
        rawScore -= 1;
      }
    }
    
    double normalized = NormalizeScore(rawScore, (int32_t)smaValues.size());
    float finalScore = normalized * weight;
    
    oss << "\nRaw Score: " << rawScore
    << "\nNormalized (-1 to +1): " << normalized
    << "\nFinal SMA Score: " << finalScore << "\n\n";
    
    oss << "What SMA Means:\n"
    << "SMA (Simple Moving Average) shows the average price of the last N candles.\n"
    << "If the price is above most SMAs → trend is generally upward (bullish).\n"
    << "If the price is below them → trend is downward (bearish).\n";
    
    result.score = finalScore;
    result.explanation = oss.str();
    return result;
  }
  
  ScoreResult GetEMAScore(double price, const std::map<int, double>& emaValues, double weight = 10.0)
  {
    ScoreResult result { 0.0f, "" };
    
    if (emaValues.empty())
    {
      result.explanation = "No EMA values available to calculate score.";
      return result;
    }
    
    int rawScore = 0;
    std::ostringstream oss;
    
    oss << "EMA Score Explanation:\n";
    oss << "Price: " << price << "\n\n";
    
    for (const auto& kv : emaValues)
    {
      int period = kv.first;
      double ema = kv.second;
      
      oss << "EMA(" << period << ") = " << ema;
      
      if (price > ema)
      {
        oss << " → Price is ABOVE EMA → Bullish\n";
        rawScore += 1;
      }
      else
      {
        oss << " → Price is BELOW EMA → Bearish\n";
        rawScore -= 1;
      }
    }
    
    double normalized = static_cast<double>(rawScore) / emaValues.size();
    float finalScore = normalized * weight;
    
    oss << "\nRaw Score: " << rawScore
    << "\nNormalized (-1 to +1): " << normalized
    << "\nFinal EMA Score: " << finalScore << "\n\n";
    
    oss << "What EMA Means:\n"
    << "EMA (Exponential Moving Average) gives more weight to recent prices.\n"
    << "This makes it react faster to trend changes compared to SMA.\n"
    << "Price above EMA → strong upward momentum (bullish).\n"
    << "Price below EMA → weak or downward momentum (bearish).\n";
    
    result.score = finalScore;
    result.explanation = oss.str();
    return result;
  }

  
  double GetRSIScore(const RSISeries& rsiData, double weight = 10.0)
  {
    double rsi = rsiData.last;    // latest value
    
    if (std::isnan(rsi))
      return 0.0;
    
    // Normalize RSI:
    // 0  -> +1 (strong bullish)
    // 50 -> 0  (neutral)
    // 100-> -1 (strong bearish)
    double normalized = (50.0 - rsi) / 50.0;
    
    // clamp to [-1, 1]
    normalized = std::max(-1.0, std::min(1.0, normalized));
    
    return normalized * weight;
  }
  
  double GetMACDScore(const MACDResult& macd, double weight = 10.0)
  {
    const auto& macdSeries   = macd.macdLine;
    const auto& signalSeries = macd.signalLine;
    const auto& histSeries   = macd.histogram;
    
    size_t n = macdSeries.size();
    if (n < 2) return 0.0;
    
    double macdNow  = macdSeries[n - 1];
    double macdPrev = macdSeries[n - 2];
    
    double sigNow   = signalSeries[n - 1];
    
    double histNow  = histSeries[n - 1];
    double histPrev = histSeries[n - 2];
    
    // --- individual normalized parts (-1 or +1) ---
    double crossover = (macdNow > sigNow) ? +1.0 : -1.0;
    double momentum  = (histNow > histPrev) ? +1.0 : -1.0;
    double slope     = (macdNow > macdPrev) ? +1.0 : -1.0;
    
    // --- Normalize to range -1 to +1 ---
    double raw = (crossover + momentum + slope) / 3.0;
    
    // --- Apply weight ---
    return raw * weight;
  }
  
  double GetPivotScore(const PivotResults& pivots, double livePrice, int totalCandles, double weight = 10.0)
  {
    if (totalCandles <= 0) return 0.0;
    
    const int maxLevels = 4;
    
    // --- Sort supports and resistances by strength (count + recency) ---
    auto sortByStrength = [&](const SRLevel& a, const SRLevel& b) {
      double scoreA = a.count + static_cast<double>(a.lastTouchIdx) / totalCandles;
      double scoreB = b.count + static_cast<double>(b.lastTouchIdx) / totalCandles;
      return scoreA > scoreB; // strongest first
    };
    
    std::vector<SRLevel> sortedSupports = pivots.supports;
    std::vector<SRLevel> sortedResistances = pivots.resistances;
    std::sort(sortedSupports.begin(), sortedSupports.end(), sortByStrength);
    std::sort(sortedResistances.begin(), sortedResistances.end(), sortByStrength);
    
    // --- Take top 4 levels for each ---
    int nSupports = std::min(maxLevels, (int)sortedSupports.size());
    int nResistances = std::min(maxLevels, (int)sortedResistances.size());
    
    double supportScore = 0.0;
    for (int i = 0; i < nSupports; i++)
    {
      double rel = (livePrice - sortedSupports[i].price) / livePrice; // bullish if price > support
      supportScore += std::clamp(rel, -1.0, 1.0);
    }
    if (nSupports > 0) supportScore /= nSupports; // average
    
    double resistanceScore = 0.0;
    for (int i = 0; i < nResistances; i++)
    {
      double rel = (sortedResistances[i].price - livePrice) / livePrice; // bearish if price < resistance
      resistanceScore += std::clamp(rel, -1.0, 1.0);
    }
    if (nResistances > 0) resistanceScore /= nResistances; // average
    
    // --- Combine support and resistance scores ---
    // Support positive → bullish, Resistance positive → bearish, so invert resistance
    double combined = (supportScore - resistanceScore) / 2.0;
    
    // Clamp and apply weight
    combined = std::clamp(combined, -1.0, 1.0);
    return combined * weight;
  }

  
  Recommendation Analyzer::AnalzeStock(const StockData &stockData)
  {
    // Technicals computation
    s_maResults = MovingAverage::Compute(stockData);
//    s_rsiSeries = RSI::Compute(stockData);
//    s_macdResult = MACD::Compute(stockData);

    // chart
//    s_pivots = Pivot::Compute(stockData);

    // Score
    auto UpdateSummaryData = [](const std::string& tag, const ScoreResult& result) {
      s_recommendation.score += result.score;
      s_recommendation.summary[tag] = result.explanation;
    };
    
    UpdateSummaryData("SMA", GetSMAScore(stockData.livePrice, s_maResults.smaValues));
    UpdateSummaryData("EMA", GetEMAScore(stockData.livePrice, s_maResults.emaValues));
    
//    recommendation.score += GetRSIScore(s_rsiSeries);
//    recommendation.score += GetMACDScore(s_macdResult);
//    recommendation.score += GetPivotScore(s_pivots, stockData.livePrice, (int32_t)stockData.history.size());

    return s_recommendation;
  }
  
  void Analyzer::SetHoldings(const Holding &holding)
  {
    s_stockHolding = holding;
  }
  
  const std::string& Analyzer::GetSummary(const std::string& tagKey)
  {
    if (auto it = s_recommendation.summary.find(tagKey); it != s_recommendation.summary.end() )
    {
      return it->second;
    }
    return "";
  }
  
  const std::map<int, double>& Analyzer::GetSMA()
  {
    return s_maResults.smaValues;
  }
  const std::map<int, double>& Analyzer::GetEMA()
  {
    return s_maResults.emaValues;
  }
  const RSISeries& Analyzer::GetRSI()
  {
    return s_rsiSeries;
  }
  const MACDResult& Analyzer::GetMACD()
  {
    return s_macdResult;
  }
  
  const PivotResults& Analyzer::GetPivots()
  {
    return s_pivots;
  }
} // namespace KanVest
