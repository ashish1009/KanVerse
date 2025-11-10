//
//  Analyzer.hpp
//  KanVest
//
//  Created by Ashish . on 07/11/25.
//

#pragma once

#include "Stocks/Analyzer/AnalyzerStrucutre.hpp"
#include "Stocks/Analyzer/Indicators.hpp"

namespace KanVest::Analysis
{
  struct AnalyzerConfig
  {
    size_t sma_short = 9;
    size_t sma_long = 21;
    size_t rsi_period = 14;
    size_t atr_period = 14;
    bool useMultiframeConfirm = true;
    double breakoutVolumeMultiplier = 1.5;
  };
  
  struct AnalysisReport
  {
    // --- Pattern detections ---
    std::vector<PatternHit> candlePatterns;
    std::vector<PatternHit> chartPatterns;
    
    // --- Indicator snapshots ---
    double lastClose   = 0.0;
    double rsi         = 0.0;
    double macd        = 0.0;
    double macdSignal  = 0.0;
    double smaShort    = 0.0;
    double smaLong     = 0.0;
    double atr         = 0.0;
    double vwap        = 0.0;
    double obv         = 0.0;
    double obvSlope    = 0.0;
    double adx         = 0.0;
    double stochasticK = 0.0;
    double stochasticD = 0.0;
    double cci         = 0.0;
    double roc         = 0.0;
    double mfi         = 0.0;
    Indicators::BB bollinger{};
    
    // --- Derived analytical results ---
    Recommendation recommendation = Recommendation::Unknown;
    double score = 0.0;          // composite score (-1.0 to +1.0)
    double confidence = 0.0;     // 0–100% confidence adjusted by volatility
    double volatility = NAN;     // volatility as % (ATR / Close * 100)
    
    // --- Support & Resistance zones ---
    std::vector<double> supportLevels;     // clustered support levels
    std::vector<double> resistanceLevels;  // clustered resistance levels
    
    // Legacy compatibility (for UI showing single levels)
    double nearestSupportLevel    = NAN;
    double nearestResistanceLevel = NAN;
    
    std::string allSupportLevel    = "";
    std::string allResistanceLevel = "";

    // --- Holding / P&L Info ---
    bool hasHolding        = false;
    double unrealizedPL    = 0.0;
    double unrealizedPct   = 0.0;
    double suggestedActionQty = 0.0;
    std::string actionReason;
    
    // --- Output summaries ---
    std::string explanation;          // short summary (Buy/Hold/Sell with reason)
    std::string detailedExplanation;  // detailed indicator breakdown and commentary
    
    // --- Tooltip info for ImGui UI ---
    std::unordered_map<std::string, std::string> tooltips;
    
    // --- Utility helpers ---
    bool IsBullish() const noexcept { return recommendation == Recommendation::Buy; }
    bool IsBearish() const noexcept { return recommendation == Recommendation::Sell; }
    bool IsNeutral() const noexcept { return recommendation == Recommendation::Hold; }
  };

  class StockAnalyzer
  {
  public:
    StockAnalyzer(const AnalyzerConfig& cfg = {}) : m_cfg(cfg) {}
    AnalysisReport Analyze(const StockData& shortTerm, const StockData* longTerm = nullptr, const HoldingInfo* holding = nullptr) const;
    
  private:
    AnalyzerConfig m_cfg;
    AnalysisReport BuildReport(const StockData& s, const StockData* l, const HoldingInfo* holding) const;
  };
} // namespace
