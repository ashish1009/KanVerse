//
//  UI_Widget.hpp
//  KanVasX
//
//  Created by Ashish . on 31/10/25.
//

#pragma once

namespace KanVasX
{
  class Widget
  {
  public:
    /// This function initializes the widget icons
    static void Initialize();
    /// This function remove widget icons
    static void Shutdown();
    
    /// This creates the cancel button
    /// - Parameters:
    ///   - title: title of enter button
    ///   - inactive: is button inactive
    static bool CancelButton(std::string_view title, ImFont* imGuiFont, bool inactive = false);
    /// This createst the enter button
    /// - Parameters:
    ///   - title: title of enter button
    ///   - inactive: is button inactive
    static bool EnterButton(std::string_view title, ImFont* imGuiFont, bool inactive = false);
    
    /// This function draws the search widget and return true if search is valid
    /// - Parameters:
    ///   - stringSearch: search string buffer
    ///   - bufferSize: buffer size
    ///   - hint: hint to be shown on search bar
    ///   - roundingVall: rounding value of search widget
    static bool Search(char* stringSearch, uint32_t bufferSize, float height = Settings::FrameHeight,
                       std::string_view hint = "Search ...", float roundingVal = Settings::FrameRounding, bool* grabFocus = nullptr);
    /// This function renders the Setting Button
    /// - Parameter color: color of button
    static bool OptionsButton(const ImU32& color);

  private:
    inline static char* s_assetSearchString;
    inline static ImTextureID s_searchTextureID = 0;
    inline static ImTextureID s_optionTextureID = 0;
  };
  
  bool IsMatchingSearch(const std::string& item, const std::string_view& searchQuery, bool caseSensitive = false, bool stripWhiteSpaces = false, bool stripUnderscores = false);
} // namespace KanVasX
