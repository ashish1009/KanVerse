//
//  Portfolio.hpp
//  KanVest
//
//  Created by Ashish . on 05/11/25.
//

#pragma once

namespace KanVest
{
  struct Holding
  {
    enum class Vision {LongTerm, MidTerm, ShortTerm};
    
    std::string symbol;
    double averagePrice;
    int32_t quantity;
    Vision vision;

    // Compute dynamic not serisalize
    double stockValue;
    double investment;
    double value;
    double profitLoss;
    double profitLossPercent;
  };
  
  class Portfolio
  {
  public:
    /// This function adds the holding in data base
    /// - Parameter holding: holding instnace
    void AddHolding(const Holding& holding);
    
    /// This function returns the holding
    std::vector<Holding>& GetHoldings();
    /// This function returns the holding
    const std::vector<Holding>& GetHoldings() const;
    
  private:
    std::vector<Holding> m_holdings;
  };
  
  class PortfolioSerializer
  {
  public:
    /// This function saves portfolio data in file
    /// - Parameters:
    ///   - path: file path
    ///   - portfolio: portfolio data
    static bool SaveToYAML(const std::filesystem::path& path, const Portfolio& portfolio);
    /// This function loads the portfolio from file
    /// - Parameters:
    ///   - path: file path
    ///   - portfolio: portfolio reference
    static bool LoadFromYAML(const std::filesystem::path& path, Portfolio& portfolio);
  };
  
  namespace PortfolioUtils
  {
    std::string VisionToString(Holding::Vision vision);
    Holding::Vision StringToVision(const std::string& str);
  } // namespace PortfolioUtils
} // namespace KanVest
