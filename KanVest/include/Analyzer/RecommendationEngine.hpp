//
//  RecommendationEngine.hpp
//  KanVest
//
//  Created by Ashish . on 20/11/25.
//

#pragma ocne

#include "Stock/StockData.hpp"
#include "Analyzer/Indicator/AllIndicatorResults.hpp"

#include "Analyzer/Chart/SupportResistance.hpp"
#include "Analyzer/Chart/ChartPattern.hpp"

#include "Analyzer/Candle/CandleEngine.hpp"

namespace KanVest
{
  enum class RecommendationLevel
  {
    STRONG_BUY, BUY, HOLD, SELL, STRONG_SELL
  };

  struct Recommendation
  {
    RecommendationLevel level = RecommendationLevel::HOLD;
    double score = 50.0; // 0..100
    int suggestedQuantity = 0; // positive -> buy, negative -> sell (to close/short)
    std::string rationale;
  };

  struct Holding
  {
    std::string symbol;
    int quantity = 0;
    double avgPrice = 0.0;
  };

  struct RecommendationParams
  {
    double portfolioValue = 100000.0;   // total portfolio value (INR / USD etc.)
    double maxPositionPct = 0.05;       // max fraction of portfolio per position (5% default)
    double riskPerTradePct = 0.01;      // fraction of portfolio risked if stop hits (1%)
    double weightTechnicals = 0.35;
    double weightCandles    = 0.15;
    double weightCharts     = 0.20;
    double weightSR         = 0.10;
    double weightHolding    = 0.10; // reward/penalty for existing holding gain/loss
    double weight52Week     = 0.10;
    // clamp weights to sum 1.0 recommended, but not required
  };

  class RecommendationEngine
  {
  public:
    RecommendationEngine(const RecommendationParams& p = RecommendationParams()) : params(p) {}
    
    // Main analyzer
    Recommendation Analyze(
                           const std::string& symbol,
                           const StockData& stock,
                           const AllIndicatorsResult& ind,
                           const std::vector<std::pair<size_t, KanVest::Candle::CandlePattern>>& candlePatterns,
                           const std::vector<KanVest::ChartPatternResult>& chartResults,
                           const KanVest::SupportResistance& sr,
                           const std::vector<Holding>& holdings,
                           double currentPrice,
                           double week52High,
                           double week52Low
                           );
    
  private:
    RecommendationParams params;
    
    // Helpers: convert indicator structures to simple numeric signals.
    // --- IMPORTANT ---
    // If your AllIndicatorsResult subtypes have different member names, change these functions.
    // They return normalized sub-scores (range -1 .. +1) where +1 is strongly bullish.
    
    double ScoreFromRSI(const AllIndicatorsResult& ind);
    double ScoreFromMA(const AllIndicatorsResult& ind);
    double ScoreFromMACD(const AllIndicatorsResult& ind);
    double ScoreFromADX(const AllIndicatorsResult& ind);
    double ScoreFromBB(const AllIndicatorsResult& ind);
    double ScoreFromMFI(const AllIndicatorsResult& ind);
    double ScoreFromStochastic(const AllIndicatorsResult& ind);
    
    double CandleAggregateScore(const std::vector<std::pair<size_t, KanVest::Candle::CandlePattern>>& patterns, const StockData& stock);
    double ChartAggregateScore(const std::vector<KanVest::ChartPatternResult>& charts);
    double SRScore(const KanVest::SupportResistance& sr, double price);
    double HoldingScore(const std::vector<Holding>& holdings, const std::string& symbol, double price);
    double Week52Score(double price, double hi, double lo);
    
    // utility
    static double Clamp01(double v) { return std::max(0.0, std::min(1.0, v)); }
    static double SigToRange(double v) { return std::max(-1.0, std::min(1.0, v)); }
    
    // Suggested quantity: positive means buy more, negative means sell to close
    int SuggestQuantity(double score, double price, const std::vector<Holding>& holdings, const std::string& symbol);
  };
} // namespace KanVest
