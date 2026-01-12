//
//  UI_Chart.cpp
//  KanVest
//
//  Created by Ashish . on 12/01/26.
//

#include "UI_Chart.hpp"

namespace KanVest
{
#define Font(font) KanVest::UI::Font::Get(KanVest::UI::FontType::font)
  
  using Align = KanVasX::UI::AlignX;
  using Color = KanVasX::Color;
  
  void Chart::Show(const StockData &stockData)
  {
    IK_PERFORMANCE_FUNC("Chart::Show");
    
    if (!stockData.IsValid())
    {
      return;
    }
    
    ShowController(stockData);
  }
  
  void Chart::ShowController(const StockData& stockData)
  {
    if (!stockData.IsValid())
    {
      return;
    }
    
    // Range controller
    {
      for (const auto& range : API_Provider::GetValidRangesString())
      {
        auto buttonColor = range == stockData.range ? KanVasX::Color::BackgroundLight : KanVasX::Color::BackgroundDark;
        std::string uniqueLabel = range + "##Range";
        
        if (KanVasX::UI::DrawButton(uniqueLabel, nullptr, buttonColor))
        {

        }
        ImGui::SameLine();
      }
    } // Range scope

  }
} // namespace KanVest
