//
//  Analyzer.hpp
//  KanVest
//
//  Created by Ashish . on 07/11/25.
//

#pragma once

#include "Stocks/Analyzer/AnalyzerStrucutre.hpp"
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
    double smaShort=0.0, smaLong=0.0;
    double atr=0.0;
    double vwap=0.0;
    Recommendation recommendation = Recommendation::Unknown;
    double score = 0.0;
    std::string explanation;
    
    bool hasHolding = false;
    double unrealizedPL = 0.0;
    double unrealizedPct = 0.0;
    double suggestedActionQty = 0.0;
    std::string actionReason;          
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
