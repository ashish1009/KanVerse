//
//  StockAnalyzer.cpp
//  KanVest
//
//  Created by Ashish . on 14/01/26.
//

#include "StockAnalyzer.hpp"

#include "UI/UI_Utils.hpp"

namespace KanVest
{
  struct ScoreResult
  {
    float score;
    std::vector<std::pair<ImU32, std::string>> explanation;
  };

  inline bool Near(double a, double b, double threshold = 0.0015)
  {
    return std::abs(a - b) / b < threshold;
  }
  
  inline double MAWeight(int period)
  {
    // Smooth, monotonic, scalable
    return std::log(static_cast<double>(period));
  }
  
  ScoreResult ComputeMAScore(TechnicalIndicators type, double price, const std::map<int, std::vector<double>>& maMap, double scoreScale = 12.0)
  {
    ScoreResult result { 0.0f, {} };
    
    if (maMap.empty())
    {
      result.explanation.push_back({KanVasX::Color::Red,"Insufficient data to evaluate trend strength."});
      return result;
    }
    
    double weightedScore = 0.0;
    double totalWeight   = 0.0;
    
    int aboveShort = 0, aboveMedium = 0, aboveLong = 0;
    int positiveSlope = 0, negativeSlope = 0;
    
    for (const auto& [period, values] : maMap)
    {
      if (values.size() < 2)
        continue;
      
      double ma     = values.back();
      double prev   = values[values.size() - 2];
      double w      = MAWeight(period);
      totalWeight  += w;
      
      // ----------------------------
      // Price position vs MA
      // ----------------------------
      if (!Near(price, ma))
        weightedScore += (price > ma ? +w : -w);
      
      // ----------------------------
      // MA slope (EMA reacts faster)
      // ----------------------------
      double slope = (ma - prev) / prev;
      double slopeThreshold = (type == TechnicalIndicators::EMA) ? 0.0006 : 0.0004;
      double slopeImpact    = (type == TechnicalIndicators::EMA) ? 0.7 : 0.5;
      
      if (slope > slopeThreshold)
      {
        weightedScore += slopeImpact * w;
        positiveSlope++;
      }
      else if (slope < -slopeThreshold)
      {
        weightedScore -= slopeImpact * w;
        negativeSlope++;
      }
      
      // ----------------------------
      // Buckets for explanation
      // ----------------------------
      if (price > ma)
      {
        if (period <= 10)       aboveShort++;
        else if (period <= 50)  aboveMedium++;
        else                    aboveLong++;
      }
    }
    
    // ----------------------------
    // Normalize & scale
    // ----------------------------
    double normalized = (totalWeight > 0.0) ? (weightedScore / totalWeight) : 0.0;
    result.score = static_cast<float>(normalized * scoreScale);
    
    // ----------------------------
    // Explanation
    // ----------------------------
    std::string explaintion = "";
    ImU32 color = KanVasX::Color::White;
    
    explaintion = "Trend Overview: ";
    if (aboveLong >= 2)
    {
      color = UI::Utils::StockProfitColor;
      explaintion += "The price is trading above most long-term averages, indicating a strong structural trend.\n";
    }
    else
    {
      color = UI::Utils::StockLossColor;
      explaintion += "The price is struggling to remain above long-term averages, suggesting weaker structure.\n";
    }
    result.explanation.push_back({color, explaintion});

    explaintion = "Short-Term View: ";
    if (aboveShort >= 1)
    {
      color = UI::Utils::StockProfitColor;
      explaintion += "Short-term momentum remains constructive with recent buying interest.\n";
    }
    else
    {
      color = UI::Utils::StockLossColor;
      explaintion += "Short-term momentum is weak, indicating hesitation from buyers.\n";
    }
    result.explanation.push_back({color, explaintion});

    explaintion = "Medium-Term View: ";
    if (aboveMedium >= 2)
    {
      color = UI::Utils::StockProfitColor;
      explaintion += "Medium-term trend support is present, suggesting the move is not purely short-lived.\n";
    }
    else
    {
      color = UI::Utils::StockLossColor;
      explaintion += "Medium-term trend lacks confirmation and may need stabilization.\n";
    }
    result.explanation.push_back({color, explaintion});

    explaintion = "Momentum Insight: ";
    if (positiveSlope > negativeSlope)
    {
      color = UI::Utils::StockProfitColor;
      explaintion += "Moving averages are rising, confirming positive momentum.\n";
    }
    else if (positiveSlope == negativeSlope)
    {
      color = UI::Utils::StockModerateColor;
      explaintion += "Momentum is mixed, suggesting consolidation.\n";
    }
    else
    {
      color = UI::Utils::StockLossColor;
      explaintion += "Momentum is weakening as several averages lose slope.\n";
    }
    result.explanation.push_back({color, explaintion});

    explaintion = "Actionable Insight: ";
    if (result.score > 6)
    {
      color = UI::Utils::StockProfitColor;
      explaintion += "Trend strength favors holding or accumulating on controlled pullbacks.\n";
    }
    else if (result.score > 2)
    {
      color = UI::Utils::StockModerateColor;
      explaintion += "Trend is neutral to mildly positive. Fresh entries should be selective.\n";
    }
    else
    {
      color = UI::Utils::StockLossColor;
      explaintion += "Trend strength is limited. Waiting for clearer confirmation may reduce risk.\n";
    }
    result.explanation.push_back({color, explaintion});

    return result;
  }

  void Analyzer::AnalzeStock(const StockData& stockData)
  {
    // Reset report data
    s_stockReport.score = 50.0f;
    s_stockReport.summary.clear();
    
    // Technical data
    s_maResults = MovingAverage::Compute(stockData);
    s_rsiSeries = RSI::Compute(stockData);
    
    // Score
    auto UpdateSummaryData = [](TechnicalIndicators tag, const ScoreResult& result) {
      s_stockReport.score += result.score;
      s_stockReport.summary[tag] = result.explanation;
    };
    
    UpdateSummaryData(TechnicalIndicators::DMA, ComputeMAScore(TechnicalIndicators::DMA, stockData.livePrice, s_maResults.dmaValues));
    UpdateSummaryData(TechnicalIndicators::EMA, ComputeMAScore(TechnicalIndicators::EMA, stockData.livePrice, s_maResults.emaValues));
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
  const RSISeries& Analyzer::GetRSI()
  {
    return s_rsiSeries;
  }
} // namespace KanVest
