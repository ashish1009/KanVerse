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
    {5,0.35},{10,0.55},              // Short
    {20,0.85},{30,1.00},{50,1.25},   // Medium
    {100,1.15},{150,0.90},{200,1.40} // Long
  };
  
  inline double NormalizeScore(int raw, int count)
  {
    return static_cast<double>(raw) / static_cast<double>(count);
  }

  inline bool Near(double a, double b, double threshold = 0.0015)
  {
    return std::abs(a - b) / b < threshold;
  }

  ScoreResult GetDMAScore(double price, const std::map<int, std::vector<double>>& dmaValuesMap, double weight = 12.0)
  {
    ScoreResult result { 0.0f, "" };
    
    if (dmaValuesMap.empty())
    {
      result.explanation = "Insufficient trend data to evaluate the stock.";
      return result;
    }
    
    double weightedScore = 0.0;
    double totalWeight = 0.0;
    
    int aboveShort = 0, aboveMedium = 0, aboveLong = 0;
    
    std::ostringstream oss;
    
    for (const auto& [period, values] : dmaValuesMap)
    {
      if (values.size() < 2 || !MA_WEIGHTS.count(period))
      {
        continue;
      }
      
      double dma = values.back();
      double prev = values[values.size() - 2];
      double w = MA_WEIGHTS.at(period);
      totalWeight += w;
      
      // Price position
      if (!Near(price, dma))
      {
        weightedScore += (price > dma ? w : -w);
      }
      
      // Slope
      double slope = (dma - prev) / prev;
      if (slope > 0.0004)
      {
        weightedScore += 0.5 * w;
      }
      else if (slope < -0.0004)
      {
        weightedScore -= 0.5 * w;
      }
      
      // Trend buckets for explanation
      if (price > dma)
      {
        if (period <= 10)
        {
          aboveShort++;
        }
        else if (period <= 50)
        {
          aboveMedium++;
        }
        else
        {
          aboveLong++;
        }
      } // if price > dma
    } // For each period
    
    double normalized = (totalWeight > 0.0) ? weightedScore / totalWeight : 0.0;
    result.score = static_cast<float>(normalized * weight);
    
    // ------------------------------
    // Human-style explanation
    // ------------------------------
    //-------------------------------------------------
    oss << "Trend Overview:\n";
    if (aboveLong >= 2)
    {
      oss << "The stock is trading above its long-term averages, indicating a strong overall trend.\n";
    }
    else
    {
      oss << "The stock is struggling to stay above long-term averages, suggesting weaker structure.\n";
    }
    
    //-------------------------------------------------
    oss << "\nShort-Term View:\n";
    if (aboveShort >= 1)
    {
      oss << "Recent price action remains positive, showing short-term buying interest.\n";
    }
    else
    {
      oss << "Short-term momentum is weak, indicating hesitation from buyers.\n";
    }
    
    //-------------------------------------------------
    oss << "\nMedium-Term View:\n";
    if (aboveMedium >= 2)
    {
      oss << "The price is well supported in the medium term, suggesting the move is not just short-lived.\n";
    }
    else
    {
      oss << "The medium-term trend lacks confirmation and may need stabilization.\n";
    }
    
    //-------------------------------------------------
    oss << "\nLong-Term Summary:\n";
    if (aboveLong >= 2)
    {
      oss << "The stock is positioned above most long-term averages, "
      "indicating a healthy structural trend suitable for long-term holding.\n";
    }
    else if (aboveLong == 1)
    {
      oss << "Long-term structure is improving, but confirmation is still developing. "
      "Sustained strength above key averages would strengthen the long-term case.\n";
    }
    else
    {
      oss << "The stock remains below long-term averages, suggesting the long-term trend "
      "is not yet favorable for investment-oriented positions.\n";
    }

    //-------------------------------------------------
    oss << "\nRisk & Support Insight:\n";
    if (aboveLong >= 2 && aboveMedium >= 2)
    {
      oss << "The stock is well supported across medium and long-term averages. "
      "Pullbacks toward these zones are likely to attract buyers.\n";
    }
    else if (aboveMedium >= 1)
    {
      oss << "Medium-term averages are acting as nearby support. "
      "A controlled pullback toward these levels would improve risk-reward.\n";
    }
    else if (aboveShort >= 1)
    {
      oss << "Short-term support exists, but medium-term structure is still weak. "
      "The stock may be prone to sharper pullbacks.\n";
    }
    else
    {
      oss << "The stock is trading below key averages, indicating limited support. "
      "Downside risk remains elevated until structure improves.\n";
    }

    //-------------------------------------------------
    oss << "\nActionable Insight:\n";
    if (result.score > 6)
    {
      oss << "The trend favors holding or gradual accumulation on dips.\n";
    }
    else if (result.score > 2)
    {
      oss << "The stock is suitable for holding, but fresh entries should be cautious.\n";
    }
    else
    {
      oss << "Trend strength is limited. It may be better to wait for clearer confirmation.\n";
    }
    
    result.explanation = oss.str();
    return result;
  }
  
  ScoreResult GetEMAScore(double price, const std::map<int, std::vector<double>>& emaValuesMap, double weight = 12.0)
  {
    ScoreResult result { 0.0f, "" };
    
    if (emaValuesMap.empty())
    {
      result.explanation = "Insufficient momentum data to evaluate the stock.";
      return result;
    }
    
    double weightedScore = 0.0;
    double totalWeight = 0.0;
    
    int strongMomentum = 0;
    int weakMomentum = 0;
    
    std::ostringstream oss;
    
    for (const auto& [period, values] : emaValuesMap)
    {
      if (values.size() < 2 || !MA_WEIGHTS.count(period))
      {
        continue;
      }
      
      double ema = values.back();
      double prev = values[values.size() - 2];
      double w = MA_WEIGHTS.at(period);
      totalWeight += w;
      
      if (!Near(price, ema))
      {
        weightedScore += (price > ema ? w : -w);
      }
      
      double slope = (ema - prev) / prev;
      if (slope > 0.0006)
      {
        weightedScore += 0.8 * w;
        strongMomentum++;
      }
      else if (slope < -0.0006)
      {
        weightedScore -= 0.8 * w;
        weakMomentum++;
      }
    }
    
    double normalized = (totalWeight > 0.0) ? weightedScore / totalWeight : 0.0;
    result.score = static_cast<float>(normalized * weight);
    
    // ------------------------------
    // Human-style explanation
    // ------------------------------
    
    //-------------------------------------------------
    oss << "Momentum Overview:\n";
    if (strongMomentum > weakMomentum)
    {
      oss << "Momentum is currently in favor of buyers, indicating positive price acceleration.\n";
    }
    else if (strongMomentum == weakMomentum)
    {
      oss << "Momentum is balanced, suggesting a pause or consolidation in price movement.\n";
    }
    else
    {
      oss << "Momentum is weakening, indicating reduced buying pressure.\n";
    }
    
    //-------------------------------------------------
    oss << "\nShort-Term View:\n";
    if (strongMomentum >= 2)
    {
      oss << "Short-term momentum is strong, with price reacting positively to recent moves.\n";
    }
    else if (strongMomentum == 1)
    {
      oss << "Short-term momentum is present but not decisive, requiring confirmation.\n";
    }
    else
    {
      oss << "Short-term momentum is weak, increasing the risk of choppy price action.\n";
    }
    
    //-------------------------------------------------
    oss << "\nMedium-Term View:\n";
    if (strongMomentum > weakMomentum)
    {
      oss << "Momentum is sustainable in the medium term, suggesting continued participation from buyers.\n";
    }
    else
    {
      oss << "Medium-term momentum lacks strength and may fade without renewed buying interest.\n";
    }
    
    //-------------------------------------------------
    oss << "\nLong-Term Bias:\n";
    if (strongMomentum >= 2 && result.score > 4)
    {
      oss << "Momentum is aligned with the broader trend, supporting continuation if conditions remain favorable.\n";
    }
    else
    {
      oss << "Momentum is not yet aligned with a durable trend, limiting long-term conviction.\n";
    }
    
    //-------------------------------------------------
    oss << "\nRisk Insight:\n";
    if (weakMomentum >= 2)
    {
      oss << "Multiple EMAs are losing slope, increasing the risk of short-term pullbacks or reversals.\n";
    }
    else
    {
      oss << "Momentum remains controlled, but sharp moves without consolidation may increase volatility.\n";
    }
    
    //-------------------------------------------------
    oss << "\nActionable Insight:\n";
    if (result.score > 6)
    {
      oss << "Momentum supports holding existing positions or adding selectively on pullbacks.\n";
    }
    else if (result.score > 2)
    {
      oss << "Momentum is neutral. Waiting for clearer acceleration may improve entry quality.\n";
    }
    else
    {
      oss << "Momentum is weak. Risk is elevated for fresh positions until strength improves.\n";
    }

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
