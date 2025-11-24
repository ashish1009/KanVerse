//
//  UI_MACD.cpp
//  KanVest
//
//  Created by Ashish . on 24/11/25.
//

#include "UI_MACD.hpp"

#include "UI/UI_Utils.hpp"

#include "Analyzer/Indicator/MACD.hpp"

#include "Analyzer/StockAnalyzer.hpp"

namespace KanVest
{
  static bool IsValid(double v)
  {
    return !std::isnan(v) && !std::isinf(v);
  };

  struct MACD_UI
  {
    double macd = 0.0;
    double signal = 0.0;
    double hist = 0.0;
    
    std::string state;               // Bullish, Bearish, Neutral
    std::string trend;               // Rising / Falling / Flat
    std::vector<std::string> signals;
    std::string interpretation;
    
    ImU32 color;                     // Main UI color (green/red/yellow)
    
    std::vector<double> macdSeries;
    std::vector<double> signalSeries;
    std::vector<double> histSeries;
  };

  static MACD_UI BuildMACD_UI(const MACDResult& macdData)
  {
    MACD_UI ui;
    
    if (macdData.macdLine.empty() || macdData.signalLine.empty() || macdData.histogram.empty())
    {
      ui.state = "Not enough data";
      ui.color = KanVasX::Color::TextMuted;
      ui.trend = "Flat";
      ui.interpretation = "Insufficient history to compute MACD.";
      return ui;
    }
    
    // Copy series for chart
    ui.macdSeries   = macdData.macdLine;
    ui.signalSeries = macdData.signalLine;
    ui.histSeries   = macdData.histogram;
    
    // Latest values
    ui.macd   = macdData.macdLine.back();
    ui.signal = macdData.signalLine.back();
    ui.hist   = macdData.histogram.back();
    
    // --- Determine MACD trend (Bullish if MACD > Signal) ---
    if (ui.macd > ui.signal)
    {
      ui.state = "Bullish Crossover";
      ui.color = KanVasX::Color::Cyan;   // same as oversold in RSI style
    }
    else if (ui.macd < ui.signal)
    {
      ui.state = "Bearish Crossover";
      ui.color = KanVasX::Color::Red;
    }
    else
    {
      ui.state = "Neutral";
      ui.color = KanVasX::Color::Yellow;
    }
    
    // --- Trend direction (same logic as RSI rising/falling) ---
    if (ui.macdSeries.size() > 1)
    {
      double last  = ui.macdSeries.back();
      double prev  = ui.macdSeries[ui.macdSeries.size() - 2];
      
      if (last > prev)
        ui.trend = "Rising";
      else if (last < prev)
        ui.trend = "Falling";
      else
        ui.trend = "Flat";
    }
    
    // --- Signals ---
    if (ui.macd > ui.signal)
      ui.signals.push_back("MACD above Signal → Bullish momentum");
    else
      ui.signals.push_back("MACD below Signal → Bearish momentum");
    
    if (ui.hist > 0)
      ui.signals.push_back("Histogram positive → Upward strength");
    else
      ui.signals.push_back("Histogram negative → Downward pressure");
    
    if (ui.trend == "Rising")
      ui.signals.push_back("Momentum is rising");
    else if (ui.trend == "Falling")
      ui.signals.push_back("Momentum weakening");
    
    // --- Interpretation (angel/zerodha style) ---
    if (ui.state == "Bullish Crossover")
      ui.interpretation = "MACD is in bullish crossover. Trend may continue upward.";
    else if (ui.state == "Bearish Crossover")
      ui.interpretation = "MACD is in bearish crossover. Selling pressure increasing.";
    else
      ui.interpretation = "MACD neutral. No strong momentum shift.";
    
    return ui;
    
  };

  void UI_MACD::Show(const StockData& stockData)
  {
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_24), "No data for stock", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Error);
      return;
    }
    
    const auto& macdData = Analyzer::GetMACD();
    MACD_UI ui = BuildMACD_UI(macdData);
    
    std::string macdString = "MACD  : " + KanVest::UI::Utils::FormatDoubleToString(ui.macd);
    KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_22), macdString, KanVasX::UI::AlignX::Left, {0, 0}, ui.color);

    // Title like RSI (centered)
    std::string title = ui.state + " : " + ui.trend + " Trend";

    ImGui::SameLine();
    KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_22), title, KanVasX::UI::AlignX::Center, {-50, 0}, ui.color);
    KanVasX::UI::Tooltip(ui.interpretation);

    std::string macdSignalString = "Signal : " + KanVest::UI::Utils::FormatDoubleToString(ui.signal);
    ImGui::SameLine();
    KanVasX::UI::Text(UI::Font::Get(UI::FontType::Header_22), macdSignalString, KanVasX::UI::AlignX::Right, {0, 0}, ui.color);
    
    // If no data
    if (ui.macdSeries.empty())
    {
      ImGui::Text("MACD data unavailable.");
      return;
    }
    
    // Convert to floats for chart
    std::vector<float> macdF, sigF, histF;
    macdF.reserve(ui.macdSeries.size());
    sigF.reserve(ui.signalSeries.size());
    histF.reserve(ui.histSeries.size());
    
    for (double v : ui.macdSeries)   macdF.push_back((float)v);
    for (double v : ui.signalSeries) sigF.push_back((float)v);
    for (double v : ui.histSeries)   histF.push_back((float)v);
        
    {
      if (ImPlot::BeginPlot("##MACDPlot", ImVec2(-1, 250),
                            ImPlotFlags_NoTitle | ImPlotFlags_NoMenus))
      {
        // --- Setup Axes ---
        ImPlot::SetupAxes("", "", ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, macdF.size(), ImGuiCond_Always);
        
        // --- Auto-fit Y Range ---
        auto [minMACD, maxMACD] = std::minmax_element(macdF.begin(), macdF.end());
        auto [minSIG , maxSIG]  = std::minmax_element(sigF.begin(), sigF.end());
        auto [minH   , maxH]    = std::minmax_element(histF.begin(), histF.end());
        
        float yMin = std::min({*minMACD, *minSIG, *minH});
        float yMax = std::max({*maxMACD, *maxSIG, *maxH});
        ImPlot::SetupAxisLimits(ImAxis_Y1, yMin, yMax, ImGuiCond_Always);
        
        // --- X axis data ---
        static std::vector<float> xs;
        xs.resize(macdF.size());
        for (int i = 0; i < xs.size(); ++i)
          xs[i] = (float)i;
        
        // --- Zero horizontal line ---
        {
          ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(150,150,150,255));
          float xline[2] = { 0, (float)macdF.size() };
          float yline[2] = { 0, 0 };
          ImPlot::PlotLine("Zero", xline, yline, 2);
          ImPlot::PopStyleColor();
        }
        
        // --- Histogram bars ---
        for (int i = 0; i < histF.size(); i++) {
          ImPlot::SetNextLineStyle(
                                   histF[i] >= 0 ? ImVec4(0.1f, 0.9f, 0.1f, 1.0f)  // green
                                   : ImVec4(0.9f, 0.1f, 0.1f, 1.0f), // red
                                   3.0f
                                   );
          float x_bar[1] = { (float)i };
          float h_val[1] = { histF[i] };
          ImPlot::PlotBars("Histogram", x_bar, h_val, 1, 0.7f);
        }
        
        // --- MACD Line ---
        ImPlot::SetNextLineStyle(ImVec4(0.1f, 0.9f, 1.0f, 1.0f), 2.0f);
        ImPlot::PlotLine("MACD", xs.data(), macdF.data(), (int)macdF.size());
        
        // --- Signal Line ---
        ImPlot::SetNextLineStyle(ImVec4(1.0f, 0.9f, 0.0f, 1.0f), 2.0f);
        ImPlot::PlotLine("Signal", xs.data(), sigF.data(), (int)sigF.size());

        ImPlot::EndPlot();
      }
    }
  }
} // namespace KanVest
