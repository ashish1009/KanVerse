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
    double score = 0.0;
    int qtyDelta = 0;
    std::string rationale;
  };
  
  class Recommender
  {
  public:
  private:
  };
} // namespace KanVest
