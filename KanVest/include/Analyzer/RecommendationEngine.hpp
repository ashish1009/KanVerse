//
//  RecommendationEngine.hpp
//  KanVest
//
//  Created by Ashish . on 20/11/25.
//

#pragma ocne

#include "Stock/StockData.hpp"

#include "Analyzer/Indicator/AllIndicatorResults.hpp"
#include "Analyzer/Indicator/IndicatorManager.hpp"

#include "Analyzer/Chart/SupportResistance.hpp"
#include "Analyzer/Chart/ChartPattern.hpp"

#include "Analyzer/Candle/CandleEngine.hpp"

#include "Portfolio/Portfolio.hpp"

namespace KanVest
{
  enum class RecommendationGrade { STRONG_SELL, SELL, HOLD, BUY, STRONG_BUY };

  struct Recommendation
  {
    RecommendationGrade grade = RecommendationGrade::HOLD;
    double score = 0.0;                  // 0..1
    int suggestedQuantityDelta = 0;      // + buy, – sell
    std::string rationale;
  };

  class Recommender
  {
  public:
    Recommender() {}
    
    void SetHoldings(const std::vector<Holding>& h)
    {
      holdings = h;
    }

    Recommendation Recommend(const StockData& data, int maxAffordableQty = 100)
    {
      Recommendation r;
      
      if (!data.IsValid())
      {
        r.rationale = "Invalid stock data";
        return r;
      }
      
      internalNotes.clear();
      
      // ---- Compute individual scores ----
      double candleScore = ComputeCandleScore(data);
      double chartScore  = ComputeChartScore(data);
      double srScore     = ComputeSRScore(data);
      double fiftyScore  = Compute52WeekScore(data);
      double momentumScore = ComputeMomentumScore(data);
      double volumeScore   = ComputeVolumeScore(data);
      
      // ---- Weighted final score ----
      double finalScore =
      0.22 * candleScore
      + 0.20 * chartScore
      + 0.18 * srScore
      + 0.20 * fiftyScore
      + 0.10 * momentumScore
      + 0.10 * volumeScore;
      
      finalScore = std::clamp(finalScore, 0.0, 1.0);
      
      r.score = finalScore;
      r.grade = ScoreToGrade(finalScore);
      r.suggestedQuantityDelta = DecideQuantityDelta(data, r.grade, maxAffordableQty);
      
      // Build rationale
      std::ostringstream ss;
      ss << std::fixed << std::setprecision(2);
      ss << "score=" << finalScore
      << " candle=" << candleScore
      << " chart=" << chartScore
      << " sr=" << srScore
      << " 52w=" << fiftyScore
      << " momentum=" << momentumScore
      << " volume=" << volumeScore
      << "; grade=" << GradeToString(r.grade)
      << "; qty=" << r.suggestedQuantityDelta;
      
      if (!internalNotes.empty())
        ss << " | notes: " << internalNotes;
      
      r.rationale = ss.str();
      return r;
    }

  private:
    std::vector<Holding> holdings;
    std::string internalNotes;
    
    // ========= Candle Score ===========
    double ComputeCandleScore(const StockData& data)
    {
      auto patterns = Candle::CandlePatternEngine::Analyze(data, 50);
      if (patterns.empty()) return 0.5;
      
      double score = 0.5;
      for (auto& p : patterns)
      {
        const auto& cp = p.second;
        if (cp.bullish) score += 0.15 * cp.confidence;
        if (cp.bearish) score -= 0.15 * cp.confidence;
        
        internalNotes += cp.name + ",";
      }
      return std::clamp(score, 0.0, 1.0);
    }
    
    // ========= Chart Score ===========
    double ComputeChartScore(const StockData& data)
    {
      ChartPatternEngine eng;
      auto results = eng.Analyze(data.history, 120);
      if (results.empty()) return 0.5;
      
      double score = 0.5;
      for (auto& p : results)
      {
        switch (p.type)
        {
          case ChartPatternType::DOUBLE_BOTTOM: score += 0.18 * p.confidence; break;
          case ChartPatternType::DOUBLE_TOP:    score -= 0.18 * p.confidence; break;
          case ChartPatternType::CUP_AND_HANDLE:score += 0.16 * p.confidence; break;
          case ChartPatternType::HEAD_AND_SHOULDERS: score -= 0.20 * p.confidence; break;
          default: break;
        }
        internalNotes += p.name + ",";
      }
      return std::clamp(score, 0.0, 1.0);
    }
    
    // ========= Support / Resistance ===========
    double ComputeSRScore(const StockData& data)
    {
      SupportResistance sr(data);
      
      if (sr.supports.empty() && sr.resistances.empty())
        return 0.5;
      
      double score = 0.5;
      
      if (!sr.supports.empty())
      {
        double sp = sr.supports.front().price;
        double rel = fabs(data.livePrice - sp) / std::max(1.0, sp);
        if (rel < 0.01) score = 0.80;
        else if (rel < 0.03) score = 0.65;
      }
      
      if (!sr.resistances.empty())
      {
        double rp = sr.resistances.front().price;
        double rel = fabs(data.livePrice - rp) / std::max(1.0, rp);
        
        if (rel < 0.01) score = 0.20;
        else if (rel < 0.03) score = 0.35;
      }
      
      internalNotes += "SR,";
      return std::clamp(score, 0.0, 1.0);
    }
    
    // ========= 52 Week Score ===========
    double Compute52WeekScore(const StockData& data)
    {
      if (data.fiftyTwoHigh <= data.fiftyTwoLow) return 0.5;
      
      double pos = (data.livePrice - data.fiftyTwoLow) /
      (data.fiftyTwoHigh - data.fiftyTwoLow);
      
      // near low → bullish
      double score = 1.0 - pos;
      
      if (pos < 0.2) internalNotes += "near_52w_low,";
      if (pos > 0.8) internalNotes += "near_52w_high,";
      
      return std::clamp(score, 0.0, 1.0);
    }
    
    // ========= Price Momentum ===========
    double ComputeMomentumScore(const StockData& data)
    {
      if (data.history.size() < 5) return 0.5;
      
      double prev = data.history[data.history.size() - 5].close;
      double now  = data.livePrice;
      
      double change = (now - prev) / std::max(1.0, prev);
      
      double score = 0.5 + change;
      return std::clamp(score, 0.0, 1.0);
    }
    
    // ========= Volume Strength ===========
    double ComputeVolumeScore(const StockData& data)
    {
      if (data.history.size() < 20) return 0.5;
      
      double avgVol = 0;
      for (size_t i = data.history.size() - 20; i < data.history.size(); ++i)
        avgVol += data.history[i].volume;
      avgVol /= 20.0;
      
      double score = data.volume / std::max(1.0, avgVol);
      
      if (score > 2.0) score = 1.0;
      else if (score < 0.5) score = 0.2;
      
      internalNotes += "volume,";
      return std::clamp(score, 0.0, 1.0);
    }
    
    // ========= Score → Grade ===========
    RecommendationGrade ScoreToGrade(double s)
    {
      if (s >= 0.80) return RecommendationGrade::STRONG_BUY;
      if (s >= 0.65) return RecommendationGrade::BUY;
      if (s >= 0.45) return RecommendationGrade::HOLD;
      if (s >= 0.30) return RecommendationGrade::SELL;
      return RecommendationGrade::STRONG_SELL;
    }
    
    // ========= Quantity Suggestion ===========
    int DecideQuantityDelta(const StockData& data, RecommendationGrade g, int maxQ)
    {
      int qty = 0;
      double avg = 0;
      
      for (auto& h : holdings)
        if (h.symbol == data.symbol) { qty = h.quantity; avg = h.averagePrice; }
      
      double pnl = 0.0;
      if (qty > 0 && avg > 0)
        pnl = (data.livePrice - avg) / avg;
      
      int sug = 0;
      switch (g)
      {
        case RecommendationGrade::STRONG_BUY: sug = maxQ; break;
        case RecommendationGrade::BUY: sug = std::max(1, maxQ / 4); break;
        case RecommendationGrade::HOLD: sug = 0; break;
        case RecommendationGrade::SELL: sug = -(qty / 2); break;
        case RecommendationGrade::STRONG_SELL: sug = -qty; break;
      }
      
      // Smooth profit sells
      if (qty > 0 && pnl > 0.10 && sug < 0)
        sug /= 2;
      
      if (sug > maxQ) sug = maxQ;
      if (sug < -qty) sug = -qty;
      
      return sug;
    }
    
    std::string GradeToString(RecommendationGrade g)
    {
      switch (g)
      {
        case RecommendationGrade::STRONG_BUY: return "STRONG_BUY";
        case RecommendationGrade::BUY: return "BUY";
        case RecommendationGrade::HOLD: return "HOLD";
        case RecommendationGrade::SELL: return "SELL";
        case RecommendationGrade::STRONG_SELL: return "STRONG_SELL";
      }
      return "UNKNOWN";
    }

  };
} // namespace KanVest
