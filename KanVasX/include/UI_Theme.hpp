//
//  UI_Theme.hpp
//  KanVasX
//
//  Created by Ashish . on 30/10/25.
//

#pragma once

namespace KanVasX
{
  /// This structure stores the ImGui Theme Color values
  struct Color
  {
    // Other
    inline static ImU32 Null                  = IM_COL32(0, 0, 0, 0);
    
    inline static ImU32 Black                 = IM_COL32(0, 0, 0, 255);
    inline static ImU32 White                 = IM_COL32(255, 255, 255, 255);
    
    inline static ImU32 Red                   = IM_COL32(215, 55, 25, 255);
    inline static ImU32 Green                 = IM_COL32(15, 40, 252, 255);
    inline static ImU32 Blue                  = IM_COL32(0, 125, 255, 255);
    
    inline static ImU32 Yellow                = IM_COL32(250, 190, 12, 255);
    
    inline static ImU32 Error                 = IM_COL32(215, 55, 25, 255);
    
    inline static ImU32 Highlight             = IM_COL32(20, 90, 222, 222);
    inline static ImU32 HighlightMuted        = IM_COL32(20, 90, 222, 222);
    inline static ImU32 HighlightContrast     = IM_COL32(255, 255, 255, 222);
    
    // Text
    inline static ImU32 Text                  = IM_COL32(55, 35, 12, 255);
    inline static ImU32 TextBright            = IM_COL32(55, 35, 12, 255);
    inline static ImU32 TextMuted             = IM_COL32(55, 35, 12, 255);
    inline static ImU32 TextSelectedBg        = IM_COL32(55, 35, 12, 255);
    inline static ImU32 TextDark              = IM_COL32(55, 35, 12, 255);
    
    // Window
    inline static ImU32 Background            = IM_COL32(55, 35, 12, 255);
    inline static ImU32 BackgroundDark        = IM_COL32(55, 35, 12, 255);
    inline static ImU32 BackgroundLight       = IM_COL32(55, 35, 12, 255);
    inline static ImU32 ChildBackground       = IM_COL32(55, 35, 12, 255);
    inline static ImU32 PopupBackground       = IM_COL32(55, 35, 12, 255);
    inline static ImU32 Border                = IM_COL32(55, 35, 12, 255);
    inline static ImU32 BackgroundShadow      = IM_COL32(55, 35, 12, 255);
    inline static ImU32 BackgroundHighlight   = IM_COL32(55, 35, 12, 255);
    
    // Models
    inline static ImU32 WindowDimBg           = IM_COL32(55, 35, 12, 255);
    
    // Frame : Checkbox BG, Text BG, Drop Menu
    inline static ImU32 FrameBg               = IM_COL32(55, 35, 12, 255);
    inline static ImU32 FrameBgHovered        = IM_COL32(55, 35, 12, 255);
    inline static ImU32 FrameBgActive         = IM_COL32(55, 35, 12, 255);
    
    // Widget Title BG
    inline static ImU32 TitleBar              = IM_COL32(55, 35, 12, 255);
    inline static ImU32 TitleBgActive         = IM_COL32(55, 35, 12, 255);
    inline static ImU32 TitleBgCollapsed      = IM_COL32(55, 35, 12, 255);
    
    // Title menu bar
    inline static ImU32 MenuBarBg             = IM_COL32(55, 35, 12, 255);
    
    // Scrol Bar
    inline static ImU32 ScrollbarBg           = IM_COL32(55, 35, 12, 255);
    inline static ImU32 ScrollbarGrab         = IM_COL32(55, 35, 12, 255);
    inline static ImU32 ScrollbarGrabHovered  = IM_COL32(55, 35, 12, 255);
    inline static ImU32 ScrollbarGrabActive   = IM_COL32(55, 35, 12, 255);
    
    // Slider
    inline static ImU32 SliderGrab            = IM_COL32(55, 35, 12, 255);
    inline static ImU32 SliderGrabActive      = IM_COL32(55, 35, 12, 255);
    
    // Button
    inline static ImU32 Button                = IM_COL32(55, 35, 12, 255);
    inline static ImU32 ButtonMuted           = IM_COL32(55, 35, 12, 255);
    inline static ImU32 ButtonHovered         = IM_COL32(55, 35, 12, 255);
    inline static ImU32 ButtonActive          = IM_COL32(55, 35, 12, 255);
    inline static ImU32 ButtonContrast        = IM_COL32(55, 35, 12, 255);
    
    // Tree Node Headers
    inline static ImU32 Header                = IM_COL32(55, 35, 12, 255);
    inline static ImU32 HeaderHovered         = IM_COL32(55, 35, 12, 255);
    inline static ImU32 HeaderActive          = IM_COL32(55, 35, 12, 255);
    
    // Widget and Column Separator
    inline static ImU32 Separator             = IM_COL32(55, 35, 12, 255);
    inline static ImU32 SeparatorActive       = IM_COL32(55, 35, 12, 255);
    inline static ImU32 SeparatorHovered      = IM_COL32(55, 35, 12, 255);
    
    // Resize
    inline static ImU32 ResizeGrip            = IM_COL32(55, 35, 12, 255);
    inline static ImU32 ResizeGripHovered     = IM_COL32(55, 35, 12, 255);
    inline static ImU32 ResizeGripActive      = IM_COL32(55, 35, 12, 255);
    
    // Widget Tab Tabs
    inline static ImU32 Tab                   = IM_COL32(55, 35, 12, 255);
    inline static ImU32 TabHovered            = IM_COL32(55, 35, 12, 255);
    inline static ImU32 TabActive             = IM_COL32(55, 35, 12, 255);
    inline static ImU32 TabUnfocused          = IM_COL32(55, 35, 12, 255);
    inline static ImU32 TabUnfocusedActive    = IM_COL32(55, 35, 12, 255);
    
    // Docking
    inline static ImU32 DockingPreview        = IM_COL32(55, 35, 12, 255);
    inline static ImU32 DockingEmptyBg        = IM_COL32(55, 35, 12, 255);
    
    // Plot
    inline static ImU32 PlotLines             = IM_COL32(55, 35, 12, 255);
    inline static ImU32 PlotLinesHovered      = IM_COL32(55, 35, 12, 255);
    inline static ImU32 PlotHistogram         = IM_COL32(55, 35, 12, 255);
    inline static ImU32 PlotHistogramHovered  = IM_COL32(55, 35, 12, 255);
    
    // Table
    inline static ImU32 TableHeaderBg         = IM_COL32(55, 35, 12, 255);
    inline static ImU32 TableBorderStrong     = IM_COL32(55, 35, 12, 255);
    inline static ImU32 TableBorderLight      = IM_COL32(55, 35, 12, 255);
    inline static ImU32 TableRowBg            = IM_COL32(55, 35, 12, 255);
    inline static ImU32 TableRowBgAlt         = IM_COL32(55, 35, 12, 255);
    
    // Drag drop
    inline static ImU32 DragDropTarget        = IM_COL32(55, 35, 12, 255);
    
    // Navigation
    inline static ImU32 NavHighlight          = IM_COL32(55, 35, 12, 255);
    inline static ImU32 NavWindowingHighlight = IM_COL32(55, 35, 12, 255);
    inline static ImU32 NavWindowingDimBg     = IM_COL32(55, 35, 12, 255);

    /// This function intializes the theme colors
    static void Initialize();
    
    /// This function converts the ImU32 color to Vec4
    /// - Parameter color: U32 color
    static glm::vec4 Vec4FromU32(const ImU32& color);
    /// This function converts the ImU32 color from Vec4
    /// - Parameter color: vec4 coloe
    static ImU32 U32FromVec4(const glm::vec4& color);
    
    /// This function add the color to some value
    /// - Parameters:
    ///   - color: color
    ///   - value: value
    static ImU32 Value(const ImColor& color, float value);
    /// This function adds the color with saturation
    /// - Parameters:
    ///   - color: color
    ///   - saturation: saturation
    static ImU32 Saturation(const ImColor& color, float saturation);
    /// This function adds the color with Hue
    /// - Parameters:
    ///   - color: color
    ///   - hue: hue
    static ImU32 Hue(const ImColor& color, float hue);
    /// This finction multiply the color with some value
    /// - Parameters:
    ///   - color: color
    ///   - multiplier: multiplier
    static ImU32 MultipliedValue(const ImColor& color, float multiplier);
    /// This finction multiply the color with some saturation
    /// - Parameters:
    ///   - color: color
    ///   - multiplier: multiplier
    static ImU32 MultipliedSaturation(const ImColor& color, float multiplier);
    /// This finction multiply the color with some hue
    /// - Parameters:
    ///   - color: color
    ///   - multiplier: multiplier
    static ImU32 MultipliedHue(const ImColor& color, float multiplier);
    
    /// THis function changes the alpha of color
    /// - Parameters:
    ///   - color: color
    ///   - alpha: new alpha value
    static ImU32 Alpha(const ImColor& color, float alpha);
    
    /// THis function returns the alpha value of color
    /// - Parameter color: color
    static float GetAlpha(const ImColor& color);
    
    /// THis function changes the alpha of color
    /// - Parameters:
    ///   - color1: color 1
    ///   - color2: color 2
    ///   - value: lerp value
    static ImU32 Lerp(const ImColor& color1, const ImColor& color2, float value);
  };
}
