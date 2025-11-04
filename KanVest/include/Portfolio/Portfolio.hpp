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
  private:
    std::vector<InvestmentHorizon> m_holdings;
  };
  
  class PortfolioSerializer
  {
  public:
    static bool SaveToYAML(const std::filesystem::path& path, const Portfolio& portfolio);
    static bool LoadFromYAML(const std::filesystem::path& path, Portfolio& portfolio);
  };

} // namespace KanVest
