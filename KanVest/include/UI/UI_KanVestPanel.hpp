//
//  UI_KanVestPanel.hpp
//  KanVest
//
//  Created by Ashish . on 02/12/25.
//

#pragma once

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
    /// This function show stock search bar
    static void ShowStockSearchBar(float width, float height);

    inline static char s_searchedStockString[128] = "";
    
    inline static ImTextureID s_shadowTextureID = 0;
    inline static ImTextureID s_reloadIconID = 0;
    inline static ImTextureID s_settingIconID = 0;
    inline static ImTextureID s_openEyeTextureID = 0;
    inline static ImTextureID s_closeEyeTextureID = 0;

  };
} // namespace KanVest::UI
