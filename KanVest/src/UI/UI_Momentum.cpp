//
//  UI_Momentum.cpp
//  KanVest
//
//  Created by Ashish . on 15/01/26.
//

#include "UI_Momentum.hpp"

#include "UI/UI_Utils.hpp"

#include "Analyzer/Indicators/MovingAverage.hpp"

#include "Analyzer/StockAnalyzer.hpp"

namespace KanVest
{
#define Font(font) KanVest::UI::Font::Get(KanVest::UI::FontType::font)
  
  using Align = KanVasX::UI::AlignX;
  using Color = KanVasX::Color;
  
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

    // Latest RSI
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
      "RSI is Neutral. No extreme momentum visible.";
    }
    
    return ui;
  }
  
  void UI_Momentum::ShowRSI(const StockData& stockData)
  {
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(Font(Header_24), "No data for stock", Align::Left, {10.0f, 0.0f}, Color::Error);
      return;
    }

    const RSISeries& rsiData = Analyzer::GetRSI();
    const auto& RSI_UI_Data = BuildRSI_UI(rsiData);

    // RSI Value, State and Trend
    std::string rsiString = KanVest::UI::Utils::FormatDoubleToString(RSI_UI_Data.value);
    rsiString += " : " + RSI_UI_Data.state;
    rsiString += " : " + RSI_UI_Data.trend + " Trend";

    // Show RSI value and its data
    if (RSI_UI_Data.series.empty())
    {
      KanVasX::UI::Text(Font(Header_22), "RSI data unavailable", Align::Center, {0, 0}, Color::Error);
      return;
    }
    else
    {
      KanVasX::UI::Text(Font(Header_22), rsiString, Align::Center, {0, 0}, RSI_UI_Data.color);
      KanVasX::UI::Tooltip(RSI_UI_Data.interpretation);
    }

    // Make float RSI
    std::vector<float> rsiFloat;
    rsiFloat.reserve(RSI_UI_Data.series.size());
    for (double v : RSI_UI_Data.series)
    {
      rsiFloat.push_back(static_cast<float>(v));
    }

    // Get min and max RSI
    float rsiMin = 100.0f, rsiMax = 0.0f;
    for (float v : rsiFloat)
    {
      rsiMin = std::min(rsiMin, v);
      rsiMax = std::max(rsiMax, v);
    }

    if (ImPlot::BeginPlot("##RSIPlot", ImVec2(-1, 250)))
    {
      const auto& rsi = RSI_UI_Data.series;
      size_t n = rsi.size();
      
      // Build X array
      std::vector<double> x;
      x.reserve(n);
      
      std::vector<double> y;
      y.reserve(n);
      
      for (size_t i = 0; i < n; i++)
      {
        x.push_back((double)i);
        y.push_back(std::isnan(rsi[i]) ? NAN : rsi[i]);
      }
      
      // Setup axes BEFORE plotting
      ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_Lock);
      ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_Lock);

      ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);
      ImPlot::SetupAxisLimits(ImAxis_X1, 0, (double)(n - 1), ImGuiCond_Always);

      // ---- Plot RSI ----
      ImPlot::PushStyleColor(ImPlotCol_Line, RSI_UI_Data.color);
      ImPlot::PlotLine("RSI", x.data(), y.data(), (int)n);
      ImPlot::PopStyleColor();
      
      // ---- Horizontal lines ----
      ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 1.5f);
      
      double xs[2] = {0, (double)(n - 1)};
      
      // Line at 30
      {
        double ys[2] = {30.0, 30.0};
        KanVasX::ScopedColor color(ImPlotCol_Line, KanVasX::Color::DarkRed);
        ImPlot::PlotLine("30", xs, ys, 2);
      }
      
      // Line at 70
      {
        double ys[2] = {70.0, 70.0};
        KanVasX::ScopedColor color(ImPlotCol_Line, KanVasX::Color::Background);
        ImPlot::PlotLine("70", xs, ys, 2);
      }
      
      ImPlot::PopStyleVar();
      
      ImPlot::EndPlot();
    }
    
    const auto& analyzerResult = Analyzer::GetReport();
    for (const auto& [color, summary] : analyzerResult.summary.at(TechnicalIndicators::RSI))
    {
      KanVasX::UI::Text(Font(Regular), summary, Align::Left, {0, 0}, color);
    }
  }
} // namespace KanVest
