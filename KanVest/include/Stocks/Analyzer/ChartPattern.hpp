//
//  ChartPattern.hpp
//  KanVest
//
//  Created by Ashish . on 07/11/25.
//

#pragma once

#include "Stocks/Analyzer/AnalyzerStrucutre.hpp"
#include "Stocks/StockManager.hpp"

namespace KanVest::Analysis::ChartPatterns
{
  // 1️⃣ Reversal
  std::vector<PatternHit> DetectDoubleTopBottom(const std::vector<StockPoint>& history);
  std::vector<PatternHit> DetectTripleTopBottom(const std::vector<StockPoint>& history);
  std::vector<PatternHit> DetectHeadAndShoulders(const std::vector<StockPoint>& history);
  std::vector<PatternHit> DetectRoundingBottom(const std::vector<StockPoint>& history);
  std::vector<PatternHit> DetectCupAndHandle(const std::vector<StockPoint>& history);
  std::vector<PatternHit> DetectWedges(const std::vector<StockPoint>& history);
  std::vector<PatternHit> DetectBroadeningFormation(const std::vector<StockPoint>& history);
  std::vector<PatternHit> DetectDiamondPatterns(const std::vector<StockPoint>& history);
  
  // 2️⃣ Continuation
  std::vector<PatternHit> DetectTrianglesFlags(const std::vector<StockPoint>& history);
  std::vector<PatternHit> DetectRectangleChannel(const std::vector<StockPoint>& history);
  std::vector<PatternHit> DetectMeasuredMove(const std::vector<StockPoint>& history);
  
  // 3️⃣ Trend and levels
  PatternHit DetectTrendStructure(const std::vector<StockPoint>& history);
  std::vector<PatternHit> DetectSupportResistance(const std::vector<StockPoint>& history);
  std::vector<PatternHit> DetectBreakoutEvents(const std::vector<StockPoint>& history);
  
  // 4️⃣ Advanced harmonic / geometry
  std::vector<PatternHit> DetectHarmonicPatterns(const std::vector<StockPoint>& history);
  std::vector<PatternHit> DetectParabolicMove(const std::vector<StockPoint>& history);
  std::vector<PatternHit> DetectAdamEvePatterns(const std::vector<StockPoint>& history);
}
