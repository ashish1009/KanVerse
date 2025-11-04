//
//  UI_Popup.hpp
//  KanVasX
//
//  Created by Ashish . on 04/11/25.
//

#pragma once

#include "UI_Utils.hpp"
#include "UI_Scoped.hpp"

namespace KanVasX
{
  class Popup
  {
  public:
    /// This constructor creates the popup instance
    /// - Parameter title: title of popup
    Popup(std::string_view title);
    
    /// This function sets the parameters of popup
    /// - Parameters:
    ///   - title: Title of popup
    ///   - openFlag: open flag
    ///   - width: new width of popup
    ///   - height: new height of popup
    ///   - center: center flag
    void Set(std::string_view title, bool openFlag, float width, float height, bool center);
    /// This funciton changes the popup flag
    /// - Parameter openFlag: popup flag
    void SetFlag(bool openFlag);
    
    /// This function shows the popup inside imGui loop
    /// - Parameters:
    ///   - popupFlags: popup flags
    ///   - func: function inside popup
    template<typename FuncT> void Show(ImGuiWindowFlags popupFlags, FuncT&& func)
    {
      ScopedColor borderColor(ImGuiCol_Border, Color::Null);
      // Check if flag is open to open popup model
      if (m_openFlag)
      {
        ImGui::OpenPopup(m_title.data());
        m_openFlag = false;
      }
      
      // Center the popup
      if (m_center)
      {
        UI::SetNextWindowAtCenter();
      }
      
      // Set size of popup
      if (m_width > 0 and m_height > 0)
      {
        UI::SetNextWindowSize({m_width, m_height});
      }
      
      // Begin popup model
      if (ImGui::BeginPopupModal(m_title.data(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | popupFlags))
      {
        UI::PushID();
        func();
        UI::PopID();
        
        ImGui::EndPopup();
      }
    }
    
    float GetWidgth() const;
    float GetHeight() const;
    
  private:
    std::string m_title {""};
    bool m_openFlag {false};
    bool m_center {true};
    float m_width, m_height;
  };
} // namespace KanVasX
