//
//  MouseButtonCodes.h
//  KanViz
//
//  Created by Ashish . on 22/08/25.
//

#pragma once

namespace KanViz
{
  /// This enum stores the mouse button codes
  /// - Note: From glfw3.h
  enum class MouseButton : uint8_t
  {
    Button0 = 0,
    Button1 = 1,
    Button2 = 2,
    Button3 = 3,
    Button4 = 4,
    Button5 = 5,
    Button6 = 6,
    Button7 = 7,
    
    Last   = Button7,
    Left   = Button0,
    Right  = Button1,
    Middle = Button2
  };
} // namespace KanViz
