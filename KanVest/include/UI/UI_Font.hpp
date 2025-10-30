//
//  UI_Font.hpp
//  KanVest
//
//  Created by Ashish . on 30/10/25.
//

#pragma once

namespace KanVest::UI
{
  /// This enum stores the fonts of Kreator
  enum class FontType : uint8_t
  {
    Regular, Light, Italic, LightItalic, SemiBold, Bold, SemiBoldItalic, BoldItalic, ExtraBold, ExtraBoldItalic,
    FixedWidthRegular, FixedWidthLight, FixedWidthMedium, FixedWidthBold,
    Small, Medium, Large, ExtraLarge, Header, SemiHeader, HugeHeader,
    Max // Should be at last for loop in class
  };
  
  /// This class provides the wrapper APIs to load and use the ImGui fonts
  class Font
  {
  public:
    /// This function changes the imgui font
    ///  - Parameter otherFonts: other fonts
    static void Load(const std::unordered_map<FontType, KanViz::UI::ImGuiFont>& otherFonts);
    /// This function returns the font from type enum
    /// - Parameter type: Font type
    static ImFont* Get(FontType type);
  };
} // namespace KanVest::UI
