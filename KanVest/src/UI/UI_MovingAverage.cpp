//
//  UI_MovingAverage.cpp
//  KanVest
//
//  Created by Ashish . on 23/11/25.
//

#include "UI_MovingAverage.hpp"

#include "UI/UI_Utils.hpp"

#include "Analyzer/Indicator/MovingAverage.hpp"

#include "Analyzer/StockAnalyzer.hpp"

namespace KanVest
{
  void UI_MovingAverage::ShowSMA(const StockData& stockData, ImTextureID shadowTexture)
  {
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_24), "No data for stock", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Error);
      return;
    }
    
    const auto& smaData = Analyzer::GetSMA();
    ShowMovingAverageData(stockData, smaData, " SMA", shadowTexture);
  }
  void UI_MovingAverage::ShowEMA(const StockData& stockData, ImTextureID shadowTexture)
  {
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_24), "No data for stock", KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Error);
      return;
    }
    
    const auto& emaData = Analyzer::GetEMA();
    ShowMovingAverageData(stockData, emaData, " EMA", shadowTexture);
  }
    
  void UI_MovingAverage::ShowMovingAverageData(const StockData& stockData, const std::map<int, double>& maMap, const std::string& maString, ImTextureID shadowTexture)
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
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Regular), smaSummary, KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Cyan);
    }
    else
    {
      std::string smaSummary = stockData.shortName + " is trading below " + std::to_string(bearish) + " out of " + std::to_string(total) + maString + "s";
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Regular), smaSummary, KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, KanVasX::Color::Red);
    }
    
    KanVasX::UI::SameLine();
    std::string smaSummary = "If current price is greater than SMA, trend is bullish";
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Small), smaSummary, KanVasX::UI::AlignX::Right, {-5.0f, 0.0f}, KanVasX::Color::White);

    {
      KanVasX::ScopedColor frameBg(ImGuiCol_FrameBg, KanVasX::Color::Alpha(KanVasX::Color::DarkRed, 0.7));
      KanVasX::ScopedColor plotColor(ImGuiCol_PlotHistogram, KanVasX::Color::Alpha(KanVasX::Color::Cyan, 0.7));
      
      float fraction = (float)bullish / (float)total;
      ImGui::ProgressBar(fraction, ImVec2(ImGui::GetContentRegionAvail().x, 0), " ");
    }
        
    auto ShowSma = [stockData](int period, double sma) {
      if (sma > 0)
      {
        std::string datString = std::to_string(period) + "d";
        KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Regular), datString, KanVasX::UI::AlignX::Left, {20.0f, 5.0f}, KanVasX::Color::White);
        ImGui::SameLine();
        auto smaColor = sma > stockData.livePrice ? KanVasX::Color::Red : KanVasX::Color::Cyan;
        KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Regular),
                          KanVest::UI::Utils::FormatDoubleToString(sma), KanVasX::UI::AlignX::Right, {-20.0f, 0.0f}, smaColor);
      }
    };

    ImVec2 smaChildSize = {(ImGui::GetContentRegionAvail().x / 2), 120.0f};

    int idx = 0;
    bool allChildEnded = true;

    KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, KanVasX::Color::BackgroundLight);
    for (const auto& [period, ma] : maMap)
    {
      if (idx % 4 == 0)
      {
        ImGui::BeginChild(std::to_string(period).c_str(), smaChildSize, true);
        KanVasX::UI::ShiftCursorY(5.0f);
        allChildEnded = false;
      }

      ShowSma(period, ma);
      
      if (idx % 4 == 3)
      {
        if (shadowTexture)
        {
          KanVasX::UI::DrawShadowAllDirection(shadowTexture);
        }
        ImGui::EndChild();
        ImGui::SameLine();
        allChildEnded = true;
      }
      idx++;
    }
    ImGui::NewLine();
    
    if (!allChildEnded)
    {
      if (shadowTexture)
      {
        KanVasX::UI::DrawShadowAllDirection(shadowTexture);
      }
      ImGui::EndChild();
    }
  }
} // namespace KanVest
