//
//  UI_Panel.hpp
//  KanVasX
//
//  Created by Ashish . on 30/10/25.
//

#pragma once

namespace KanVasX
{
  class Panel
  {
  public:
    /// This function wraps the ImGui Begin with theme modification
    /// - Parameter title: title of imgui
    static bool Begin(std::string_view title, bool* isOpen = nullptr, ImGuiWindowFlags flags = 0);
    /// This function ends the ImGui panel
    /// - Parameter shadow: Flag to render shadow
    static void End(ImTextureID shadowTextureID = 0, int32_t radius = 15);
  };
} // namespace KanVasX
