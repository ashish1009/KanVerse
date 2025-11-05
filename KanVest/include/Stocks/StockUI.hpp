//
//  StockUI.hpp
//  KanVest
//
//  Created by Ashish . on 05/11/25.
//

#pragma once


namespace KanVest
{
  /// This class responsible for Stock UI Panel
  class StockUI
  {
  public:
    /// This function initializes Stock UI
    static void Initialize(ImTextureID reloadIconID);
    
    /// This is primary stock UI panel
    static void StockPanel();
    
  private:
    inline static ImTextureID s_reloadIconID = 0;
  };
} // namespace KanVest
