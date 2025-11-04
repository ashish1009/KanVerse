//
//  Portfolio.hpp
//  KanVest
//
//  Created by Ashish . on 04/11/25.
//

#pragma once

namespace KanVest
{
  struct InvestmentHorizon
  {
    enum class Vision {LongTerm, MidTerm, ShortTerm};
    
    std::string symbol;
    double averagePrice;
    int32_t quantity;
    Vision vision;
  };
  
  class Portfolio
  {
  public:
    void AddHolding(const InvestmentHorizon& holding);
    
    std::vector<InvestmentHorizon>& GetHoldings();
    const std::vector<InvestmentHorizon>& GetHoldings() const;

  private:
    std::vector<InvestmentHorizon> m_holdings;
  };
  
  class PortfolioSerializer
  {
  public:
    static bool SaveToYAML(const std::filesystem::path& path, const Portfolio& portfolio);
    static bool LoadFromYAML(const std::filesystem::path& path, Portfolio& portfolio);
  };

  namespace PortfolioUtils
  {
    // Helper conversion for Vision enum
    std::string VisionToString(InvestmentHorizon::Vision vision);
    InvestmentHorizon::Vision StringToVision(const std::string& str);
  } // namespace PortfolioUtils
} // namespace KanVest
