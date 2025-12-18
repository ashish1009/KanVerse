//
//  StockAnalyzer.cpp
//  KanVest
//
//  Created by Ashish . on 14/12/25.
//

#include "StockAnalyzer.hpp"

namespace KanVest
{
  struct ScoreResult
  {
    float score;
    std::string explanation;
  };

  static const std::map<int, double> MA_WEIGHTS =
  {
    {5,0.35},{10,0.55},{20,0.85},{30,1.00},
    {50,1.25},{100,1.15},{150,0.90},{200,1.40}
  };
  
  inline double NormalizeScore(int raw, int count)
  {
    return static_cast<double>(raw) / static_cast<double>(count);
  }

  inline bool Near(double a, double b, double threshold = 0.0015)
  {
    return std::abs(a - b) / b < threshold;
  }

  ScoreResult GetDMAScore(
                          double price,
                          const std::map<int, std::vector<double>>& dmaValuesMap,
                          double weight = 12.0
                          )
  {
    ScoreResult result {0.0f, ""};
    
    if (dmaValuesMap.empty())
    {
      result.explanation = "No SMA data available.";
      return result;
    }
    
    double weightedScore = 0.0;
    double totalWeight = 0.0;
    std::ostringstream oss;
    
    oss << "SMA (DMA) Trend Analysis\n";
    oss << "Price: " << price << "\n\n";
    
    for (const auto& [period, values] : dmaValuesMap)
    {
      if (values.size() < 2 || !MA_WEIGHTS.count(period))
        continue;
      
      double sma = values.back();
      double prev = values[values.size() - 2];
      double w = MA_WEIGHTS.at(period);
      totalWeight += w;
      
      oss << "SMA(" << period << ") = " << sma;
      
      // --- Price vs SMA ---
      if (!Near(price, sma))
      {
        weightedScore += (price > sma ? w : -w);
        oss << (price > sma ? " → Price ABOVE → Bullish" : " → Price BELOW → Bearish");
      }
      else
      {
        oss << " → Near SMA → Neutral";
      }
      
      // --- Slope confirmation ---
      double slope = (sma - prev) / prev;
      if (slope > 0.0004)
      {
        weightedScore += 0.5 * w;
        oss << " | Rising SMA";
      }
      else if (slope < -0.0004)
      {
        weightedScore -= 0.5 * w;
        oss << " | Falling SMA";
      }
      
      oss << "\n";
    }
    
    double normalized = (totalWeight > 0.0)
    ? weightedScore / totalWeight
    : 0.0;
    
    result.score = static_cast<float>(normalized * weight);
    
    oss << "\nWeighted Score: " << weightedScore
    << "\nNormalized (-1 to +1): " << normalized
    << "\nFinal SMA Score: " << result.score << "\n\n";
    
    oss << "Interpretation:\n"
    << "• SMA reflects medium-to-long-term trend strength.\n"
    << "• Rising SMA + price above = strong institutional trend.\n"
    << "• Flat SMA or price near SMA = indecision.\n";
    
    result.explanation = oss.str();
    return result;
  }
  
  ScoreResult GetEMAScore(
                          double price,
                          const std::map<int, std::vector<double>>& emaValuesMap,
                          double weight = 12.0
                          )
  {
    ScoreResult result {0.0f, ""};
    
    if (emaValuesMap.empty())
    {
      result.explanation = "No EMA data available.";
      return result;
    }
    
    double weightedScore = 0.0;
    double totalWeight = 0.0;
    std::ostringstream oss;
    
    oss << "EMA Momentum Analysis\n";
    oss << "Price: " << price << "\n\n";
    
    for (const auto& [period, values] : emaValuesMap)
    {
      if (values.size() < 2 || !MA_WEIGHTS.count(period))
        continue;
      
      double ema = values.back();
      double prev = values[values.size() - 2];
      double w = MA_WEIGHTS.at(period);
      totalWeight += w;
      
      oss << "EMA(" << period << ") = " << ema;
      
      // --- Price vs EMA ---
      if (!Near(price, ema))
      {
        weightedScore += (price > ema ? w : -w);
        oss << (price > ema ? " → Price ABOVE → Bullish" : " → Price BELOW → Bearish");
      }
      else
      {
        oss << " → Near EMA → Neutral";
      }
      
      // --- Momentum (slope) ---
      double slope = (ema - prev) / prev;
      if (slope > 0.0006)
      {
        weightedScore += 0.8 * w;
        oss << " | Strong Momentum";
      }
      else if (slope < -0.0006)
      {
        weightedScore -= 0.8 * w;
        oss << " | Weak Momentum";
      }
      
      oss << "\n";
    }
    
    double normalized = (totalWeight > 0.0)
    ? weightedScore / totalWeight
    : 0.0;
    
    result.score = static_cast<float>(normalized * weight);
    
    oss << "\nWeighted Score: " << weightedScore
    << "\nNormalized (-1 to +1): " << normalized
    << "\nFinal EMA Score: " << result.score << "\n\n";
    
    oss << "Interpretation:\n"
    << "• EMA reacts faster to recent price changes.\n"
    << "• Rising EMA confirms momentum.\n"
    << "• Flat EMA suggests consolidation.\n";
    
    result.explanation = oss.str();
    return result;
  }

  void Analyzer::AnalzeStock(const StockData& stockData)
  {
    // Reset report data
    s_stockReport.score = 50.0f;
    s_stockReport.summary.clear();
    
    // Technical data
    s_maResults = MovingAverage::Compute(stockData);
    
    // Score
    auto UpdateSummaryData = [](TechnicalIndicators tag, const ScoreResult& result) {
      s_stockReport.score += result.score;
      s_stockReport.summary[tag] = result.explanation;
    };
    
    UpdateSummaryData(TechnicalIndicators::DMA, GetDMAScore(stockData.livePrice, s_maResults.dmaValues));
    UpdateSummaryData(TechnicalIndicators::EMA, GetEMAScore(stockData.livePrice, s_maResults.emaValues));
  }
  
  const StockReport& Analyzer::GetReport()
  {
    return s_stockReport;
  }
  
  const std::map<int, std::vector<double>>& Analyzer::GetDMAValues()
  {
    return s_maResults.dmaValues;
  }
  const std::map<int, std::vector<double>>& Analyzer::GetEMAValues()
  {
    return s_maResults.emaValues;
  }
} // namespace KanVest
