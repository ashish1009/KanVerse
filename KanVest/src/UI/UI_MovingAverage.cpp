//
//  UI_MovingAverage.cpp
//  KanVest
//
//  Created by Ashish . on 14/12/25.
//

#include "UI_MovingAverage.hpp"

#include "UI/UI_Utils.hpp"

#include "Analyzer/StockAnalyzer.hpp"

namespace KanVest
{
#define Font(font) KanVest::UI::Font::Get(KanVest::UI::FontType::font)
  
  using Align = KanVasX::UI::AlignX;
  using Color = KanVasX::Color;

  void UI_MovingAverage::ShowDMA(const StockData& stockData, ImTextureID shadowTexture)
  {
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(Font(Header_24), "No data for stock", Align::Left, {10.0f, 0.0f}, Color::Error);
      return;
    }
    const auto& dmaData = Analyzer::GetDMA();
    ShowMovingAverageData(stockData, dmaData, " SMA", shadowTexture);

  }
  void UI_MovingAverage::ShowEMA(const StockData& stockData, ImTextureID shadowTexture)
  {
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(Font(Header_24), "No data for stock", Align::Left, {10.0f, 0.0f}, Color::Error);
      return;
    }
    const auto& emaData = Analyzer::GetEMA();
    ShowMovingAverageData(stockData, emaData, " EMA", shadowTexture);
  }
  
  void UI_MovingAverage::ShowMovingAverageData(const StockData& stockData, const std::map<int, double>& maMap, const std::string& maString, ImTextureID shadowTexture)
  {
    for (const auto& [period, ma] : maMap)
    {
      ImGui::Text("%d, %f", period, ma);
    }
  }
} // namespace KanVest
