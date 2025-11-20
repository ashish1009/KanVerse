//
//  RecommendationEngine.hpp
//  KanVest
//
//  Created by Ashish . on 20/11/25.
//

#pragma ocne

namespace KanVest
{
  // User holding info (optional)
  struct HoldingInfo
  {
    double shares = 0.0;
    double avgPrice = 0.0;   // user's average cost per share
    double cashAvailable = 0.0; // cash user can use to buy more shares
  };
  
  // Suggested action
  enum class Action
  {
    StrongBuy,
    Buy,
    Hold,
    Sell,
    StrongSell
  };

  struct Recommendation
  {
    double score = 0.0;            // 0..100
    Action action = Action::Hold;
    double suggestedQuantity = 0.0; // positive -> buy, negative -> sell, zero -> none
    double suggestedTradeValue = 0.0; // suggestedQuantity * currentPrice
    std::vector<std::string> reasons; // human-readable reasons for the decision
    
    std::string ToString() const;
  };
} // namespace KanVest
