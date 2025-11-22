//
//  UI_KanVestPanel.hpp
//  KanVest
//
//  Created by Ashish . on 21/11/25.
//

#pragma once

namespace KanVest::UI
{
  class Panel
  {
  public:
    /// This is primary stock UI panel
    static void Show();
    
    static void SetShadowTextureId(ImTextureID shadowTextureID);
    static void SetReloadTextureId(ImTextureID reloadTextureID);
    static void SetOpenEyeTextureId(ImTextureID openTextureID);
    static void SetCloseEyeTextureId(ImTextureID closeTextureID);

  private:
    static void ShowPortfolio();
    
    inline static ImTextureID s_shadowTextureID = 0;
    inline static ImTextureID s_reloadIconID = 0;
    inline static ImTextureID s_openEyeTextureID = 0;
    inline static ImTextureID s_closeEyeTextureID = 0;
  };
} // namespace KanVest::UI
