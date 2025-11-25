//
//  UI_Pivot.cpp
//  KanVest
//
//  Created by Ashish . on 24/11/25.
//

#include "UI_Pivot.hpp"

#include "UI/UI_Utils.hpp"

#include "Analyzer/Chart/Pivot.hpp"

#include "Analyzer/StockAnalyzer.hpp"

namespace KanVest
{
  void UI_Pivot::Show(const StockData& stockData, ImTextureID shadowTexture)
  {
    if (!stockData.IsValid())
    {
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_24),
                        "No data for stock", KanVasX::UI::AlignX::Left,
                        {10.0f, 0.0f}, KanVasX::Color::Error);
      return;
    }
    
    const auto& pivots = Analyzer::GetPivots();
    
    // Copy to local so we can sort
    std::vector<SRLevel> supports = pivots.supports;
    std::vector<SRLevel> resistances = pivots.resistances;
    
    // --- Sort by strength first (count -> recent) ---
    auto strengthSort = [](const SRLevel& a, const SRLevel& b) {
      if (a.count != b.count) return a.count > b.count;
      if (a.lastTouchIdx != b.lastTouchIdx) return a.lastTouchIdx > b.lastTouchIdx;
      return false; // tie-break left for price sort later
    };
    
    std::sort(supports.begin(), supports.end(), strengthSort);
    std::sort(resistances.begin(), resistances.end(), strengthSort);
    
    // --- Sort for display visually: supports low->high, resistances high->low ---
    auto priceSortSupport = [](const SRLevel& a, const SRLevel& b) { return a.price < b.price; };
    auto priceSortResistance = [](const SRLevel& a, const SRLevel& b) { return a.price > b.price; };
    
    std::stable_sort(supports.begin(), supports.end(), priceSortSupport);
    std::stable_sort(resistances.begin(), resistances.end(), priceSortResistance);
    
    static const int32_t MaxLevel = 4;
    
    ImVec2 smaChildSize = { ImGui::GetContentRegionAvail().x / 2, 130.0f };
    
    // --- Display Supports ---
    ImGui::BeginChild("Support", smaChildSize, true);
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_26), "Support",
                      KanVasX::UI::AlignX::Center, {0.0f, 0.0f}, KanVasX::Color::White);
    KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1);

    for (int i = 0; i < std::min(MaxLevel, (int)supports.size()); i++)
    {
      auto sup = supports[i].price;
      ImU32 color = (stockData.livePrice < sup ? KanVasX::Color::Red : KanVasX::Color::Cyan);
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_20),
                        KanVest::UI::Utils::FormatDoubleToString(sup),
                        KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, color);

      ImGui::SameLine();
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_20),
                        std::to_string(supports[i].count),
                        KanVasX::UI::AlignX::Center, {0, 0.0f}, color);

      ImGui::SameLine();
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_20),
                        std::to_string(supports[i].lastTouchIdx),
                        KanVasX::UI::AlignX::Right, {-10.0f, 0.0f}, color);
    }
    if (shadowTexture)
      KanVasX::UI::DrawShadowAllDirection(shadowTexture);
    ImGui::EndChild();
    
    ImGui::SameLine();
    
    // --- Display Resistances ---
    ImGui::BeginChild("Resistance", smaChildSize, true);
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_26), "Resistance",
                      KanVasX::UI::AlignX::Center, {0.0f, 0.0f}, KanVasX::Color::White);
    KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1);

    for (int i = 0; i < std::min(MaxLevel, (int)resistances.size()); i++)
    {
      auto res = resistances[i].price;
      ImU32 color = (stockData.livePrice < res ? KanVasX::Color::Red : KanVasX::Color::Cyan);
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_20),
                        KanVest::UI::Utils::FormatDoubleToString(res),
                        KanVasX::UI::AlignX::Left, {10.0f, 0.0f}, color);
      
      ImGui::SameLine();
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_20),
                        std::to_string(resistances[i].count),
                        KanVasX::UI::AlignX::Center, {0, 0.0f}, color);
      
      ImGui::SameLine();
      KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header_20),
                        std::to_string(resistances[i].lastTouchIdx),
                        KanVasX::UI::AlignX::Right, {-10.0f, 0.0f}, color);

    }
    if (shadowTexture)
      KanVasX::UI::DrawShadowAllDirection(shadowTexture);
    ImGui::EndChild();
  }
} // namespace KanVest
