//
//  UI_KanVestPanel.cpp
//  KanVest
//
//  Created by Ashish . on 08/01/26.
//

#include "UI_KanVestPanel.hpp"

#include "UI/UI_Chart.hpp"

#include "Stock/StockManager.hpp"

#include "Analyzer/StockAnalyzer.hpp"

namespace KanVest::UI
{
#define Font(font) KanVest::UI::Font::Get(KanVest::UI::FontType::font)
  
  using Align = KanVasX::UI::AlignX;
  using Color = KanVasX::Color;
  
  void Panel::Show()
  {
    IK_PERFORMANCE_FUNC("Panel::Show");
    
    ImGui::Begin("Stock Analyzer");
    
    // Update selected stock data
    UpdateSelectedStock();

    // Get Stock Data
    StockData stockData = StockManager::GetLatestStockData(s_selectedStockSymbol);
    
    // Analyze Stock;
    Analyzer::AnalzeStock(stockData);

    // Show Stock Data
    ImGui::BeginChild(" Stock - Data - Analyzer ", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.45f));
    {
      // Show search bar
      ShowStockSearchBar(ImGui::GetContentRegionAvail().x - 4.0f /* Padding */ , 8.0f);
    }
    ImGui::EndChild();
            
    // Show Chart
    ImGui::BeginChild(" Chart", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y));
    {
      // Show chart UI
      Chart::Show(stockData);
    }
    ImGui::EndChild();

    ImGui::End();
  }
  
  void Panel::UpdateSelectedStock()
  {
    // Update stock if new data entered
    if (s_selectedStockSymbol != s_searchedStockString and ImGui::IsKeyPressed(ImGuiKey_Enter))
    {
      // Get previous symbol stock data
      const auto& prevStockData = StockManager::GetLatestStockData(s_selectedStockSymbol);
      
      // Get default range and interval
      Range range = Range::_1D;
      Interval interval = API_Provider::GetOptimalIntervalForRange(Range::_1D);
      
      // Update range with previous range if data present
      if (prevStockData.IsValid())
      {
        range = API_Provider::GetRangeEnumFromString(prevStockData.range);
        interval = API_Provider::GetIntervalEnumFromString(prevStockData.dataGranularity);
      }
      
      // Add new symbol in stock data extracter
      s_selectedStockSymbol = s_searchedStockString;
      StockManager::AddStockDataRequest(s_selectedStockSymbol, range, interval);
    }
  }
  
  void Panel::ShowStockSearchBar(float width, float height)
  {
    // Search bar
    if (KanVasX::Widget::Search(s_searchedStockString, 128, height, width, "Enter Symbol ...", Font(Large), 40.0f))
    {
      KanViz::Utils::String::ToUpper(s_searchedStockString);
    }
  }
} // namespace KanVest::UI
