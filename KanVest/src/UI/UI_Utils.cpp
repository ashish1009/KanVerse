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
  std::string FormatWithCommas(int64_t value)
  {
    std::string s = std::to_string(value);
    int32_t insertPos = (int32_t)s.length() - 3;
    
    while (insertPos > 0)
    {
      s.insert(insertPos, ",");
      insertPos -= 3;
    }
    return s;
  }
} // namespace KanVest::UI::Utils
