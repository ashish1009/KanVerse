//
//  Portfolio.hpp
//  KanVest
//
//  Created by Ashish . on 21/11/25.
//

#pragma once

namespace KanVest
{
  struct Holding
  {
    // To be serialize
    std::string symbolName;
    float averagePrice = 0.0f;
    uint32_t quantity = 0;
    
    // Compute dynamic not serisalize
    float stockValue;
    float dayChange;
    float dayChangePercent;

    float GetTotalInvestment() const {return quantity * averagePrice;};
    float GetTotalValue() const {return quantity * stockValue;};
    float GetProfitLoss() const {return GetTotalValue() - GetTotalInvestment();};
    float GetProfitLossPercent() const {return ((GetProfitLoss() * 100) / GetTotalInvestment());};
  };
  
  class Portfolio
  {
  public:
    /// This function returns the holding
    std::vector<Holding>& GetHoldings();
    /// This function returns the holding
    const std::vector<Holding>& GetHoldings() const;
    /// This function returns the holding
    const Holding& GetHolding(const std::string& symbol) const;

  private:
    /// This function adds the holding in data base
    /// - Parameter holding: holding instnace
    void AddHolding(const Holding& holding);

    std::vector<Holding> m_holdings;
  };
} // namespace KanVest
