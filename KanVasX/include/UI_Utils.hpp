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
    /// This function push unque id for ImGui
    static void PushID();
    /// This function pop last used id for ImGui
    static void PopID();

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

    /// This function show tooltip
    /// - Parameter desc: hint
    static void HelpMarker(const char* desc);
    /// This function show tooltip
    /// - Parameter desc: hint
    static void Tooltip(const std::string& desc);

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

    /// This function create same line
    static void NewLine();
    /// This function create separtor
    static void Separator(float alpha = 0.08f);

    // Rectangles API --------------------------------------------------------------------------------------------------------------------------------
    /// This function returns the ImGui Item Rectangle vertex
    static ImRect GetItemRect();
    /// This function expand the ImGui Retangle
    /// - Parameters:
    ///   - rect: rectangle
    ///   - x: expand x
    ///   - y: expand y
    static ImRect RectExpanded(const ImRect& rect, float x, float y);
    /// This function Offset the rectangle
    /// - Parameters:
    ///   - rect: rectangle pos
    ///   - x: x offset
    ///   - y: y offset
    static ImRect RectOffset(const ImRect& rect, float x, float y);
    /// This function Offset the rectangle
    /// - Parameters:
    ///   - rect: rectangle pos
    ///   - xy: offset
    static ImRect RectOffset(const ImRect& rect, const ImVec2& xy);

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
    /// This function draw the Activity outline
    /// - Parameters:
    ///   - rounding: Outline rounding
    ///   - drawWhenInactive: Flag to draw when inactive
    ///   - colourWhenActive: Color when active
    static void DrawItemActivityOutline(float rounding = Settings::FrameRounding, bool drawWhenInactive = false, const ImColor& colourWhenActive = ImColor(80, 80, 80));
    /// This function renders the line in Ingui
    /// - Parameters:
    ///   - color: color of line
    ///   - fullWidth: cover full widget of widget
    ///   - offsetX: X offset
    ///   - offsetY: X offset
    static void DrawUnderline(const ImU32& color = Color::Separator, bool fullWidth = true, float offsetX = 0.0f, float offsetY = 1.0f);

    
    // Image -----------------------------------------------------------------------------------------------------------------------------------------
    /// This function renders the imgui image
    /// - Parameters:
    ///   - texture: Ikan Image refernce
    ///   - size: size of imgui image
    ///   - color: override color of image
    ///   - bodorColor: boder color of image
    ///   - uv0 top left corner border of image
    ///   - uv1 bottom right corner border of image
    static void Image(ImTextureID textureId, const ImVec2& size,
                      const ImU32& color = Color::White, const ImU32& borderColor = Color::Null,
                      const ImVec2& uv0 = {0, 1}, const ImVec2& uv1 = {1, 0});

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
    /// This function draw the button with string
    /// - Parameters:
    ///   - title: button title
    ///   - fontType: Button font instance
    ///   - textColor: button text color
    ///   - buttonColor: button color
    ///   - ZeroVec2: button size
    ///   - rounding: button edge rounding
    ///   - size: button extra size
    static bool DrawButton(std::string_view title, ImFont* imGuiFont, const ImU32& buttonColor = Color::Button, const ImU32& textColor = Color::Text,
                           bool inactive = false, float rounding = Settings::FrameRounding, const ImVec2& size = {0, 0});

    // Shadow ----------------------------------------------------------------------------------------------------------------------------------------
    /// This function render the Shadow Texture in ImGui
    /// - Parameters:
    ///   - position: position of shadow
    ///   - radius: radius of shadow
    ///   - alpha: Blurr alpha
    static void DrawShadow(ImTextureID shadowImageID, Position position = Position::Right, int32_t radius = 15.0f, float alpha = 1.0f);
    /// This function render the Shadow Texture in ImGui in all directions
    /// - Parameters:
    ///   - radius: radius of shadow
    ///   - alpha: Blurr alpha
    static void DrawShadowAllDirection(ImTextureID shadowImageID, int32_t radius = 15.0f, float alpha = 1.0f);
  };
} // namespace KanVasX
