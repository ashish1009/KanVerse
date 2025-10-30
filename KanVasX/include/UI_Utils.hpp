//
//  UI_Utils.hpp
//  KanVasX
//
//  Created by Ashish . on 30/10/25.
//

#pragma once

namespace KanVasX
{
  class UI
  {
  public:
    /// This function shift the cursor position for x
    /// - Parameter distance: val x
    static void SetCursorPosX(float x);
    /// This function shift the cursor position for y
    /// - Parameter distance: val y
    static void SetCursorPosY(float y);
    /// This function shift the cursor position
    /// - Parameter val: New shift value
    static void SetCursorPos(const ImVec2& val);
  };
} // namespace KanVasX
