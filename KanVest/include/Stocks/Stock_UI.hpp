//
//  Stock_UI.hpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#pragma once

namespace KanVest
{
  /// This class responsible for Stock UI Panel
  class StockUI
  {
  public:
    /// This function initializes Stock UI
    static void Initialize(ImTextureID s_reloadIconID);
    
    /// This is primary stock UI panel
    static void StockAnalyzer();
    
  private:
    inline static ImTextureID s_reloadIconID = 0;                     
  };
} // namespace KanVest
