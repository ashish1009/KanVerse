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

namespace KanVest
{
  // User holding info (optional)
  struct HoldingInfo
  {
    double shares = 0.0;
    double avgPrice = 0.0;   // user's average cost per share
    double cashAvailable = 0.0; // cash user can use to buy more shares
  };
  
  // Suggested action
  enum class Action
  {
    StrongBuy,
    Buy,
    Hold,
    Sell,
    StrongSell
  };

  struct Recommendation
  {
    double score = 0.0;            // 0..100
    Action action = Action::Hold;
    double suggestedQuantity = 0.0; // positive -> buy, negative -> sell, zero -> none
    double suggestedTradeValue = 0.0; // suggestedQuantity * currentPrice
    std::vector<std::string> reasons; // human-readable reasons for the decision
    
    std::string ToString() const;
  };
  
  struct RecommendationConfig
  {
    // weights (sum doesn't have to be 100; engine will normalize)
    double wTrend = 30.0;        // moving averages / MACD / ADX
    double wMomentum = 20.0;     // RSI, Stochastic, MFI
    double wVolatility = 8.0;    // Bollinger width (preference for tight bands)
    double wVolume = 8.0;        // volume spikes vs avg
    double wPatterns = 12.0;     // candle & chart patterns
    double wProximity = 10.0;    // proximity to 52-week high/low & support/resistance
    double wReturn = 12.0;       // recent returns / interval-change behavior

    // risk/trade sizing defaults
    double maxPortfolioAllocation = 0.20; // max fraction of user's portfolio to allocate to this symbol
    double aggressiveMultiplier = 1.0;    // multiplier for suggested buy if StrongBuy
    double conservativeMultiplier = 0.6;  // multiplier for suggested buy if Buy

    // thresholds
    double adxTrendThreshold = 25.0;
    double rsiOverbought = 70.0;
    double rsiOversold = 30.0;
    double macdHistBullThresh = 0.0; // >0 bullish
    double macdHistBearThresh = 0.0; // <0 bearish
    double supportProximityPct = 0.03; // within 3% of support considered close
    double resistanceProximityPct = 0.03; // within 3% of resistance considered close

    // min required history points to compute time-series scores
    size_t minHistoryPoints = 30;
  };
  
  class RecommendationEngine
  {
  public:
    RecommendationEngine(const RecommendationConfig& cfg = RecommendationConfig()) : cfg(cfg) {}

    // Compute a recommendation for a single symbol -> returns Recommendation
    // - `data` is your StockData with latest history and price
    // - `inds` is AllIndicatorsResult containing computed indicator series (aligned to history)
    // - `srLevels` optional support/resistance levels (vector of SRLevel)
    // - `holding` optional user holding info
    Recommendation ComputeRecommendation(const StockData& data,
                                         const AllIndicatorsResult& inds,
                                         const std::vector<SRLevel>& srLevels = {},
                                         const std::optional<HoldingInfo>& holding = std::nullopt) const;
    

  private:
    RecommendationConfig cfg;
    
    // helper scoring submodules
    double ScoreTrend(const StockData& data, const AllIndicatorsResult& inds, std::vector<std::string>& reasons) const;
    double ScoreMomentum(const StockData& data, const AllIndicatorsResult& inds, std::vector<std::string>& reasons) const;
    double ScoreVolatility(const StockData& data, const AllIndicatorsResult& inds, std::vector<std::string>& reasons) const;
    double ScoreVolume(const StockData& data, std::vector<std::string>& reasons) const;
    double ScorePatterns(const StockData& data, const std::vector<SRLevel>& srLevels, std::vector<std::string>& reasons) const;
    double ScoreProximity(const StockData& data, const AllIndicatorsResult& inds, const std::vector<SRLevel>& srLevels, std::vector<std::string>& reasons) const;
    double ScoreReturns(const StockData& data, std::vector<std::string>& reasons) const;
    
    // utility
    static double SafeLast(const std::vector<double>& v);
    static bool IsValidNumber(double v);
    static double Clamp(double v, double lo, double hi);

  };
} // namespace KanVest
