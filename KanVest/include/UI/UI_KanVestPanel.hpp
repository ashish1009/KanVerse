//
//  UI_KanVestPanel.hpp
//  KanVest
//
//  Created by Ashish . on 02/12/25.
//

#pragma once

#include "Stock/StockManager.hpp"

namespace KanVest::UI
{
  class Panel
  {
  public:
    /// This function intialize stock UI
    static void Initialize();
    /// This is primary stock UI panel
    static void Show();

    // Sets the texutre icon IDs
    static void SetShadowTextureId(ImTextureID shadowTextureID);
    static void SetReloadTextureId(ImTextureID reloadTextureID);
    static void SetSettingTextureId(ImTextureID settingIconID);
    static void SetOpenEyeTextureId(ImTextureID openTextureID);
    static void SetCloseEyeTextureId(ImTextureID closeTextureID);

  private:
    /// This function shows stock search bar
    static void ShowStockSearchBar(float width, float height);
    /// This function shows stock basic data
    /// - Parameter stockData: stock data to show
    static void ShowStockData(const StockData& stockData);

    inline static char s_searchedStockString[128] = "Nifty";
    inline static std::string s_selectedStockSymbol = "Nifty";
    
    inline static ImTextureID s_shadowTextureID = 0;
    inline static ImTextureID s_reloadIconID = 0;
    inline static ImTextureID s_settingIconID = 0;
    inline static ImTextureID s_openEyeTextureID = 0;
    inline static ImTextureID s_closeEyeTextureID = 0;

  };
} // namespace KanVest::UI
