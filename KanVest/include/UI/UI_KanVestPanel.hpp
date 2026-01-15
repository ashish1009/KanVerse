//
//  UI_KanVestPanel.hpp
//  KanVest
//
//  Created by Ashish . on 08/01/26.
//

#pragma once

#include "Stock/StockMetadata.hpp"

namespace KanVest::UI
{
  class Panel
  {
  public:
    /// This is primary stock UI panel
    static void Show();
    
    // Sets the texutre icon IDs
    static void SetShadowTextureId(ImTextureID shadowTextureID);

  private:
    /// This function updates the selected stock data
    static void UpdateSelectedStock();
    
    /// This function shows stock search bar
    static void ShowStockSearchBar(float width, float height);

    /// This function shows stock basic data
    /// - Parameter stockData: stock data to show
    static void ShowStockData(const StockData& stockData);
    /// This function shows stock basic data
    /// - Parameter stockData: stock data to show
    static void ShowStockTechnicals(const StockData& stockData);

    // Stock search data
    inline static char s_searchedStockString[128] = "Nifty";
    inline static std::string s_selectedStockSymbol = "Nifty";

    // Stock change cache
    inline static bool s_stockChanged = true;
    inline static std::string s_lastSymbol;
    inline static std::string s_lastRange;
    inline static std::string s_lastInterval;

    // Texture data
    inline static ImTextureID s_shadowTextureID = 0;
  };
} // namespace KanVest::UI
