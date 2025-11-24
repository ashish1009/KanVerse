//
//  UI_Momentum.cpp
//  KanVest
//
//  Created by Ashish . on 24/11/25.
//

#include "UI_Momentum.hpp"

#include "UI/UI_Utils.hpp"

#include "Analyzer/Indicator/Momentum.hpp"

#include "Analyzer/StockAnalyzer.hpp"

namespace KanVest
{
  static bool IsValid(double v)
  {
    return !std::isnan(v) && !std::isinf(v);
  };

  struct RSI_UI
  {
    double value = 0.0;                     // latest RSI
    std::string state;                      // Oversold / Neutral / Overbought
    std::string trend;                      // Rising / Falling / Flat
    std::vector<std::string> signals;       // textual signals
    std::string interpretation;             // human-friendly message
    ImU32 color = KanVasX::Color::Text;     // UI color (green/yellow/red)
    std::vector<double> series;             // full RSI series
  };

  static RSI_UI BuildRSI_UI(const RSISeries& rsiData)
  {
    RSI_UI ui;
    return ui;
  }
  
  void UI_Momentum::ShowRSI(const StockData& stockData, ImTextureID shadowTexture)
  {
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_24), "No data for stock", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Error);
      return;
    }
    
    const RSISeries& rsiData = Analyzer::GetRSI();
    const auto& RSI_UI_Data = BuildRSI_UI(rsiData);

    {
      KanVasX::ScopedColor rsiColor(ImGuiCol_PlotLines, RSI_UI_Data.color);
      
      std::string rsiString = KanVest::UI::Utils::FormatDoubleToString(RSI_UI_Data.value);
      rsiString += " : " + RSI_UI_Data.state;
      rsiString += " : " + RSI_UI_Data.trend + " Trend";
      
      KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_22), rsiString, KanVasX::UI::AlignX::Center, {0, 0}, RSI_UI_Data.color);
      
      if (RSI_UI_Data.series.empty())
      {
        KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_22), "RSI data unavailable", KanVasX::UI::AlignX::Center, {0, 0}, KanVasX::Color::Error);
        return;
      }
    }
  }
} // namespace KanVest
