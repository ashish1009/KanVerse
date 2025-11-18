//
//  UI_MovingAverage.cpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#include "UI_MovingAverage.hpp"

#include "Analyzer/Indicator/MovingAverage.hpp"

namespace KanVest
{
  namespace Utils
  {
    std::string FormatDoubleToString(double value)
    {
      char buf[32];
      std::snprintf(buf, sizeof(buf), "%.2f", value);
      return std::string(buf);
    }
  }
  
  void UI_MovingAverage::ShowMovingAverageData(const StockData& stockData, const std::unordered_map<int, double>& maMap, const std::string& maString)
  {
    int bullish = 0;
    int bearish = 0;
    for (const auto& [period, ma] : maMap)
    {
      if (ma > stockData.livePrice)
        bearish ++;
      else if (ma < stockData.livePrice and ma != 0)
        bullish++;
    }
    int total = bullish + bearish;
    
    if (bullish >= bearish)
    {
      std::string smaSummary = stockData.shortName + " is trading above " + std::to_string(bullish) + " out of " + std::to_string(total) + maString + "s";
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Regular), smaSummary, KanVasX::UI::AlignX::Left, {0, 0.0f}, KanVasX::Color::Cyan);
    }
    else
    {
      std::string smaSummary = stockData.shortName + " is trading below " + std::to_string(bearish) + " out of " + std::to_string(total) + maString + "s";
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Regular), smaSummary, KanVasX::UI::AlignX::Left, {0, 0.0f}, KanVasX::Color::Red);
    }
    
    {
      KanVasX::ScopedColor frameBg(ImGuiCol_FrameBg, KanVasX::Color::Alpha(KanVasX::Color::DarkRed, 0.7));
      KanVasX::ScopedColor plotColor(ImGuiCol_PlotHistogram, KanVasX::Color::Alpha(KanVasX::Color::Cyan, 0.7));
      
      float fraction = (float)bullish / (float)total;
      ImGui::ProgressBar(fraction, ImVec2(-1, 0), "");
    }
    
    std::string smaSummary = "If current price is greater than SMA, trend is bullish";
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Small), smaSummary, KanVasX::UI::AlignX::Left, {0, 0.0f}, KanVasX::Color::White);
    
    auto ShowSma = [stockData](int period, double sma) {
      if (sma > 0)
      {
        std::string datString = std::to_string(period) + "d";
        KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Regular), datString, KanVasX::UI::AlignX::Left, {0, 0.0f}, KanVasX::Color::White);
        ImGui::SameLine();
        auto smaColor = sma > stockData.livePrice ? KanVasX::Color::Red : KanVasX::Color::Cyan;
        KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Regular), Utils::FormatDoubleToString(sma), KanVasX::UI::AlignX::Right, {0, 0.0f}, smaColor);
      }
    };
    
    int idx = 0;
    bool allChildEnded = true;
    ImVec2 smaChildSize = {(ImGui::GetContentRegionAvail().x / 2) - 6.0f, 100.0f};
    for (const auto& [period, ma] : maMap)
    {
      if (idx % 4 == 0)
      {
        ImGui::BeginChild(std::to_string(period).c_str(), smaChildSize, true);
        allChildEnded = false;
      }
      
      ShowSma(period, ma);
      
      if (idx % 4 == 3)
      {
        ImGui::EndChild();
        ImGui::SameLine();
        allChildEnded = true;
      }
      idx++;
    }
    ImGui::NewLine();
    
    if (!allChildEnded)
    {
      ImGui::EndChild();
    }
  }
    
  void UI_MovingAverage::ShowSMA(const StockData& stockData, const std::string& range)
  {
    const auto& maData = MovingAverages::Compute(stockData, range);
    ShowMovingAverageData(stockData, maData.smaValues, " SMA");
  }
  void UI_MovingAverage::ShowEMA(const StockData& stockData, const std::string& range)
  {
    const auto& maData = MovingAverages::Compute(stockData, range);
    ShowMovingAverageData(stockData, maData.emaValues, " EMA");
  }
} // namespace KanVest
