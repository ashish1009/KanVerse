//
//  UI_Theme.cpp
//  KanVasX
//
//  Created by Ashish . on 30/10/25.
//

#include "UI_Theme.hpp"

namespace KanVasX
{
  void Color::Initialize()
  {
    // Get Imgui style to overrite theme
    ImGuiStyle& imGuiStyle = ImGui::GetStyle();
    
    // ImGui Style ---------------------------------------------------------------------------------------------------------------------------------
    Settings::FrameRounding = 6.0f;
    Settings::FrameHeight = 8.0f;
    
    imGuiStyle.FrameRounding = Settings::FrameRounding;
    imGuiStyle.TabRounding = std::min(Settings::FrameRounding, 5.0f);
    imGuiStyle.ScrollbarRounding = Settings::FrameRounding;
    
    imGuiStyle.WindowRounding = 0.0f;
    imGuiStyle.ChildRounding = 0.0;
    imGuiStyle.PopupRounding = 0.0;
    imGuiStyle.GrabRounding = 0.0;
    
    imGuiStyle.WindowBorderSize = 1.0f;
    imGuiStyle.ChildBorderSize = 1.0f;
    imGuiStyle.PopupBorderSize = 1.0f;
    imGuiStyle.FrameBorderSize = 1.0f;
    imGuiStyle.TabBorderSize = 1.0f;
    
    // Colors ----------------------------------------------------------------------------------------------------------------------------------------
    // Values -----------------------------
    // Window
    BackgroundDark        = IM_COL32(2, 9, 14, 255);
    Background            = IM_COL32(12, 19, 24, 255);
    BackgroundLight       = IM_COL32(25, 32, 38, 255);

    // Highlight
    Highlight             = IM_COL32(40, 48, 58, 255);;
    HighlightContrast     = IM_COL32(5, 5, 5, 255);
    
    // Text
    Text                  = IM_COL32(240, 240, 240, 222);
    TextBright            = IM_COL32(255, 255, 255, 255);
    
    // Frame : Checkbox BG, Text BG, Drop Menu
    FrameBg               = IM_COL32(24, 30, 36, 255);
    
    // Commons ----------------------------
    // Highlight
    HighlightMuted        = Value(Highlight, 0.3f);
    
    // Widget and Column Separator
    Separator             = IM_COL32(40, 48, 58, 255);;
    SeparatorHovered      = MultipliedValue(Highlight, 0.5f);
    SeparatorActive       = Alpha(Separator, 0.3);
    
    // Text
    TextMuted             = Value(Text, 0.5f);
    TextDark              = Value(Text, 1.5f);
    TextSelectedBg        = HighlightMuted;
    
    // Window
    Border                = Alpha(Separator, 0.3f);
    PopupBackground       = Alpha(Background, 0.6f);
    BackgroundShadow      = Alpha(Highlight, 0.07f);
    ChildBackground       = Alpha(BackgroundDark, 0.6f);
    BackgroundHighlight   = Value(Background, 1.6);
    
    // Model
    WindowDimBg           = Alpha(BackgroundDark, 0.3f);
    
    // Frame : Checkbox BG, Text BG, Drop Menu
    FrameBgHovered        = MultipliedValue(FrameBg, 1.2f);
    FrameBgActive         = Alpha(FrameBg, 0.3f);
    
    // Widget Title BG
    TitleBar              = Alpha(Saturation(Background, 0.3f), 1.0f);
    TitleBgActive         = Alpha(TitleBar, 0.2f);
    TitleBgCollapsed      = Alpha(TitleBar, 0.3f);
    
    // Resize
    ResizeGrip            = MultipliedValue(Highlight, 1.6f);
    ResizeGripHovered     = MultipliedValue(Highlight, 2.9f);
    ResizeGripActive      = MultipliedValue(Highlight, 0.9f);
    
    // Tree Node Headers
    Header                = Value(HighlightMuted, 0.3f);
    HeaderHovered         = MultipliedValue(Header, 1.1f);
    HeaderActive          = Alpha(Header, 0.3f);
    
    // Table
    TableHeaderBg         = Header;
    TableBorderLight      = Separator;
    TableBorderStrong     = Alpha(Separator, 0.5f);
    TableRowBgAlt         = Alpha(Background, 0.6f);
    
    // Button
    Button                = Saturation(Highlight, 0.4f);
    ButtonMuted           = Alpha(PopupBackground, 0.5f);
    ButtonHovered         = MultipliedValue(Button, 1.2f);
    ButtonActive          = Alpha(Button, 0.3f);
    ButtonContrast        = Saturation(HighlightContrast, 0.3f);
    
    // Widget Tab Tabs
    Tab                   = Alpha(Lerp(Highlight, Background, 0.1f), 0.6f);
    TabActive             = Alpha(Tab, 1.0f);
    TabHovered            = MultipliedValue(TabActive, 1.2f);
    TabUnfocused          = Alpha(Tab, 0.6f);
    TabUnfocusedActive    = Alpha(TabActive, 0.6f);
    
    // Scrol Bar
    ScrollbarBg           = Lerp(Highlight, Background, 0.98);
    ScrollbarGrab         = Lerp(Highlight, Background, 0.3);
    ScrollbarGrabHovered  = Lerp(Highlight, Background, 0.1);
    ScrollbarGrabActive   = Lerp(Highlight, Background, 0.4);
    
    // Slider
    SliderGrab            = Alpha(Value(Highlight, 0.8f), 0.8f);
    SliderGrabActive      = Value(Highlight, 0.9f);
    
    // Docking
    DockingPreview        = MultipliedValue(Background, 2.2f);
    DockingEmptyBg        = MultipliedValue(Background, 1.5f);
    
    // Drop Target
    DragDropTarget        = Highlight;

    // Common Style -----------------------------------------------
    ImVec4* colors = imGuiStyle.Colors;
    
    // Text
    colors[ImGuiCol_Text]                   = ImGui::ColorConvertU32ToFloat4(Text);
    colors[ImGuiCol_TextDisabled]           = ImGui::ColorConvertU32ToFloat4(TextMuted);
    colors[ImGuiCol_TextSelectedBg]         = ImGui::ColorConvertU32ToFloat4(TextSelectedBg);
    
    // Window
    colors[ImGuiCol_WindowBg]               = ImGui::ColorConvertU32ToFloat4(Background);
    colors[ImGuiCol_ChildBg]                = ImGui::ColorConvertU32ToFloat4(ChildBackground);
    colors[ImGuiCol_PopupBg]                = ImGui::ColorConvertU32ToFloat4(PopupBackground);
    colors[ImGuiCol_Border]                 = ImGui::ColorConvertU32ToFloat4(Border);
    colors[ImGuiCol_BorderShadow]           = ImGui::ColorConvertU32ToFloat4(BackgroundShadow);
    
    // Model
    colors[ImGuiCol_ModalWindowDimBg]       = ImGui::ColorConvertU32ToFloat4(WindowDimBg);
    
    // Frame : Checkbox BG, Text BG, Drop Menu
    colors[ImGuiCol_FrameBg]                = ImGui::ColorConvertU32ToFloat4(FrameBg);
    colors[ImGuiCol_FrameBgHovered]         = ImGui::ColorConvertU32ToFloat4(FrameBgHovered);
    colors[ImGuiCol_FrameBgActive]          = ImGui::ColorConvertU32ToFloat4(FrameBgActive);
    
    // Widget Title BG
    colors[ImGuiCol_TitleBg]                = ImGui::ColorConvertU32ToFloat4(TitleBar);
    colors[ImGuiCol_TitleBgActive]          = ImGui::ColorConvertU32ToFloat4(TitleBgActive);
    colors[ImGuiCol_TitleBgCollapsed]       = ImGui::ColorConvertU32ToFloat4(TitleBgCollapsed);
    
    // Title Menu BG
    colors[ImGuiCol_MenuBarBg]              = ImGui::ColorConvertU32ToFloat4(MenuBarBg);
    
    // Scrol Bar
    colors[ImGuiCol_ScrollbarBg]            = ImGui::ColorConvertU32ToFloat4(ScrollbarBg);
    colors[ImGuiCol_ScrollbarGrab]          = ImGui::ColorConvertU32ToFloat4(ScrollbarGrab);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImGui::ColorConvertU32ToFloat4(ScrollbarGrabHovered);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImGui::ColorConvertU32ToFloat4(ScrollbarGrabActive);
    
    // Check Mark
    colors[ImGuiCol_CheckMark]              = ImGui::ColorConvertU32ToFloat4(Text);
    
    // Slider
    colors[ImGuiCol_SliderGrab]             = ImGui::ColorConvertU32ToFloat4(SliderGrab);
    colors[ImGuiCol_SliderGrabActive]       = ImGui::ColorConvertU32ToFloat4(SliderGrabActive);
    
    // Button
    colors[ImGuiCol_Button]                 = ImGui::ColorConvertU32ToFloat4(Button);
    colors[ImGuiCol_ButtonHovered]          = ImGui::ColorConvertU32ToFloat4(ButtonHovered);
    colors[ImGuiCol_ButtonActive]           = ImGui::ColorConvertU32ToFloat4(ButtonActive);
    
    // Tree Node Headers
    colors[ImGuiCol_Header]                 = ImGui::ColorConvertU32ToFloat4(Header);
    colors[ImGuiCol_HeaderHovered]          = ImGui::ColorConvertU32ToFloat4(HeaderHovered);
    colors[ImGuiCol_HeaderActive]           = ImGui::ColorConvertU32ToFloat4(HeaderActive);
    
    // Widget and Column Separator
    colors[ImGuiCol_Separator]              = ImGui::ColorConvertU32ToFloat4(Separator);
    colors[ImGuiCol_SeparatorActive]        = ImGui::ColorConvertU32ToFloat4(SeparatorActive);
    colors[ImGuiCol_SeparatorHovered]       = ImGui::ColorConvertU32ToFloat4(SeparatorHovered);
    
    // Resize
    colors[ImGuiCol_ResizeGrip]             = ImGui::ColorConvertU32ToFloat4(ResizeGrip);
    colors[ImGuiCol_ResizeGripHovered]      = ImGui::ColorConvertU32ToFloat4(ResizeGripHovered);
    colors[ImGuiCol_ResizeGripActive]       = ImGui::ColorConvertU32ToFloat4(ResizeGripActive);
    
    // Widget Tab Tabs
    colors[ImGuiCol_TabHovered]             = ImGui::ColorConvertU32ToFloat4(TabHovered);
    colors[ImGuiCol_Tab]                    = ImGui::ColorConvertU32ToFloat4(Tab);
    colors[ImGuiCol_TabActive]              = ImGui::ColorConvertU32ToFloat4(TabActive);
    colors[ImGuiCol_TabUnfocused]           = ImGui::ColorConvertU32ToFloat4(TabUnfocused);
    colors[ImGuiCol_TabUnfocusedActive]     = ImGui::ColorConvertU32ToFloat4(TabUnfocusedActive);
    
    // Docking
    colors[ImGuiCol_DockingPreview]         = ImGui::ColorConvertU32ToFloat4(DockingPreview);
    colors[ImGuiCol_DockingEmptyBg]         = ImGui::ColorConvertU32ToFloat4(DockingEmptyBg);
    
    // Plot
    colors[ImGuiCol_PlotLines]              = ImGui::ColorConvertU32ToFloat4(Red);
    colors[ImGuiCol_PlotLinesHovered]       = ImGui::ColorConvertU32ToFloat4(Green);
    colors[ImGuiCol_PlotHistogram]          = ImGui::ColorConvertU32ToFloat4(Blue);
    colors[ImGuiCol_PlotHistogramHovered]   = ImGui::ColorConvertU32ToFloat4(Highlight);
    
    // Table
    colors[ImGuiCol_TableHeaderBg]          = ImGui::ColorConvertU32ToFloat4(TableHeaderBg);
    colors[ImGuiCol_TableBorderLight]       = ImGui::ColorConvertU32ToFloat4(TableBorderLight);
    colors[ImGuiCol_TableBorderStrong]      = ImGui::ColorConvertU32ToFloat4(TableBorderStrong);
    colors[ImGuiCol_TableRowBg]             = ImGui::ColorConvertU32ToFloat4(TableRowBg);
    colors[ImGuiCol_TableRowBgAlt]          = ImGui::ColorConvertU32ToFloat4(TableRowBgAlt);
    
    // Drag Drop
    colors[ImGuiCol_DragDropTarget]         = ImGui::ColorConvertU32ToFloat4(DragDropTarget);
    
    // Navigation
    colors[ImGuiCol_NavHighlight]           = ImGui::ColorConvertU32ToFloat4(NavHighlight);
    colors[ImGuiCol_NavWindowingHighlight]  = ImGui::ColorConvertU32ToFloat4(NavWindowingHighlight);
    colors[ImGuiCol_NavWindowingDimBg]      = ImGui::ColorConvertU32ToFloat4(NavWindowingDimBg);
  }
  
  ImU32 Color::Value(const ImColor& color, float value)
  {
    const ImVec4& colRow = color.Value;
    float hue, sat, val;
    ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
    return ImColor::HSV(hue, sat, std::min(value, 1.0f));
  }
  
  ImU32 Color::Saturation(const ImColor& color, float saturation)
  {
    const ImVec4& colRow = color.Value;
    float hue, sat, val;
    ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
    return ImColor::HSV(hue, std::min(saturation, 1.0f), val);
  }
  
  ImU32 Color::Hue(const ImColor& color, float hue)
  {
    const ImVec4& colRow = color.Value;
    float h, s, v;
    ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, h, s, v);
    return ImColor::HSV(std::min(hue, 1.0f), s, v);
  }
  
  ImU32 Color::MultipliedValue(const ImColor& color, float multiplier)
  {
    const ImVec4& colRow = color.Value;
    float hue, sat, val;
    ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
    return ImColor::HSV(hue, sat, std::min(val * multiplier, 1.0f));
  }
  
  ImU32 Color::MultipliedSaturation(const ImColor& color, float multiplier)
  {
    const ImVec4& colRow = color.Value;
    float hue, sat, val;
    ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
    return ImColor::HSV(hue, std::min(sat * multiplier, 1.0f), val);
  }
  
  ImU32 Color::MultipliedHue(const ImColor& color, float multiplier)
  {
    const ImVec4& colRow = color.Value;
    float hue, sat, val;
    ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
    return ImColor::HSV(std::min(hue * multiplier, 1.0f), sat, val);
  }
  
  ImU32 Color::Alpha(const ImColor &color, float alpha)
  {
    return ImGui::ColorConvertFloat4ToU32({color.Value.x, color.Value.y, color.Value.z, alpha});
  }
  
  float Color::GetAlpha(const ImColor &color)
  {
    return color.Value.w;
  }
  
  ImU32 Color::Lerp(const ImColor &color1, const ImColor &color2, float value)
  {
    return ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(color1), ImGui::ColorConvertU32ToFloat4(color2), value));
  }
  
  glm::vec4 Color::Vec4FromU32(const ImU32& color)
  {
    ImVec4 result = ImGui::ColorConvertU32ToFloat4(color);
    return {result.x, result.y, result.z, result.w};
  }
  ImU32 Color::U32FromVec4(const glm::vec4& color)
  {
    return ImGui::ColorConvertFloat4ToU32({color.r, color.g, color.b, color.a});
  }
} // namespace KanVasX
