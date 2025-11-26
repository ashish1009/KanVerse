//
//  StockAnalyzer.hpp
//  KanVest
//
//  Created by Ashish . on 23/11/25.
//

#pragma once

#include "Stock/StockData.hpp"

#include "Portfolio/Portfolio.hpp"

#include "Analyzer/Indicator/MovingAverage.hpp"
#include "Analyzer/Indicator/Momentum.hpp"
#include "Analyzer/Indicator/MACD.hpp"

#include "Analyzer/Chart/Pivot.hpp"

namespace KanVest
{
  enum class RecommendationGrade
  {
    STRONG_SELL,
    SELL,
    HOLD,
    BUY,
    STRONG_BUY
  };

  struct Recommendation
  {
    RecommendationGrade grade = RecommendationGrade::HOLD;
    double score = 50.0;
    int qtyDelta = 0;
    std::unordered_map<std::string, std::string> summary;
  };

  class Analyzer
  {
  public:
    /// This function analyze stock from data
    /// - Parameter stockData: stock data
    static Recommendation AnalzeStock(const StockData& stockData);
    
    /// This function sets the holding data for analuzer
    /// - Parameter holding: holding data
    static void SetHoldings(const Holding& holding);
    
    static const std::string& GetSummary(const std::string& tagKey);
    
    static const std::map<int, double>& GetSMA();
    static const std::map<int, double>& GetEMA();
    static const RSISeries& GetRSI();
    static const MACDResult& GetMACD();
    
    static const PivotResults& GetPivots();

  private:
    inline static Recommendation s_recommendation;
    
    inline static Holding s_stockHolding;
    
    inline static MAResult s_maResults;
    inline static RSISeries s_rsiSeries;
    inline static MACDResult s_macdResult;
    
    inline static PivotResults s_pivots;
  };
} // namespace KanVest
