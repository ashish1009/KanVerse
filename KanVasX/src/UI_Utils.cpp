//
//  UI_Utils.cpp
//  KanVasX
//
//  Created by Ashish . on 30/10/25.
//

#include "UI_Utils.hpp"
#include "UI_Scoped.hpp"

namespace KanVasX
{
  // This MACRO Cast uint32_t into void pointer
#define INT2VOIDP(i)    (void*)(uintptr_t)(i)
  
  static int32_t s_UIContextID = 0;
  static uint32_t s_counter = 0;
  static char s_bufferID[16] = "##";
  static char s_labeledBufferID[1024];

  void BeginDisabled(bool disabled)
  {
    if (disabled)
    {
      ImGui::PushStyleColor(ImGuiCol_Text, Color::TextMuted);
      ImGui::BeginDisabled(true);
    }
  }
  void EndDisabled()
  {
    if (GImGui->DisabledStackSize > 0)
    {
      ImGui::PopStyleColor();
      ImGui::EndDisabled();
    }
  }

  void UI::PushID()
  {
    ImGui::PushID(s_UIContextID++);
    s_counter = 0;
  }
  void UI::PopID()
  {
    ImGui::PopID();
    s_UIContextID--;
  }

  const char* UI::GenerateID()
  {
    std::string result = "##";
    result += std::to_string(s_counter++);
    memcpy(s_bufferID, result.c_str(), 16);
    return s_bufferID;
  }
  const char* UI::GenerateLabelID(const std::string_view& label)
  {
    auto formatted = std::format("{}##{}", label, s_counter++);
    std::snprintf(s_labeledBufferID, std::size(s_labeledBufferID), "%s", formatted.c_str());
    return s_labeledBufferID;
  }

  bool UI::IsItemHovered()
  {
    return ImGui::IsItemHovered();
  }
  bool UI::IsItemDisabled()
  {
    return ImGui::GetItemFlags() & ImGuiItemFlags_Disabled;
  }

  ImTextureID UI::GetTextureID(uint32_t textureID)
  {
    return textureID ? (ImTextureID)INT2VOIDP(textureID) : 0;
  }

  void UI::ShiftCursorX(float distance)
  {
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + distance);
  }
  void UI::ShiftCursorY(float distance)
  {
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + distance);
  }
  void UI::ShiftCursor(const ImVec2& val)
  {
    const ImVec2 cursor = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(cursor.x + val.x, cursor.y + val.y));
  }

  void UI::SetCursorPosX(float x)
  {
    ImGui::SetCursorPosX(x);
  }
  void UI::SetCursorPosY(float y)
  {
    ImGui::SetCursorPosY(y);
  }
  void UI::SetCursorPos(const ImVec2& val)
  {
    ImGui::SetCursorPos(val);
  }
  
  void UI::SameLine(float offsetFromStartX, float spacing)
  {
    ImGui::SameLine(offsetFromStartX, spacing);
  }
  void UI::Separator(float alpha)
  {
    ScopedColor Separator(ImGuiCol_Separator, KanVasX::Color::Alpha(KanVasX::Color::Separator, alpha));
    UI::ShiftCursorY(5.0f);
    ImGui::Separator();
  }
  void UI::NewLine()
  {
    ImGui::NewLine();
  }
  
  ImRect UI::GetItemRect()
  {
    return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
  }
  
  ImRect UI::RectExpanded(const ImRect& rect, float x, float y)
  {
    ImRect result = rect;
    result.Min.x -= x;
    result.Min.y -= y;
    result.Max.x += x;
    result.Max.y += y;
    return result;
  }
  
  ImRect UI::RectOffset(const ImRect& rect, const ImVec2& xy)
  {
    return RectOffset(rect, xy.x, xy.y);
  }
  
  ImRect UI::RectOffset(const ImRect& rect, float x, float y)
  {
    ImRect result = rect;
    result.Min.x += x;
    result.Min.y += y;
    result.Max.x += x;
    result.Max.y += y;
    return result;
  }

  void UI::Text(ImFont* imGuiFont, std::string_view string, AlignX xAlign, const glm::vec2& offset, const ImU32& color)
  {
    KanVasX::ScopedFont header(imGuiFont);
    KanVasX::ScopedColor textColor(ImGuiCol_Text, color);
    
    float xOffset = offset.x;
    switch (xAlign)
    {
      case AlignX::Center:
        xOffset += (ImGui::GetColumnWidth() - ImGui::CalcTextSize(string.data()).x) / 2;
        break;
      case AlignX::Right:
        xOffset += (ImGui::GetColumnWidth() - ImGui::CalcTextSize(string.data()).x) ;
        break;
      case AlignX::Left:
      default:
        break;
    }
    
    UI::SetCursorPos({ImGui::GetCursorPosX() + xOffset, ImGui::GetCursorPosY() + offset.y});
    ImGui::Text("%s", string.data());
  }

  void UI::DrawRect(const ImU32& color, float height, float widthFactor, const glm::vec2& offset, float rounding)
  {
    // Draw the title Bar rectangle ---------------------------------------------------
    const ImVec2 titlebarMin =
    {
      ImGui::GetCursorScreenPos().x + offset.x,
      ImGui::GetCursorScreenPos().y + offset.y
    };
    const ImVec2 titlebarMax =
    {
      ImGui::GetCursorScreenPos().x + offset.x + (ImGui::GetWindowWidth() * widthFactor),
      ImGui::GetCursorScreenPos().y + offset.y + height
    };
    
    auto* drawList = ImGui::GetWindowDrawList();
    drawList->AddRect(titlebarMin, titlebarMax, color, rounding);
  }
  void UI::DrawFilledRect(const ImU32& color, float height, float widthFactor, const glm::vec2& offset, float rounding)
  {
    const ImVec2 titlebarMin =
    {
      ImGui::GetCursorScreenPos().x + offset.x,
      ImGui::GetCursorScreenPos().y + offset.y
    };
    const ImVec2 titlebarMax =
    {
      ImGui::GetCursorScreenPos().x + offset.x + (ImGui::GetWindowWidth() * widthFactor),
      ImGui::GetCursorScreenPos().y + offset.y + height
    };
    
    auto* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(titlebarMin, titlebarMax, color, rounding);
  }
  
  // Image -----------------------------------------------------------------------------------------------------------------------------------------
  void UI::Image(ImTextureID textureId, const ImVec2& size, const ImU32& tintCol, const ImU32& borderCol, const ImVec2& uv0, const ImVec2& uv1)
  {
    ImGui::Image(textureId, size, uv0, uv1, ImGui::ColorConvertU32ToFloat4(tintCol), ImGui::ColorConvertU32ToFloat4(borderCol));
  }

  void UI::DrawItemActivityOutline(float rounding, bool drawWhenInactive, const ImColor& colorWhenActive)
  {
    auto* drawList = ImGui::GetWindowDrawList();
    const ImRect rect = RectExpanded(GetItemRect(), 0.1f, 0.1f);
    if (ImGui::IsItemHovered() and !ImGui::IsItemActive())
    {
      drawList->AddRect(rect.Min, rect.Max, Color::Lerp(colorWhenActive, Color::TitleBar, 0.3), rounding, 0, 1.5f);
    }
    if (ImGui::IsItemActive())
    {
      drawList->AddRect(rect.Min, rect.Max, colorWhenActive, rounding, 0, 2.0f);
    }
    else if (!ImGui::IsItemHovered() and drawWhenInactive)
    {
      drawList->AddRect(rect.Min, rect.Max, Color::Lerp(colorWhenActive, Color::TitleBar, 0.5), rounding, 0, 1.0f);
    }
  }
  void UI::DrawUnderline(const ImU32& color, bool fullWidth, float offsetX, float offsetY)
  {
    if (fullWidth)
    {
      if (ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr)
      {
        ImGui::PushColumnsBackground();
      }
      else if (ImGui::GetCurrentTable() != nullptr)
      {
        ImGui::TablePushBackgroundChannel();
      }
    }
    
    const float width = fullWidth ? ImGui::GetWindowWidth() : ImGui::GetContentRegionAvail().x;
    UI::ShiftCursorY(5);
    const ImVec2 cursor = ImGui::GetCursorScreenPos();
    ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x + offsetX, cursor.y + offsetY), ImVec2(cursor.x + width, cursor.y + offsetY), color, 1.0f);
    
    if (fullWidth)
    {
      if (ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr)
      {
        ImGui::PopColumnsBackground();
      }
      else if (ImGui::GetCurrentTable() != nullptr)
      {
        ImGui::TablePopBackgroundChannel();
      }
    }
  }

  void DrawButtonImageImpl(ImTextureID imageNormal, ImTextureID imageHovered, ImTextureID imagePressed,
                       const ImVec2& rectMin, const ImVec2& rectMax,
                       const ImU32& normalColor, const ImU32& hoveredColor, const ImU32& pressedColor)
  {
    auto* drawList = ImGui::GetWindowDrawList();
    if (ImGui::IsItemActive())
    {
      drawList->AddImage(imagePressed, rectMin, rectMax, ImVec2(0, 1), ImVec2(1, 0), pressedColor);
    }
    else if (ImGui::IsItemHovered())
    {
      drawList->AddImage(imageHovered, rectMin, rectMax, ImVec2(0, 1), ImVec2(1, 0), hoveredColor);
    }
    else
    {
      drawList->AddImage(imageNormal, rectMin, rectMax, ImVec2(0, 1), ImVec2(1, 0), normalColor);
    }
  }
  
  void DrawButtonImageUtil(ImTextureID image, const ImRect& rectangle,
                       const ImU32& normalColor = Color::White, const ImU32& hoveredColor = Color::White, const ImU32& pressedColor = Color::White)
  {
    DrawButtonImageImpl(image, image, image, rectangle.Min, rectangle.Max, normalColor, hoveredColor, pressedColor);
  }
  bool UI::DrawButtonImage(const std::string_view title, ImTextureID image, bool highlight, const ImVec2& size, const ImVec2& offset,
                       const ImU32& normalColor, const ImU32& hoveredColor, const ImU32& pressedColor)
  {
    // Shift the offset
    UI::ShiftCursor(offset);
    
    // Invisible buttons to be drawn
    const bool clicked = ImGui::InvisibleButton(title.data(), size, ImGuiButtonFlags_AllowItemOverlap);
    
    // Modify the icon size if hovered
    ImVec2 iconSizeModified = size;
    if (IsItemHovered() and highlight)
    {
      iconSizeModified.x += 20.0f;
      iconSizeModified.y += 20.0f;
    }
    
    const ImVec2 logoRectStart =
    {
      ImGui::GetItemRectMin().x,
      ImGui::GetItemRectMin().y
    };
    const ImVec2 logoRectMax =
    {
      logoRectStart.x + iconSizeModified.x,
      logoRectStart.y + iconSizeModified.y
    };
    
    DrawButtonImageUtil(image, ImRect{logoRectStart, logoRectMax}, normalColor, hoveredColor, pressedColor);
    return clicked;
  }
  
  bool UI::DrawButton(std::string_view title, ImFont* imGuiFont, const ImU32& buttonColor, const ImU32& textColor, bool inactive, float rounding, const ImVec2& size)
  {
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    ImGui::PushID(title.data());
    
    ScopedStyle roundingStyle(ImGuiStyleVar_FrameRounding, rounding);
    ScopedFont headerStyle(imGuiFont);
    
    if (inactive)
    {
      ImGui::PushStyleColor(ImGuiCol_Button, Color::ButtonMuted);
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Color::ButtonMuted);
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, Color::ButtonMuted);
      
      ImGui::PushStyleColor(ImGuiCol_BorderShadow, Color::Lerp(Color::BackgroundShadow, Color::ButtonMuted, 0.9));
      ImGui::PushStyleColor(ImGuiCol_Border, Color::Lerp(Color::Border, Color::ButtonMuted, 0.9));
      
      ImGui::PushStyleColor(ImGuiCol_Text, Color::TextMuted);
    }
    else
    {
      ImU32 tintNormal = Color::Alpha(buttonColor, 0.6f);
      ImU32 tintHovered = Color::Alpha(buttonColor, 1.1f);
      ImU32 tintPressed = Color::Alpha(buttonColor, 0.3f);
      
      ImGui::PushStyleColor(ImGuiCol_Button, tintNormal);
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, tintHovered);
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, tintPressed);
    }
    
    bool clicked = ImGui::Button(title.data(), size);
    
    ImGui::PopStyleColor(3);
    if (inactive)
    {
      ImGui::PopStyleColor(3);
    }
    ImGui::PopID();
    ImGui::PopStyleColor();
    return inactive ? false : clicked;
  }

  // Shadow ----------------------------------------------------------------------------------------------------------------------------------------
  void DrawShadowInnerImpl(ImTextureID shadowImageID, UI::Position position, int radius, const ImVec2& rectMin,
                           const ImVec2& rectMax, float alpha, float lengthStretch)
  {
    const float widthOffset = lengthStretch;
    const float alphaTop = alpha; //std::min(0.25f * alphMultiplier, 1.0f);
    const float alphaSides = alpha; //std::min(0.30f * alphMultiplier, 1.0f);
    const float alphaBottom = alpha; //std::min(0.60f * alphMultiplier, 1.0f);
    const auto p1 = ImVec2(rectMin.x + radius, rectMin.y + radius);
    const auto p2 = ImVec2(rectMax.x - radius, rectMax.y - radius);
    auto* drawList = ImGui::GetWindowDrawList();
    
    switch (position)
    {
      case UI::Position::Top:
        drawList->AddImage(shadowImageID, { p1.x - widthOffset,  p1.y - radius }, { p2.x + widthOffset, p1.y },
                           ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImColor(0.0f, 0.0f, 0.0f, alphaTop));
        break;
      case UI::Position::Bottom:
        drawList->AddImage(shadowImageID, { p1.x - widthOffset,  p2.y }, { p2.x + widthOffset, p2.y + radius },
                           ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), ImColor(0.0f, 0.0f, 0.0f, alphaBottom));
        break;
      case UI::Position::Left:
        drawList->AddImageQuad(shadowImageID, { p1.x - radius, p1.y - widthOffset }, { p1.x, p1.y - widthOffset },
                               { p1.x, p2.y + widthOffset }, { p1.x - radius, p2.y + widthOffset },
                               { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f },
                               ImColor(0.0f, 0.0f, 0.0f, alphaSides));
        break;
      case UI::Position::Right:
        drawList->AddImageQuad(shadowImageID, { p2.x, p1.y - widthOffset }, { p2.x + radius, p1.y - widthOffset },
                               { p2.x + radius, p2.y + widthOffset }, { p2.x, p2.y + widthOffset },
                               { 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f },
                               ImColor(0.0f, 0.0f, 0.0f, alphaSides));
        break;
      default:
        break;
    }
  }
  
  void DrawShadowInner(ImTextureID shadowImageID, UI::Position position, int radius, const ImRect& rectangle, float alpha, float lengthStretch)
  {
    DrawShadowInnerImpl(shadowImageID, position, radius, rectangle.Min, rectangle.Max, alpha, lengthStretch);
  }
  
  void UI::DrawShadow(ImTextureID shadowImageID, Position position, int32_t radius, float alpha)
  {
    // Draw side shadow
    ImRect windowRect = UI::RectExpanded(ImGui::GetCurrentWindow()->Rect(), 0.0f, 0.0f);
    ImGui::PushClipRect(windowRect.Min, windowRect.Max, false);
    DrawShadowInner(shadowImageID, position, radius, windowRect, alpha, windowRect.GetHeight() / 4.0f);
    ImGui::PopClipRect();
  }
  
  void UI::DrawShadowAllDirection(ImTextureID shadowImageID, int32_t radius, float alpha)
  {
    UI::DrawShadow(shadowImageID, UI::Position::Left, radius, alpha);
    UI::DrawShadow(shadowImageID, UI::Position::Right, radius, alpha);
    UI::DrawShadow(shadowImageID, UI::Position::Top, radius, alpha);
    UI::DrawShadow(shadowImageID, UI::Position::Bottom, radius, alpha);
  }
} // namespace KanVasX
