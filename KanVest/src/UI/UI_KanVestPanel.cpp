//
//  UI_KanVestPanel.cpp
//  KanVest
//
//  Created by Ashish . on 02/12/25.
//

#include "UI_KanVestPanel.hpp"

namespace KanVest::UI
{
  void Panel::Show()
  {
    IK_PERFORMANCE_FUNC("Panel::Show");
    
    KanVasX::Panel::Begin("Stock Analyzer");
    KanVasX::Panel::End(0.0f);
  }
} // namespace KanVest::UI
