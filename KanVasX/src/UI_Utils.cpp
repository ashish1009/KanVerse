//
//  UI_Utils.cpp
//  KanVasX
//
//  Created by Ashish . on 30/10/25.
//

#include "UI_Utils.hpp"

namespace KanVasX
{
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
} // namespace KanVasX
