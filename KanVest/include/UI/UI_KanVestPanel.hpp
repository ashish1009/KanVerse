//
//  UI_KanVestPanel.hpp
//  KanVest
//
//  Created by Ashish . on 08/01/26.
//

#pragma once

namespace KanVest::UI
{
  class Panel
  {
  public:
    /// This is primary stock UI panel
    static void Show();
    
  private:
    inline static char s_searchedStockString[128] = "Nifty";
    inline static std::string s_selectedStockSymbol = "Nifty";
  };
} // namespace KanVest::UI
