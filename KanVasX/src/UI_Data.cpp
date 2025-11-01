//
//  UI_Data.cpp
//  KanVasX
//
//  Created by Ashish . on 01/11/25.
//

#include "UI_Data.hpp"

namespace KanVasX
{
  std::string Date::ToString()
  {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << day << "-" << std::setw(2) << month << "-" << std::setw(4) << year;
    return oss.str();
  }
  
  int32_t Date::DaysInMonth(int32_t year, int32_t month)
  {
    static const int32_t days[]{31,28,31,30,31,30,31,31,30,31,30,31};
    int32_t d = days[month - 1];
    bool leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    if (month == 2 && leap)
    {
      d = 29;
    }
    return d;
  }
  
  bool Date::ShowCalendarPopup(const char* popupId)
  {
    bool changed = false;
    
    if (ImGui::BeginPopup(popupId))
    {
      ImGui::TextUnformatted("Select Date");
      ImGui::Separator();
      
      const float cellWidth = 28.0f;   // fixed width for each day column
      const float cellHeight = 26.0f;
      const float calendarWidth = 7 * (cellWidth + ImGui::GetStyle().ItemSpacing.x);
      
      // --- Month / Year Selectors with Arrow Controls ---
      ImGui::PushItemWidth(100);
      
      static const char* months[] = {
        "Jan","Feb","Mar","Apr","May","Jun",
        "Jul","Aug","Sep","Oct","Nov","Dec"
      };
      
      // --- Row: Navigation Buttons + Month / Year Selection ---
      ImGui::BeginGroup();
      {
        // Year decrement
        if (ImGui::ArrowButton("##PrevYear", ImGuiDir_Left))
        {
          year--;
          day = std::min(day, DaysInMonth(year, month));
          changed = true;
        }
        ImGui::SameLine();
        
        // Month decrement
        if (ImGui::ArrowButton("##PrevMonth", ImGuiDir_Left))
        {
          month--;
          if (month < 1)
          {
            month = 12;
            year--;
          }
          day = std::min(day, DaysInMonth(year, month));
          changed = true;
        }
        
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90);
        int monthIndex = month - 1;
        if (ImGui::Combo("##MonthCombo", &monthIndex, months, IM_ARRAYSIZE(months)))
        {
          month = monthIndex + 1;
          day = std::min(day, DaysInMonth(year, month));
          changed = true;
        }
        
        ImGui::SameLine();
        static int minYear = 2000, maxYear = 2030;
        int y = year;
        ImGui::SetNextItemWidth(70);
        if (ImGui::DragInt("##YearDrag", &y, 1, minYear, maxYear, "%d"))
        {
          year = std::clamp(y, minYear, maxYear);
          day = std::min(day, DaysInMonth(year, month));
          changed = true;
        }
        
        ImGui::SameLine();
        
        // Month increment
        if (ImGui::ArrowButton("##NextMonth", ImGuiDir_Right))
        {
          month++;
          if (month > 12)
          {
            month = 1;
            year++;
          }
          day = std::min(day, DaysInMonth(year, month));
          changed = true;
        }
        
        ImGui::SameLine();
        
        // Year increment
        if (ImGui::ArrowButton("##NextYear", ImGuiDir_Right))
        {
          year++;
          day = std::min(day, DaysInMonth(year, month));
          changed = true;
        }
      }
      ImGui::EndGroup();
      
      ImGui::PopItemWidth();
      ImGui::Separator();
      
      // --- Week Headers (Aligned) ---
      const char* days[] = {"Su","Mo","Tu","We","Th","Fr","Sa"};
      ImGui::BeginChild("CalendarGrid", ImVec2(calendarWidth, 6 * (cellHeight + 4) + 40), false);
      
      for (int i = 0; i < 7; i++)
      {
        ImGui::SetCursorPosX(i * (cellWidth + ImGui::GetStyle().ItemSpacing.x));
        ImGui::TextUnformatted(days[i]);
        if (i < 6) ImGui::SameLine();
      }
      
      // --- Calculate first day of the month ---
      std::tm time_in = {};
      time_in.tm_year = year - 1900;
      time_in.tm_mon = month - 1;
      time_in.tm_mday = 1;
      std::mktime(&time_in);
      int firstDayOfWeek = time_in.tm_wday;
      
      int daysCount = DaysInMonth(year, month);
      ImGui::Dummy(ImVec2(0, 5));
      
      // --- Draw Calendar Grid ---
      int d = 1;
      for (int row = 0; row < 6 && d <= daysCount; ++row)
      {
        for (int col = 0; col < 7; ++col)
        {
          if (row == 0 && col < firstDayOfWeek)
          {
            ImGui::SetCursorPosX(col * (cellWidth + ImGui::GetStyle().ItemSpacing.x));
            ImGui::Dummy(ImVec2(cellWidth, cellHeight));
          }
          else if (d <= daysCount)
          {
            ImGui::SetCursorPosX(col * (cellWidth + ImGui::GetStyle().ItemSpacing.x));
            char buf[4];
            std::snprintf(buf, sizeof(buf), "%d", d);
            bool isSelected = (d == day);
            if (ImGui::Selectable(buf, isSelected, 0, ImVec2(cellWidth, cellHeight)))
            {
              day = d;
              changed = true;
              ImGui::CloseCurrentPopup();
            }
            d++;
          }
          if (col < 6) ImGui::SameLine();
        }
      }
      
      ImGui::EndChild();
      ImGui::EndPopup();
    }
    
    return changed;
  }


  
  // --- Main range selector ---
  void Date::RangeSelectorUI(Date& startDate, Date& endDate)
  {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));
    
    // --- Start Date ---
    ImGui::TextUnformatted("Start:");
    ImGui::SameLine();
    
    if (ImGui::Button(startDate.ToString().c_str(), ImVec2(110, 0)))
    {
      ImGui::OpenPopup("StartDatePopup");
    }
    
    startDate.ShowCalendarPopup("StartDatePopup");
    
    ImGui::SameLine(0, 20.0f);
    
    // --- End Date ---
    ImGui::TextUnformatted("End:");
    ImGui::SameLine();
    
    if (ImGui::Button(endDate.ToString().c_str(), ImVec2(110, 0)))
    {
      ImGui::OpenPopup("EndDatePopup");
    }
    
    endDate.ShowCalendarPopup("EndDatePopup");
    
    ImGui::PopStyleVar();
    
    // --- Range validation ---
    if (endDate.year < startDate.year ||
        (endDate.year == startDate.year && endDate.month < startDate.month) ||
        (endDate.year == startDate.year && endDate.month == startDate.month && endDate.day < startDate.day))
    {
      ImGui::Spacing();
      ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "⚠️ Invalid range (End before Start)");
    }
  }

} // namespace KanVasX
