//
//  IndicatorManager.hpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#pragma once

#include "IIndicator.hpp"
#include "AllIndicatorResults.hpp"
#include "Indicators.hpp"

namespace KanVest
{
  class IndicatorsManager
  {
  public:
    IndicatorsManager();
    
    // Add new indicator to manager
    void AddIndicator(std::unique_ptr<IIndicator> indicator);

    // Compute all indicators and store results internally
    void ComputeAll(const StockData& data);
    
    // Reset all indicators
    void ResetAll();
    
    // Get centralized results
    const AllIndicatorsResult& GetResults() const { return allResults; }
    
    // Access individual indicators (optional)
    const std::vector<std::unique_ptr<IIndicator>>& GetIndicators() const { return indicators; }
    
  private:
    std::vector<std::unique_ptr<IIndicator>> indicators;
    AllIndicatorsResult allResults;
  };
}
