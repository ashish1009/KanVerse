//
//  UI_Momentum.cpp
//  KanVest
//
//  Created by Ashish . on 19/11/25.
//

#include "UI_Momentum.hpp"

#include "UI/UI_Utils.hpp"

#include "Analyzer/Indicator/Momentum.hpp"

namespace KanVest
{
  struct RSI_UI
  {
    double value = 0.0;                     // latest RSI
    std::string state;                      // Oversold / Neutral / Overbought
    std::string trend;                      // Rising / Falling / Flat
    std::vector<std::string> signals;       // textual signals
    std::string interpretation;             // human-friendly message
    ImU32 color;                            // UI color (green/yellow/red)
    std::vector<double> series;             // full RSI series
  };

  static bool IsValid(double v)
  {
    return !std::isnan(v) && !std::isinf(v);
  };
  
  static RSI_UI BuildRSI_UI(const RSISeries& rsiData)
  {
    RSI_UI ui;
    ui.series = rsiData.series;
    
    if (!IsValid(rsiData.last))
    {
      ui.value = 0.0;
      ui.state = "Not enough data";
      ui.color = KanVasX::Color::TextMuted;
      ui.trend = "Flat";
      ui.interpretation = "Insufficient history to compute RSI.";
      return ui;
    }
    
    ui.value = rsiData.last;
    
    // --- Determine State ---
    if (ui.value < 30)
    {
      ui.state = "Oversold";
      ui.color = KanVasX::Color::Cyan;
    }
    else if (ui.value > 70)
    {
      ui.state = "Overbought";
      ui.color = KanVasX::Color::Red;
    }
    else
    {
      ui.state = "Neutral";
      ui.color = KanVasX::Color::Yellow;
    }
    
    // --- Determine Trend ---
    int lastIndex = (int)ui.series.size() - 1;
    double prev = ui.series[lastIndex - 1];
    
    if (!IsValid(prev))
      ui.trend = "Flat";
    else if (ui.value > prev)
      ui.trend = "Rising";
    else if (ui.value < prev)
      ui.trend = "Falling";
    else
      ui.trend = "Flat";
    
    // --- Generate Signals ---
    if (ui.state == "Oversold")
    {
      ui.signals.push_back("Stock in oversold zone");
    }
    else if (ui.state == "Overbought")
    {
      ui.signals.push_back("Stock in overbought zone");
    }
    
    if (ui.trend == "Rising")
      ui.signals.push_back("Momentum improving");
    else if (ui.trend == "Falling")
      ui.signals.push_back("Momentum weakening");
    
    // --- Interpretation ---
    if (ui.state == "Oversold")
    {
      ui.interpretation =
      "RSI indicates an oversold region. Buying interest may come soon.";
    }
    else if (ui.state == "Overbought")
    {
      ui.interpretation =
      "RSI indicates an overbought region. A correction is possible.";
    }
    else
    {
      ui.interpretation =
      "RSI is neutral. No extreme momentum visible.";
    }
    
    return ui;
  };

  void UI_Momentum::ShowRSI(const StockData& stockData)
  {
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_24), "No data for stock", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Error);
      return;
    }

    const RSISeries& rsiData = RSI::Compute(stockData);
    
    const auto& RSI_UI_Data = BuildRSI_UI(rsiData);
    {
      KanVasX::ScopedColor rsiColor(ImGuiCol_PlotLines, RSI_UI_Data.color);
      
      std::string rsiString = KanVest::UI::Utils::FormatDoubleToString(RSI_UI_Data.value);
      rsiString += " : " + RSI_UI_Data.state;
      rsiString += " : " + RSI_UI_Data.trend + " Trend";
      
      KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_22), rsiString, KanVasX::UI::AlignX::Center, {0, 0}, RSI_UI_Data.color);
      
      if (RSI_UI_Data.series.empty())
      {
        ImGui::Text("RSI data unavailable.");
        return;
      }
      
      std::vector<float> rsiFloat;
      rsiFloat.reserve(RSI_UI_Data.series.size());
      for (double v : RSI_UI_Data.series)
        rsiFloat.push_back(static_cast<float>(v));
      
      float rsiMin = 100.0f, rsiMax = 0.0f;
      for (float v : rsiFloat)
      {
        rsiMin = std::min(rsiMin, v);
        rsiMax = std::max(rsiMax, v);
      }
      
      // --- RSI Chart ---
      ImGui::PushItemWidth(-1);
      ImGui::PlotLines(
                       "##RSIPlot",
                       rsiFloat.data(),
                       (int)rsiFloat.size(),
                       0,
                       nullptr,
                       0.0f,
                       100.0f,
                       ImVec2(0, std::min(200.0f, ImGui::GetContentRegionAvail().y))
                       );
    }
  }
} // namespace KanVest
