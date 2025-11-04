//
//  Portfolio.hpp
//  KanVest
//
//  Created by Ashish . on 04/11/25.
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
  };
  
  class Portfolio
  {
  public:
  private:
    std::vector<Holding> m_holdings;
  };
} // namespace KanVest
