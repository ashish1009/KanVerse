//
//  Analyzer.hpp
//  KanVest
//
//  Created by Ashish . on 07/11/25.
//

#pragma once

#include "Stocks/Analyzer/AnalyzerStrucutre.hpp"
#include "Stocks/Analyzer/Indicators.hpp"
#include "Stocks/StockManager.hpp"

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
    std::vector<PatternHit> candlePatterns;
    std::vector<PatternHit> chartPatterns;
    
    // indicator snapshots
    double lastClose = 0.0;
    double rsi = 0.0;
    double macd = 0.0, macdSignal = 0.0;
    double smaShort = 0.0, smaLong = 0.0;
    double atr = 0.0;
    double vwap = 0.0;
    double obv = 0.0, obvSlope = 0.0;
    double adx = 0.0;
    double stochasticK = 0.0, stochasticD = 0.0;
    double cci = 0.0, roc = 0.0, mfi = 0.0;
    Indicators::BB bollinger{};
    
    Recommendation recommendation = Recommendation::Unknown;
    double score = 0.0;
    std::string explanation;          // short summary (already exists)
    std::string detailedExplanation;  // NEW: human-friendly analysis summary
    
    bool hasHolding = false;
    double unrealizedPL = 0.0;
    double unrealizedPct = 0.0;
    double suggestedActionQty = 0.0;
    std::string actionReason;
    
    // Tooltip descriptions (for UI hover info)
    std::unordered_map<std::string, std::string> tooltips;
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
