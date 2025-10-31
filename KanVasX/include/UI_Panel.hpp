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
    /// This initializes the texture ID
    /// - Parameter shadowTextureID: shadow texture ID
    static void Initialize(ImTextureID shadowTextureID);
    
    /// This function wraps the ImGui Begin with theme modification
    /// - Parameter title: title of imgui
    static bool Begin(std::string_view title, bool* isOpen = nullptr, ImGuiWindowFlags flags = 0);
    /// This function ends the ImGui panel
    /// - Parameter shadow: Flag to render shadow
    static void End(int32_t radius = 15);
    
  private:
    inline static ImTextureID s_shadowTextureID = 0;
  };
} // namespace KanVasX
