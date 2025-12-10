//
//  UI_Chart.cpp
//  KanVest
//
//  Created by Ashish . on 09/12/25.
//

#include "UI_Chart.hpp"

#include "Stock/StockUtils.hpp"

namespace KanVest
{
  void Chart::Show(const StockData &stockData)
  {
    IK_PERFORMANCE_FUNC("Chart::Show");
    
    // Check if stock data is valid
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_24), "No Chart Available !!", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Error);
      return;
    }
    
    // Get stock's candle history data
    const auto& candleHistory = stockData.candleHistory;
    
    // Check if candle history is empty
    if (candleHistory.empty())
    {
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_24), "No Candle Data Available !!", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Error);
      return;
    }
    
    // Keep only trading days (weekdays)
    std::vector<CandleData> filteredDaysCandles = Utils::FilterTradingDays(candleHistory);
    
    // We'll use sequential x indices so weekends/holidays are skipped visually.
    std::vector<double> xs, opens, highs, lows, closes;
    xs.reserve(filteredDaysCandles.size());
    opens.reserve(filteredDaysCandles.size());
    highs.reserve(filteredDaysCandles.size());
    lows.reserve(filteredDaysCandles.size());
    closes.reserve(filteredDaysCandles.size());
    
    double ymin = DBL_MAX;
    double ymax = -DBL_MAX;
    
    for (size_t i = 0; i < filteredDaysCandles.size(); ++i)
    {
      const auto& candle = filteredDaysCandles[i];
      xs.push_back(static_cast<double>(i)); // sequential index
      opens.push_back(candle.open);
      highs.push_back(candle.high);
      lows.push_back(candle.low);
      closes.push_back(candle.close);
      ymin = std::min(ymin, candle.low);
      ymax = std::max(ymax, candle.high);
    }
    
    // Build human-readable labels locally (e.g., "2025-11-03").
    // To avoid too many labels, we'll place a label every few points.
    std::vector<std::string> labelStrings;
    std::vector<const char*> labelPtrs;
    std::vector<double> labelPositions;

    // Decide label step based on number of points: aim for ~8-12 labels max.
    int targetLabels = 10;
    size_t n = filteredDaysCandles.size();
    size_t labelStep = 1;
    if (n > 0)
    {
      labelStep = std::max<size_t>(1, (n + targetLabels - 1) / targetLabels);
    }
    
    labelStrings.reserve((n + labelStep - 1) / labelStep);
    labelPositions.reserve(labelStrings.capacity());
    labelPtrs.reserve(labelStrings.capacity());

    // Lable string
    for (size_t i = 0; i < n; i += labelStep)
    {
      // Convert timestamp to UTC date string
      time_t t = static_cast<time_t>(filteredDaysCandles[i].timestamp);
      struct tm tm{};
      
      gmtime_r(&t, &tm);
      
      char buf[64];
      // Format: YYYY-MM-DD (change format if you want time too)
      std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
      labelStrings.emplace_back(buf);
      labelPositions.push_back(static_cast<double>(i));
    }

    // Convert label strings to char* for ImPlot
    for (auto &s : labelStrings)
    {
      labelPtrs.push_back(s.c_str());
    }

    // Start plotting
    KanVasX::ScopedColor ChartBg(ImGuiCol_WindowBg, KanVasX::Color::Background);
    KanVasX::ScopedColor ChartBorderBg(ImGuiCol_FrameBg, KanVasX::Color::Background);
    
    static const auto ChartFlag =  ImPlotFlags_NoFrame | ImPlotFlags_NoMenus;
    if (ImPlot::BeginPlot("##StockPlot", ImVec2(ImGui::GetContentRegionAvail().x - 1.0f, 500.0f), ChartFlag))
    {
      // We use AutoFit for X and set Y limits explicitly
      ImPlot::SetupAxes("", "", ImPlotAxisFlags_NoDecorations | ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
      ImPlot::SetupAxisLimits(ImAxis_Y1, ymin - 1.0, ymax + 1.0, ImGuiCond_Always);

      ImPlot::EndPlot();
    }
  }
} // namespace KanVest
