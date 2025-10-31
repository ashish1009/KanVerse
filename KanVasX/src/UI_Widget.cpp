//
//  UI_Widget.cpp
//  KanVasX
//
//  Created by Ashish . on 31/10/25.
//

#include "UI_Widget.hpp"

#include <vendors/Choc/text/choc_StringUtilities.h>

#include "UI_Utils.hpp"
#include "UI_Scoped.hpp"

namespace KanVasX
{
#define MAX_INPUT_BUFFER_LENGTH 256
  
  namespace StringUtils
  {
    /// Converts a string to lowercase
    std::string ToLower(const std::string& string)
    {
      std::string result;
      result.reserve(string.size());
      
      for (const char c : string)
      {
        result += static_cast<char>(std::tolower(c));
      }
      
      return result;
    }
    
    /// Converts a string to uppercase
    std::string ToUpper(const std::string_view& string)
    {
      std::string result;
      result.reserve(string.size());
      
      for (const char c : string)
      {
        result += static_cast<char>(std::toupper(c));
      }
      
      return result;
    }
  } // namespace StringUtils
  
  bool IsMatchingSearch(const std::string& item, const std::string_view& searchQuery, bool caseSensitive, bool stripWhiteSpaces, bool stripUnderscores)
  {
    if (searchQuery.empty())
    {
      return true;
    }
    
    if (item.empty())
    {
      return false;
    }
    
    std::string itemSanitized = stripUnderscores ? choc::text::replace(item, "_", " ") : item;
    
    if (stripWhiteSpaces)
    {
      itemSanitized = choc::text::replace(itemSanitized, " ", "");
    }
    
    std::string searchString = stripWhiteSpaces ? choc::text::replace(searchQuery, " ", "") : std::string(searchQuery);
    
    if (!caseSensitive)
    {
      itemSanitized = StringUtils::ToLower(itemSanitized);
      searchString = StringUtils::ToLower(searchString);
    }
    
    bool result = false;
    if (choc::text::contains(searchString, " "))
    {
      std::vector<std::string> searchTerms = choc::text::splitAtWhitespace(searchString);
      for (const auto& searchTerm : searchTerms)
      {
        if (!searchTerm.empty() and choc::text::contains(itemSanitized, searchTerm))
        {
          result = true;
        }
        else
        {
          result = false;
          break;
        }
      }
    }
    else
    {
      result = choc::text::contains(itemSanitized, searchString);
    }
    
    return result;
  }
  
  void Widget::Initialize()
  {
    s_assetSearchString = new char[MAX_INPUT_BUFFER_LENGTH];
  }
  
  void Widget::Shutdown()
  {
    delete [] s_assetSearchString;
  }
  
  void Widget::SetSearchIcon(ImTextureID searchTextureID)
  {
    s_searchTextureID = searchTextureID;
  }
  void Widget::SetSettingIcon(ImTextureID optionTextureID)
  {
    s_optionTextureID = optionTextureID;
  }

  bool Widget::CancelButton(std::string_view title, ImFont* imGuiFont, bool inactive)
  {
    return (UI::DrawButton(title.data(), imGuiFont, Color::ButtonMuted, Color::Text, inactive) or ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Escape));
  }
  
  bool Widget::EnterButton(std::string_view title, ImFont* imGuiFont, bool inactive)
  {
    return (UI::DrawButton(title, imGuiFont, inactive ? Color::TextMuted : Color::Button, Color::ButtonContrast, inactive) or (!inactive and ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Enter)));
  }
  
  bool Widget::Search(char *searchString, uint32_t bufferSize, float height, int32_t width, std::string_view hint, float roundingVal, bool* grabFocus)
  {
    ScopedStyle headerPaddingAndHeight(ImGuiStyleVar_FramePadding, ImVec2{height, height});
    
    bool modified = false;
    bool searching = false;
    
    // Suspend the ImGui layout to overlap the Text box with Icon
    const bool LayoutSuspended = []
    {
      ImGuiWindow* window = ImGui::GetCurrentWindow();
      if (window->DC.CurrentLayout)
      {
        ImGui::SuspendLayout();
        return true;
      }
      return false;
    }();
    
    UI::PushID();
    {
      const float areaPosX = ImGui::GetCursorPosX();
      const float framePaddingY = ImGui::GetStyle().FramePadding.y;
      
      ScopedStyle rounding(ImGuiStyleVar_FrameRounding, roundingVal);
      ScopedStyle padding(ImGuiStyleVar_FramePadding, ImVec2(28.0f, framePaddingY));
      
      // Input Tex box
      ImGui::SetNextItemWidth(width);
      if (ImGui::InputText(UI::GenerateID(), searchString, bufferSize))
      {
        modified = true;
      }
      else if (ImGui::IsItemDeactivatedAfterEdit())
      {
        modified = true;
      }
      searching = searchString[0] != 0;

      if (grabFocus and *grabFocus)
      {
        if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) and !ImGui::IsAnyItemActive() and !ImGui::IsMouseClicked(0))
        {
          ImGui::SetKeyboardFocusHere(-1);
        }
        if (ImGui::IsItemFocused())
        {
          *grabFocus = false;
        }
      }
      
      UI::DrawItemActivityOutline(roundingVal, true, Color::Highlight);
      ImGui::SetItemAllowOverlap();
      
      UI::SameLine(areaPosX + 5.0f);
      
      // Resume the Layout to overlap
      if (LayoutSuspended)
      {
        ImGui::ResumeLayout();
      }
      
      ImGui::BeginHorizontal(UI::GenerateID(), ImGui::GetItemRectSize());
      const ImVec2 iconSize(ImGui::GetTextLineHeight() - 2.0f, ImGui::GetTextLineHeight() - 2.0f);
      
      // Search icon
      {
        if (s_optionTextureID)
        {
          const float iconYOffset = framePaddingY - 3.0f;
          UI::ShiftCursorY(iconYOffset);
          UI::Image(s_searchTextureID, iconSize);
          UI::ShiftCursorY(-iconYOffset);
        }
        
        // Hint
        if (!searching)
        {
          UI::ShiftCursorX(10.0f);
          UI::ShiftCursorY(-framePaddingY + 1.0f);
          ScopedColor text(ImGuiCol_Text, Color::TextDark);
          ScopedStyle padding(ImGuiStyleVar_FramePadding, ImVec2(0.0f, framePaddingY));
          ImGui::TextUnformatted(hint.data());
          UI::ShiftCursorY(-1.0f);
        }
      }
      ImGui::EndHorizontal();
      
      // Clear the Text on Escape Button
      if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Escape))
      {
        memset(searchString, 0, bufferSize);
        modified = true;
      }
      
    }
    UI::PopID();
    
    return modified;
  }
  
  bool Widget::OptionsButton( const ImU32& color)
  {
    return UI::DrawButtonImage("##options", s_optionTextureID, false, {15.0f, 15.0f}, {0, 0}, color);
  }
} // namespace KanVasX
