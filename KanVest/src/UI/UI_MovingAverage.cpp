//
//  UI_MovingAverage.cpp
//  KanVest
//
//  Created by Ashish . on 15/01/26.
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
    const auto& dmaValues = Analyzer::GetDMAValues(MAPriceSource::Close);
    ShowMovingAverageData(stockData, dmaValues, " SMA", shadowTexture);
  }
  void UI_MovingAverage::ShowEMA(const StockData& stockData, ImTextureID shadowTexture)
  {
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(Font(Header_24), "No data for stock", Align::Left, {10.0f, 0.0f}, Color::Error);
      return;
    }
    const auto& emaValues = Analyzer::GetEMAValues(MAPriceSource::Close);
    ShowMovingAverageData(stockData, emaValues, " EMA", shadowTexture);
  }

  void UI_MovingAverage::ShowMovingAverageData(const StockData& stockData, const std::map<int, std::vector<double>>& maMap, const std::string& maString, ImTextureID shadowTexture)
  {
    int bullish = 0;
    int bearish = 0;
    
    for (const auto& [period, maValues] : maMap)
    {
      if (maValues.back() > stockData.livePrice)
        bearish ++;
      else if (maValues.back() < stockData.livePrice and maValues.back() != 0)
        bullish++;
    }
    int total = bullish + bearish;
    
    if (bullish >= bearish)
    {
      std::string maSummary = stockData.shortName + " is trading above " + std::to_string(bullish) + " out of " + std::to_string(total) + maString + "s";
      KanVasX::UI::Text(Font(Regular), maSummary, Align::Left, {10.0f, 0.0f}, UI::Utils::StockProfitColor);
    }
    else
    {
      std::string maSummary = stockData.shortName + " is trading below " + std::to_string(bearish) + " out of " + std::to_string(total) + maString + "s";
      KanVasX::UI::Text(Font(Regular), maSummary, Align::Left, {10.0f, 0.0f}, UI::Utils::StockLossColor);
    }
    
    std::string maSummary = "If current price is greater than Moving Average, trend is bullish";
    KanVasX::UI::Text(Font(Regular), maSummary, Align::Left, {10.0f, 0.0f}, Color::White);
    
    {
      KanVasX::ScopedColor frameBg(ImGuiCol_FrameBg, Color::Alpha(UI::Utils::StockLossColor, 0.7));
      KanVasX::ScopedColor plotColor(ImGuiCol_PlotHistogram, UI::Utils::StockProfitColor);
      
      float fraction = (float)bullish / (float)total;
      KanVasX::UI::ShiftCursorX(10.0f);
      ImGui::ProgressBar(fraction, ImVec2(ImGui::GetContentRegionAvail().x - 10.0f, 0), " ");
    }
    
    auto ShowSma = [stockData](int period, double ma)
    {
      if (ma > 0)
      {
        auto GetPeriodString = [](int period, const std::string& interval) -> std::string
        {
          if (period <= 0 || interval.empty())
            return {};
          
          // Split numeric part and unit part
          size_t i = 0;
          while (i < interval.size() && std::isdigit(interval[i]))
            ++i;
          
          if (i == 0 || i == interval.size())
            return {}; // invalid interval like "m" or "10"
          
          int base = std::stoi(interval.substr(0, i));
          std::string unit = interval.substr(i); // m, h, d, wk, mo, etc.
          
          int total = base * period;
          
          return std::to_string(total) + unit;
        };
        
        std::string datString = GetPeriodString(period, stockData.dataGranularity);
        KanVasX::UI::Text(Font(Regular), datString, Align::Left, {20.0f, 5.0f}, Color::White);
        ImGui::SameLine();
        
        auto maColor = ma > stockData.livePrice ? UI::Utils::StockLossColor : UI::Utils::StockProfitColor;
        KanVasX::UI::Text(Font(Regular), KanVest::UI::Utils::FormatDoubleToString(ma), Align::Right, {-20.0f, 0.0f}, maColor);
      }
    };
    
    static int numChild = 3;
    ImVec2 maChildSize = {(ImGui::GetContentRegionAvail().x / numChild), 110};
    
    int idx = 0;
    bool allChildEnded = true;
    
    KanVasX::ScopedColor childBgColor(ImGuiCol_ChildBg, Color::BackgroundLight);
    for (const auto& [period, maValues] : maMap)
    {
      if (idx % numChild == 0)
      {
        ImGui::BeginChild(std::to_string(period).c_str(), maChildSize, true);
        KanVasX::UI::ShiftCursorY(5.0f);
        allChildEnded = false;
      }
      
      ShowSma(period, maValues.back());
      
      if (idx % numChild == numChild - 1)
      {
        if (shadowTexture)
        {
          KanVasX::UI::DrawShadowAllDirection(shadowTexture, 10.0f);
        }
        ImGui::EndChild();
        
        ImGui::SameLine();
        KanVasX::UI::ShiftCursorX(-10.0f);
        allChildEnded = true;
      }
      idx++;
    }
    ImGui::NewLine();
    
    if (!allChildEnded)
    {
      if (shadowTexture)
      {
        KanVasX::UI::DrawShadowAllDirection(shadowTexture, 10.0f);
      }
      ImGui::EndChild();
    }
  }
} // namespace KanVest
