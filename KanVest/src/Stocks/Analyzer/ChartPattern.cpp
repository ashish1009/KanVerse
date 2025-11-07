//
//  ChartPattern.cpp
//  KanVest
//
//  Created by Ashish . on 07/11/25.
//

#include "ChartPattern.hpp"

using namespace KanVest::Analysis::ChartPatterns;
using namespace KanVest::Analysis;
using namespace KanVest;

// Helper to extract highs/lows
static std::vector<double> highsFrom(const std::vector<StockPoint>& h)
{
  std::vector<double> out; out.reserve(h.size());
  for (auto &p : h)
  {
    out.push_back(p.high);
  }
  return out;
}
static std::vector<double> lowsFrom(const std::vector<StockPoint>& h)
{
  std::vector<double> out; out.reserve(h.size());
  for (auto &p : h)
  {
    out.push_back(p.low);
  }
  return out;
}
static std::vector<double> closesFrom(const std::vector<StockPoint>& h)
{
  std::vector<double> out; out.reserve(h.size());
  for (auto &p : h)
  {
    out.push_back(p.close);
  }
  return out;
}

std::vector<PatternHit> DetectDoubleTopBottom(const std::vector<StockPoint>& history)
{
  std::vector<PatternHit> out;
  if (history.size() < 8)
  {
    return out;
  }
  auto highs = highsFrom(history);
  auto lows = lowsFrom(history);
  auto closes = closesFrom(history);
  size_t n = history.size();
  
  // find two high peaks in left & right halves
  size_t mid = n/2;
  auto leftMaxIt = std::max_element(highs.begin(), highs.begin()+mid);
  auto rightMaxIt = std::max_element(highs.begin()+mid, highs.end());
  size_t leftIdx = std::distance(highs.begin(), leftMaxIt);
  size_t rightIdx = std::distance(highs.begin(), rightMaxIt);
  double leftVal = *leftMaxIt; double rightVal = *rightMaxIt;
  double tolerance = 0.03 * std::max(leftVal, rightVal); // 3%
  // require peaks similar and valley between is sufficiently lower
  double betweenMin = *std::min_element(lows.begin()+leftIdx, lows.begin()+rightIdx+1);
  if (std::fabs(leftVal - rightVal) <= tolerance && betweenMin < std::min(leftVal, rightVal) * 0.95)
  {
    PatternHit ph; ph.name = "DoubleTop"; ph.startIndex = leftIdx; ph.endIndex = rightIdx;
    ph.strength = 0.85;
    ph.rationale = "Two similar peaks with valley between; neckline at valley";
    out.push_back(ph);
  }
  // double bottom similar (swap min peaks)
  auto leftMinIt = std::min_element(lows.begin(), lows.begin()+mid);
  auto rightMinIt = std::min_element(lows.begin()+mid, lows.end());
  size_t lmin = std::distance(lows.begin(), leftMinIt);
  size_t rmin = std::distance(lows.begin(), rightMinIt);
  double lminv = *leftMinIt, rminv = *rightMinIt;
  double tol2 = 0.03 * std::max(lminv, rminv);
  double betweenMax = *std::max_element(highs.begin()+lmin, highs.begin()+rmin+1);
  if (std::fabs(lminv - rminv) <= tol2 && betweenMax > std::max(lminv, rminv) * 1.03)
  {
    PatternHit ph; ph.name = "DoubleBottom"; ph.startIndex = lmin; ph.endIndex = rmin;
    ph.strength = 0.85;
    ph.rationale = "Two similar lows with peak between; neckline at peak";
    out.push_back(ph);
  }
  
  return out;
}

std::vector<PatternHit> DetectHeadAndShoulders(const std::vector<StockPoint>& history)
{
  std::vector<PatternHit> out;
  if (history.size() < 9)
  {
    return out;
  }
  auto highs = highsFrom(history);
  size_t n = highs.size();
  
  // naive 3-peak scan: find triple peak pattern where middle > shoulders by X%
  for (size_t i = 2; i + 2 < n; ++i)
  {
    double left = highs[i-2], mid = highs[i-1], right = highs[i];
    // require mid significantly higher and shoulders similar
    if (mid > left * 1.03 && mid > right * 1.03 && std::fabs(left - right) / std::max(1.0, std::min(left, right)) < 0.06)
    {
      PatternHit ph; ph.name = "HeadAndShoulders"; ph.startIndex = i-2; ph.endIndex = i;
      ph.strength = 0.9;
      ph.rationale = "Three peaks with middle head noticeably higher than shoulders";
      out.push_back(ph);
    }
  }
  // inverse H&S by scanning lows similarly
  auto lows = lowsFrom(history);
  for (size_t i = 2; i + 2 < n; ++i)
  {
    double left = lows[i-2], mid = lows[i-1], right = lows[i];
    if (mid < left * 0.97 && mid < right * 0.97 && std::fabs(left - right) / std::max(1.0, std::min(left, right)) < 0.06)
    {
      PatternHit ph; ph.name = "InverseHeadAndShoulders"; ph.startIndex = i-2; ph.endIndex = i;
      ph.strength = 0.9;
      ph.rationale = "Three troughs with middle trough significantly lower";
      out.push_back(ph);
    }
  }
  
  return out;
}

PatternHit DetectTrendStructure(const std::vector<StockPoint>& history)
{
  PatternHit ph;
  ph.name = "Trend";
  ph.startIndex = 0;
  ph.endIndex = history.size()-1;
  ph.strength = 0.0;
  
  if (history.size() < 5)
  {
    ph.rationale = "insufficient";
    return ph;
  }
  
  size_t n = history.size();
  int hh = 0, hl = 0, lh = 0, ll = 0;
  // naive pivot-based detection: compare sequential peaks/troughs for last 5 pivots
  for (size_t i = 1; i+1 < n; ++i)
  {
    if (history[i].high > history[i-1].high && history[i].high > history[i+1].high)
    {
      ++hh;
    }
    if (history[i].low > history[i-1].low && history[i].low > history[i+1].low)
    {
      ++hl;
    }
    if (history[i].high < history[i-1].high && history[i].high < history[i+1].high)
    {
      ++lh;
    }
    if (history[i].low < history[i-1].low && history[i].low < history[i+1].low)
    {
      ++ll;
    }
  }
  if (hh >= 2 && hl >= 2)
  {
    ph.rationale = "Higher highs & higher lows detected";
    ph.strength = 0.85;
  }
  else if (lh >= 2 && ll >= 2)
  {
    ph.rationale = "Lower highs & lower lows detected";
    ph.strength = 0.85;
  }
  else
  {
    ph.rationale = "No clear trend";
    ph.strength = 0.4;
  }
  return ph;
}
