//
//  PortfolioController.hpp
//  KanVest
//
//  Created by Ashish . on 06/11/25.
//

#pragma once

#include "Portfolio/Portfolio.hpp"

namespace KanVest
{
  /// This class links portfolio with stock manager
  class PortfolioController
  {
  public:
    /// This creates portfolio manager constructor
    /// - Parameters:
    ///   - portfolio: portfolio instance
    PortfolioController(Portfolio& portfolio);
    
    /// This function Adds holding in portfolio
    /// - Parameter holding: holding data
    void AddHolding(const Holding& holding);
    
    /// This function loads portfolio from file
    /// - Parameter path: file path
    bool LoadPortfolio(const std::filesystem::path& path);
    
    /// This function loads portfolio from file
    /// - Parameter path: file path
    bool SavePortfolio(const std::filesystem::path& path);
    
    /// This function returns holdings
    const std::vector<Holding>& GetHoldings() const;
    
  private:
    Portfolio& m_portfolio;
  };
}
