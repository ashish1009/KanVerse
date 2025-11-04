//
//  UI_Popup.cpp
//  KanVasX
//
//  Created by Ashish . on 04/11/25.
//

#include "UI_Popup.hpp"

namespace KanVasX
{
  Popup::Popup (std::string_view title)
  : m_title(title)
  {
    
  }
  
  void Popup::Set(std::string_view title, bool openFlag, float width, float height, bool center)
  {
    m_title = title;
    m_openFlag = openFlag;
    m_width = width;
    m_height = height;
    m_center = center;
  }
  
  void Popup::SetFlag(bool flag)
  {
    m_openFlag = flag;
  }
  
  float Popup::GetWidgth() const
  {
    return m_width;
  }
  float Popup::GetHeight() const
  {
    return m_height;
  }

} // namespace KanVasX
