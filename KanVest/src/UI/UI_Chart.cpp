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
    
    // Button Setting
    static constexpr ImVec2 buttonSize = {50, 30};
    static constexpr float buttonRounding = 10.0f;
    
    // Range controller
    for (const auto& range : API_Provider::GetValidRangesString())
    {
      auto buttonColor = range == stockData.range ? KanVasX::Color::Button : KanVasX::Color::BackgroundDark;
      auto textColor = range == stockData.range ? KanVasX::Color::Text : KanVasX::Color::TextMuted;

      std::string uniqueLabel = range + "##Range";
      if (KanVasX::UI::DrawButton(uniqueLabel, nullptr, buttonColor, textColor, false, buttonRounding, buttonSize))
      {
        Range currentRange = API_Provider::GetRangeEnumFromString(range);
        Interval optimalInterval = API_Provider::GetOptimalIntervalForRange(currentRange);
        
        StockManager::AddStockDataRequest(stockData.symbol, currentRange, optimalInterval);
      }
      ImGui::SameLine();
    }

    // Interval Controller
    ImGui::SameLine();

    const auto& possibleIntervals = API_Provider::GetValidIntervalsStringForRangeString(stockData.range);
    KanVasX::UI::ShiftCursorX(ImGui::GetContentRegionAvail().x - possibleIntervals.size() * 60.0f);

    for (const auto& interval : possibleIntervals)
    {
      auto buttonColor = interval == stockData.dataGranularity ? KanVasX::Color::BackgroundLight : KanVasX::Color::BackgroundDark;
      auto textColor = interval == stockData.dataGranularity ? KanVasX::Color::Text : KanVasX::Color::TextMuted;

      std::string uniqueLabel = interval + "##Interval";
      if (KanVasX::UI::DrawButton(uniqueLabel, nullptr, buttonColor, textColor, false, buttonRounding, buttonSize))
      {
        Range currentRange = API_Provider::GetRangeEnumFromString(stockData.range);
        Interval currentInterval = API_Provider::GetIntervalEnumFromString(interval);

        StockManager::AddStockDataRequest(stockData.symbol, currentRange, currentInterval);
      }
      ImGui::SameLine();
    }
    ImGui::NewLine();

  }
} // namespace KanVest
