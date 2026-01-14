//
//  UI_Utils.cpp
//  KanVest
//
//  Created by Ashish . on 14/01/26.
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
  std::string FormatLargeNumber(double value)
  {
    const double billion = 1e9, million = 1e6, thousand = 1e3;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    if (value >= billion)
      oss << (value / billion) << "B";
    else if (value >= million)
      oss << (value / million) << "M";
    else if (value >= thousand)
      oss << (value / thousand) << "K";
    else
      oss << value;
    
    return oss.str();
  }
} // namespace KanVest::UI::Utils
