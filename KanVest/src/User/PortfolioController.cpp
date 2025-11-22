//
//  PortfolioController.cpp
//  KanVest
//
//  Created by Ashish . on 21/11/25.
//

#include "PortfolioController.hpp"

#include "Stock/StockManager.hpp"

namespace KanVest
{
  PortfolioController::PortfolioController(Portfolio& portfolio)
  : m_portfolio(portfolio) {}
  
  void PortfolioController::AddHolding(const Holding& holding)
  {
    m_portfolio.AddHolding(holding);
    StockManager::AddStock(holding.symbolName);
  }
  
  void PortfolioController::EditHolding(const Holding& holding)
  {
    StockManager::EditStock(holding.symbolName);
  }
  
  bool PortfolioController::LoadPortfolio(const std::filesystem::path& path)
  {
    if (!PortfolioSerializer::LoadFromYAML(path, m_portfolio))
    {
      return false;
    }
    
    // Sync all holdings with live manager
    for (auto& h : m_portfolio.GetHoldings())
    {
      StockManager::AddStock(h.symbolName);
    }
    
    return true;
  }
  
  bool PortfolioController::SavePortfolio(const std::filesystem::path& path)
  {
    return PortfolioSerializer::SaveToYAML(path, m_portfolio);
  }
  
  const std::vector<Holding>& PortfolioController::GetHoldings() const
  {
    return m_portfolio.GetHoldings();
  }
} // namespace KanVest
