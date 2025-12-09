//
//  UI_KanVestPanel.cpp
//  KanVest
//
//  Created by Ashish . on 02/12/25.
//

#include "UI_KanVestPanel.hpp"

#include "UI/UI_Utils.hpp"

#include "Stock/StockManager.hpp"

namespace KanVest::UI
{
#define Font(font) KanVest::UI::Font::Get(KanVest::UI::FontType::font)
  
  using Align = KanVasX::UI::AlignX;
  using Color = KanVasX::Color;

  void Panel::Show()
  {
    IK_PERFORMANCE_FUNC("Panel::Show");
    KanVasX::Panel::Begin("Stock Analyzer");
    
    KanVasX::UI::Text(Font(Regular), Utils::FormatDoubleToString(ImGui::GetIO().Framerate), Align::Right, {0.0f, ImGui::GetContentRegionAvail().y - 18.0f}, Color::Gray);

    KanVasX::Panel::End(0.0f);
  }
//    static bool TempInit = true;
//    if (TempInit)
//    {
//      StockManager::AddRequest("Nifty", Range::_1D, Interval::_5M);
//      TempInit = false;
//    }
//    
//    
//    static constexpr std::string symbolName = "Nifty";
//    StockData stockData = StockManager::GetLatest(symbolName);
//    if (!stockData.IsValid())
//    {
//      std::string errorMessage = "Invalid stock data for " + symbolName;
//      KanVasX::UI::Text(Font(Header_22), errorMessage, Align::Left, {0.0f, 0.0f}, KanVasX::Color::Error);
//    }
//    else
//    {
//      ImGui::Text("symbol  :%s ", stockData.symbol.c_str());
//      ImGui::Text("currency  :%s ", stockData.currency.c_str());
//      ImGui::Text("exchangeName  :%s ", stockData.exchangeName.c_str());
//      ImGui::Text("shortName  :%s ", stockData.shortName.c_str());
//      ImGui::Text("longName  :%s ", stockData.longName.c_str());
//      ImGui::Text("instrumentType  :%s ", stockData.instrumentType.c_str());
//      ImGui::Text("timezone  :%s ", stockData.timezone.c_str());
//      ImGui::Text("range  :%s ", stockData.range.c_str());
//      ImGui::Text("dataGranularity  :%s ", stockData.dataGranularity.c_str());
//      
//      ImGui::Text("livePrice : %f ", stockData.livePrice);
//      ImGui::Text("prevClose : %f ", stockData.prevClose);
//      
//      ImGui::Text("change : %f ", stockData.change);
//      ImGui::Text("changePercent : %f ", stockData.changePercent);
//      
//      ImGui::Text("volume : %f ", stockData.volume);
//      
//      ImGui::Text("fiftyTwoHigh : %f ", stockData.fiftyTwoHigh);
//      ImGui::Text("fiftyTwoLow : %f ", stockData.fiftyTwoLow);
//      ImGui::Text("dayHigh : %f ", stockData.dayHigh);
//      ImGui::Text("dayLow : %f ", stockData.dayLow);
//    }
//  }
} // namespace KanVest::UI
