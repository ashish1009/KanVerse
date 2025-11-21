//
//  Portfolio.cpp
//  KanVest
//
//  Created by Ashish . on 21/11/25.
//

#include "Portfolio.hpp"

namespace KanVest
{
  void Portfolio::AddHolding(const Holding &holding)
  {
    // Check if holding already exists
    auto it = std::find_if(m_holdings.begin(), m_holdings.end(),
                           [&](const Holding& existing) {
      return existing.symbolName == holding.symbolName;
    });
    
    if (it != m_holdings.end())
    {
      // Update existing holding (weighted average)
      int32_t totalQuantity = holding.quantity + it->quantity;
      if (totalQuantity > 0)
      {
        float totalValue = (it->averagePrice * it->quantity) + (holding.averagePrice * holding.quantity);
        it->averagePrice = totalValue / totalQuantity;
      }
      
      // Update total of holding
      it->quantity = totalQuantity;
    }
    else
    {
      // Add new holding
      m_holdings.emplace_back(holding);
    }
  }
  
  std::vector<Holding>& Portfolio::GetHoldings()
  {
    return m_holdings;
  }
  
  const std::vector<Holding>& Portfolio::GetHoldings() const
  {
    return m_holdings;
  }
  
  const Holding& Portfolio::GetHolding(const std::string &symbolName) const
  {
    static Holding EmptyHolding;
    for (const auto& h : m_holdings)
    {
      if (h.symbolName == symbolName)
      {
        return h;
      }
    }
    return EmptyHolding;
  }

} // namespace KanVest
