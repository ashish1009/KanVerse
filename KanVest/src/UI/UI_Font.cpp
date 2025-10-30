//
//  UI_Font.cpp
//  KanVest
//
//  Created by Ashish . on 30/10/25.
//

#include "UI_Font.hpp"

namespace KanVest::UI
{
  void Font::Load(const std::unordered_map<FontType, KanViz::UI::ImGuiFont>& otherFonts)
  {
    auto& imguiLayer = KanViz::Application::Get().GetImGuiLayer();
    imguiLayer.OnDetach();
    imguiLayer.OnAttach();
    
    // Store the fonts in vector in same order which we need to pass to ImGui
    std::vector <KanViz::UI::ImGuiFont> fonts(otherFonts.size());
    for (uint8_t fontIdx = 0; fontIdx < (uint8_t)FontType::Max; fontIdx++)
    {
      fonts[fontIdx] = otherFonts.at((FontType)fontIdx);
    }
    
    imguiLayer.LoadFonts(fonts);
  }
  
  ImFont* Font::Get(FontType type)
  {
    auto fonts = ImGui::GetIO().Fonts->Fonts;
    return fonts.size() > 1 ? fonts[static_cast<int32_t>(type)] : fonts[0];
  }
} // namespace KanVest::UI
