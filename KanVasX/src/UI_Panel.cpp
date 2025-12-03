//
//  UI_Panel.cpp
//  KanVasX
//
//  Created by Ashish . on 30/10/25.
//

#include "UI_Panel.hpp"
#include "UI_Scoped.hpp"

namespace KanVasX
{
  void Panel::Initialize(ImTextureID shadowTextureID)
  {
    s_shadowTextureID = shadowTextureID;
  }
  
  bool Panel::Begin(std::string_view title, bool* isOpen, ImGuiWindowFlags flags)
  {
    ScopedColor textColor(ImGuiCol_Text, Color::HighlightContrast);
    ScopedColorStack tabColors(ImGuiCol_TabHovered, Color::TabHovered, ImGuiCol_Tab, Color::Tab, ImGuiCol_TabActive, Color::TabActive,
                               ImGuiCol_TabUnfocused, Color::TabUnfocused, ImGuiCol_TabUnfocusedActive, Color::TabUnfocusedActive);
    
    return ImGui::Begin(title.data(), isOpen, flags | ImGuiWindowFlags_NoTitleBar);
  }
  void Panel::End(int32_t radius)
  {
    if (s_shadowTextureID != 0)
    {
      UI::DrawShadowAllDirection(s_shadowTextureID, radius);
    }
    ImGui::End();
  }
} // namespace KanVasX
