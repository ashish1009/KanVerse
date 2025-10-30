//
//  UI_Utils.hpp
//  KanVasX
//
//  Created by Ashish . on 30/10/25.
//

#pragma once

#include "UI_Theme.hpp"

namespace KanVasX
{
  class UI
  {
  public:
    /// This enum stores the ImGui item position in X Axis
    /// Do not chnage Values ....
    enum class AlignX : uint8_t
    {
      Left = 0,
      Center = 1,
      Right = 2
    };
    
    /// This enum stores the ImGui item position
    enum class Position : uint8_t
    {
      Left, Right, Top, Bottom
    };
    
    // Begin / End -----------------------------------------------------------------------------------------------------------------------------------
    /// This function begin disable
    static void BeginDisabled(bool disabled = true);
    /// This function Ends the disable
    static void EndDisabled();

    // Wrappers --------------------------------------------------------------------------------------------------------------------------------------
    /// This function checkes is imgui item hovered
    static bool IsItemHovered();
    /// This function checles is imgui item disabled
    static bool IsItemDisabled();
    
    /// This function generate unique ID for ImGui
    static const char* GenerateID();
    /// This function generate Unique id for labeled widget
    /// - Parameter label: Widget lable
    static const char* GenerateLabelID(const std::string_view& label);

    /// This function return ImTexture id from ID
    /// - Parameter ID: Client texture textureID
    static ImTextureID GetTextureID(uint32_t textureID);

    // Cursor ------------------------------------------------------------------------------------------------------------------------------------------
    /// This function shift the cursor position for x
    /// - Parameter distance: distance x
    static void ShiftCursorX(float distance);
    /// This function shift the cursor position for y
    /// - Parameter distance: distance y
    static void ShiftCursorY(float distance);
    /// This function shift the cursor position
    /// - Parameter val: new shifted value
    static void ShiftCursor(const ImVec2& val);

    /// This function shift the cursor position for x
    /// - Parameter distance: val x
    static void SetCursorPosX(float x);
    /// This function shift the cursor position for y
    /// - Parameter distance: val y
    static void SetCursorPosY(float y);
    /// This function shift the cursor position
    /// - Parameter val: New shift value
    static void SetCursorPos(const ImVec2& val);
    
    /// This function create same line
    /// - Parameters:
    ///   - offsetFromStartX: offset from sstart point
    ///   - spacing: space after last item
    static void SameLine(float offsetFromStartX = 0.0f, float spacing = -1.0f);

    // Texts -----------------------------------------------------------------------------------------------------------------------------------------
    /// This function renders the text
    /// - Parameters:
    ///   - type: Type of font
    ///   - string: string to be printed
    ///   - xAlign: alignment in x axis
    ///   - offset: offset from current cursor position
    static void Text(ImFont* imGuiFont, std::string_view string, AlignX xAlign, const glm::vec2& offset = {0, 0}, const ImU32& color = Color::Text);

    // Draw ------------------------------------------------------------------------------------------------------------------------------------------
    /// This function draw hollow rectangle
    /// - Parameters:
    ///   - color: color of rectangle
    ///   - height: height of rectangle
    ///   - widthFactor: factor of width to window width (0.5 means half width of current cursor till width)
    ///   - offset: offset position
    ///   - rounding: rounding factor
    static void DrawRect(const ImU32& color, float height, float widthFactor = 1.0f, const glm::vec2& offset = {0, 0}, float rounding = Settings::FrameRounding);
    /// This function draw filled rectangle
    /// - Parameters:
    ///   - color: color of rectangle
    ///   - height: height of rectangle
    ///   - widthFactor: factor of width to window width (0.5 means half width of current cursor till width)
    ///   - offset: offset position
    ///   - rounding: rounding factor
    static void DrawFilledRect(const ImU32& color, float height, float widthFactor = 1.0f, const glm::vec2& offset = {0, 0}, float rounding = Settings::FrameRounding);

    // Buttons ---------------------------------------------------------------------------------------------------------------------------------------
    /// This function draws the image button
    /// - Parameters:
    ///   - title: Title of image button
    ///   - image: image icon
    ///   - size: Size of image icon
    ///   - offset: Image icon offset
    ///   - textNormalColor: Text color normal
    ///   - textHoveredColor: Text color when hovered
    ///   - textPressedColor: Text color when pressed
    static bool DrawButtonImage(const std::string_view title, ImTextureID image, bool highlight,
                                const ImVec2& size, const ImVec2& offset = {0, 0},
                                const ImU32& normalColor = Color::White, const ImU32& hoveredColor = Color::White, const ImU32& pressedColor = Color::White);
  };
} // namespace KanVasX
