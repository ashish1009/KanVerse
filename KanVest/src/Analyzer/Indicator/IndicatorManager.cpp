//
//  IndicatorManager.cpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#include "IndicatorManager.hpp"

namespace KanVest
{
  IndicatorsManager::IndicatorsManager()
  {
    // Initialize default indicators with default parameters
    AddIndicator(std::make_unique<MAIndicator>(20, 20));
    AddIndicator(std::make_unique<RSIIndicator>(14));
    AddIndicator(std::make_unique<MACDIndicator>(12, 26, 9));
    AddIndicator(std::make_unique<ADXIndicator>(14));
    AddIndicator(std::make_unique<BBIndicator>(20, 2.0));
    AddIndicator(std::make_unique<MFIIndicator>(14));
    AddIndicator(std::make_unique<StochasticIndicator>(14));
  }
  
  void IndicatorsManager::ComputeAll(const StockData& data)
  {
    for (auto& ind : indicators)
    {
      ind->Compute(data);
    }
    
    // Store all results centrally
    for (auto& ind : indicators)
    {
      if (auto p = dynamic_cast<MAIndicator*>(ind.get()))       allResults.ma = p->result;
      else if (auto p = dynamic_cast<RSIIndicator*>(ind.get())) allResults.rsi = p->result;
      else if (auto p = dynamic_cast<MACDIndicator*>(ind.get())) allResults.macd = p->result;
      else if (auto p = dynamic_cast<ADXIndicator*>(ind.get())) allResults.adx = p->result;
      else if (auto p = dynamic_cast<BBIndicator*>(ind.get()))  allResults.bb = p->result;
      else if (auto p = dynamic_cast<MFIIndicator*>(ind.get())) allResults.mfi = p->result;
      else if (auto p = dynamic_cast<StochasticIndicator*>(ind.get())) allResults.stochastic = p->result;
    }
  }
  
  void IndicatorsManager::ResetAll()
  {
    for (auto& ind : indicators)
    {
      ind->Reset();
    }
    
    // Clear centralized results
    allResults = AllIndicatorsResult{};
  }
  
  void IndicatorsManager::AddIndicator(std::unique_ptr<IIndicator> indicator)
  {
    indicators.push_back(std::move(indicator));
  }
}
