//
//  UI_KanVestPanel.cpp
//  KanVest
//
//  Created by Ashish . on 08/01/26.
//

#include "UI_KanVestPanel.hpp"

namespace KanVest::UI
{
#define Font(font) KanVest::UI::Font::Get(KanVest::UI::FontType::font)
  
  using Align = KanVasX::UI::AlignX;
  using Color = KanVasX::Color;
  
  void Panel::Show()
  {
    IK_PERFORMANCE_FUNC("Panel::Show");
    
    ImGui::Begin("Stock Analyzer");
    ImGui::End();
  }
} // namespace KanVest::UI
