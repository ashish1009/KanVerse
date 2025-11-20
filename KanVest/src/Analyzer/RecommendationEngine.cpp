//
//  RecommendationEngine.cpp
//  KanVest
//
//  Created by Ashish . on 20/11/25.
//

#include "RecommendationEngine.hpp"

namespace KanVest
{
  std::string Recommendation::ToString() const
  {
    std::ostringstream ss;
    ss << "Score: " << score << " Action: ";
    switch (action)
    {
      case Action::StrongBuy: ss << "StrongBuy"; break;
      case Action::Buy: ss << "Buy"; break;
      case Action::Hold: ss << "Hold"; break;
      case Action::Sell: ss << "Sell"; break;
      case Action::StrongSell: ss << "StrongSell"; break;
    }
    ss << " Qty: " << suggestedQuantity << " Value: " << suggestedTradeValue << "\nReasons:";
    for (auto &r : reasons) ss << "\n - " << r;
    return ss.str();
  }
} // namespace KanVest
