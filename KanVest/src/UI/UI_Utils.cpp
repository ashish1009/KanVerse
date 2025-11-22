//
//  UI_Utils.cpp
//  KanVest
//
//  Created by Ashish . on 22/11/25.
//

#include "UI_Utils.hpp"

namespace KanVest::UI::Utils
{
  std::string FormatDoubleToString(double value)
  {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%.2f", value);
    return std::string(buf);
  }
} // namespace KanVest::UI::Utils
