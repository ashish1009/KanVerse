//
//  UI_Data.hpp
//  KanVasX
//
//  Created by Ashish . on 01/11/25.
//

#pragma once

namespace KanVasX
{
  struct Date
  {
    int32_t year, month, day;
    
    std::string ToString();
    bool ShowCalendarPopup(const char* popupId);
    
    static int32_t DaysInMonth(int year, int month);
    static void RangeSelectorUI(Date& startDate, Date& endDate);
  };
  
} // namespace KanVasX
