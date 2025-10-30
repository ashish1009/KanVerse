//
//  UI_Utils.cpp
//  KanVasX
//
//  Created by Ashish . on 30/10/25.
//

#include "UI_Utils.hpp"

namespace KanVasX
{
  // This MACRO Cast uint32_t into void pointer
#define INT2VOIDP(i)    (void*)(uintptr_t)(i)

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
} // namespace KanVasX
