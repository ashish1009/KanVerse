//
//  UI_KanVestPanel.cpp
//  KanVest
//
//  Created by Ashish . on 21/11/25.
//

#include "UI_KanVestPanel.hpp"

namespace KanVest::UI
{
  void Panel::SetShadowTextureId(ImTextureID shadowTextureID)
  {
    s_shadowTextureID = shadowTextureID;
  }
  void Panel::SetReloadTextureId(ImTextureID reloadIconID)
  {
    s_reloadIconID = reloadIconID;
  }
  
  void Panel::Show()
  {
    IK_PERFORMANCE_FUNC("Panel::Show");
    KanVasX::Panel::Begin("Stock Analyzer");
    
    {
//      KanVasX::ScopedColor windowBgColor(ImGuiCol_WindowBg, KanVasX::Color::BackgroundLight);
//      KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, KanVasX::Color::BackgroundLight);
//
//      float totalWidth  = ImGui::GetContentRegionAvail().x;
//      float totalHeight = ImGui::GetContentRegionAvail().y;
//      float childHeight = totalHeight * 0.6f;
//      
//      if (ImGui::BeginChild(" Stock-Analyzer ", ImVec2(totalWidth, childHeight)))
//      {
//        KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
//      }
//      ImGui::EndChild();
//      
//      if (ImGui::BeginChild(" Portfolio-Chart ", ImVec2(totalWidth, 0)))
//      {
//        KanVasX::UI::DrawShadowAllDirection(s_shadowTextureID);
//      }
//      ImGui::EndChild();
    }

    KanVasX::Panel::End(0);
  }
} // namespace KanVest::UI
