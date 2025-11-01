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
    Small, Medium, Large, ExtraLarge, Header, SemiHeader, LargeHeader, HugeHeader,

    FixedWidthHeader_12, FixedWidthHeader_14, FixedWidthHeader_16, FixedWidthHeader_18, FixedWidthHeader_20,
    FixedWidthHeader_22, FixedWidthHeader_24, FixedWidthHeader_26, FixedWidthHeader_28, FixedWidthHeader_30,
    FixedWidthHeader_32, FixedWidthHeader_34, FixedWidthHeader_36, FixedWidthHeader_38, FixedWidthHeader_40,

    Header_22, Header_24, Header_26, Header_28, Header_30,
    Header_32, Header_34, Header_36, Header_38, Header_40,
    Header_42, Header_44, Header_46, Header_48, Header_50,
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
