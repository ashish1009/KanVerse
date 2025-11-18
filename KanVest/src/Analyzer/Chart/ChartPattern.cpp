//
//  ChartPattern.cpp
//  KanVest
//
//  Created by Ashish . on 18/11/25.
//

#include "ChartPattern.hpp"

namespace KanVest
{
  std::vector<ChartPatternResult> ChartPatternEngine::Analyze(const std::vector<StockPoint>& candles, int lookback)
  {
    std::vector<ChartPatternResult> out;
    if (candles.empty()) return out;
    
    int N = (int)candles.size();
    int start = 0;
    if (lookback > 0 && lookback < N) start = N - lookback;
    int end = N - 1;
    
    // Detect patterns in the requested window
    // We call each detector and accept valid results
    if (auto r = DetectDoubleTop(candles, start, end); r.IsValid()) out.push_back(r);
    if (auto r = DetectDoubleBottom(candles, start, end); r.IsValid()) out.push_back(r);
    if (auto r = DetectHeadAndShoulders(candles, start, end); r.IsValid()) out.push_back(r);
    if (auto r = DetectInverseHeadAndShoulders(candles, start, end); r.IsValid()) out.push_back(r);
    if (auto r = DetectCupAndHandle(candles, start, end); r.IsValid()) out.push_back(r);
    if (auto r = DetectTriangles(candles, start, end); r.IsValid()) out.push_back(r);
    
    return out;
  }
  
  // ---------------- Utility helpers ----------------
  
  bool ChartPatternEngine::IsLocalMax(const std::vector<StockPoint>& c, int idx, int halfWindow)
  {
    int N = (int)c.size();
    double val = c[idx].high;
    int s = std::max(0, idx - halfWindow);
    int e = std::min(N - 1, idx + halfWindow);
    for (int i = s; i <= e; ++i)
      if (i != idx && c[i].high >= val) return false;
    return true;
  }
  
  bool ChartPatternEngine::IsLocalMin(const std::vector<StockPoint>& c, int idx, int halfWindow)
  {
    int N = (int)c.size();
    double val = c[idx].low;
    int s = std::max(0, idx - halfWindow);
    int e = std::min(N - 1, idx + halfWindow);
    for (int i = s; i <= e; ++i)
      if (i != idx && c[i].low <= val) return false;
    return true;
  }
  
  // ---------------- Double Top ----------------
  
  ChartPatternResult ChartPatternEngine::DetectDoubleTop(const std::vector<StockPoint>& c, int searchStart, int searchEnd)
  {
    ChartPatternResult res;
    [[maybe_unused]] int N = (int)c.size();
    if (searchEnd - searchStart < 6) return res; // need some space
    
    // Find recent local maxima within window
    std::vector<int> peaks;
    for (int i = searchStart + 2; i <= searchEnd - 2; ++i)
    {
      if (IsLocalMax(c, i, 2)) peaks.push_back(i);
    }
    
    if (peaks.size() < 2) return res;
    
    // Try pairs of peaks from the end (most recent)
    for (int i = (int)peaks.size() - 1; i >= 1; --i)
    {
      int p2 = peaks[i];
      int p1 = peaks[i-1];
      if (p2 - p1 < 2 || p2 - p1 > 60) continue; // too close or too far
      
      double h1 = PeakHeight(c[p1]);
      double h2 = PeakHeight(c[p2]);
      double higher = std::max(h1,h2);
      double lower = std::min(h1,h2);
      double tol = 0.03 * higher; // 3% tolerance
      if (higher - lower > tol) continue; // peaks not similar
      
      // valley between peaks
      int valleyIdx = p1 + 1;
      double valley = c[valleyIdx].low;
      for (int k = p1+1; k < p2; ++k) valley = std::min(valley, c[k].low);
      
      // confirmation: price breaks valley (latest close below valley) -> strong signal
      double latestClose = c[searchEnd].close;
      bool confirmed = latestClose < valley;
      
      res.type = ChartPatternType::DOUBLE_TOP;
      res.name = "Double Top";
      res.startIndex = p1;
      res.peakIndex = p2;
      res.endIndex = searchEnd;
      res.confidence = confirmed ? 0.9 : 0.6;
      return res;
    }
    
    return res;
  }
  
  // ---------------- Double Bottom ----------------
  
  ChartPatternResult ChartPatternEngine::DetectDoubleBottom(const std::vector<StockPoint>& c, int searchStart, int searchEnd)
  {
    ChartPatternResult res;
    if (searchEnd - searchStart < 6) return res;
    std::vector<int> troughs;
    for (int i = searchStart + 2; i <= searchEnd - 2; ++i)
    {
      if (IsLocalMin(c, i, 2)) troughs.push_back(i);
    }
    if (troughs.size() < 2) return res;
    
    for (int i = (int)troughs.size() - 1; i >= 1; --i)
    {
      int t2 = troughs[i];
      int t1 = troughs[i-1];
      if (t2 - t1 < 2 || t2 - t1 > 60) continue;
      
      double l1 = TroughHeight(c[t1]);
      double l2 = TroughHeight(c[t2]);
      double higher = std::max(l1,l2);
      double lower = std::min(l1,l2);
      double tol = 0.03 * higher;
      if (higher - lower > tol) continue;
      
      // peak between troughs
      double peak = c[t1+1].high;
      for (int k = t1+1; k < t2; ++k) peak = std::max(peak, c[k].high);
      
      double latestClose = c[searchEnd].close;
      bool confirmed = latestClose > peak;
      
      res.type = ChartPatternType::DOUBLE_BOTTOM;
      res.name = "Double Bottom";
      res.startIndex = t1;
      res.peakIndex = t2;
      res.endIndex = searchEnd;
      res.confidence = confirmed ? 0.9 : 0.6;
      return res;
    }
    return res;
  }
  
  // ---------------- Head and Shoulders ----------------
  
  ChartPatternResult ChartPatternEngine::DetectHeadAndShoulders(const std::vector<StockPoint>& c, int searchStart, int searchEnd)
  {
    ChartPatternResult res;
    [[maybe_unused]] int N = (int)c.size();
    if (searchEnd - searchStart < 8) return res;
    
    // naive approach: look for three peaks with head higher than shoulders
    std::vector<int> peaks;
    for (int i = searchStart + 2; i <= searchEnd - 2; ++i)
      if (IsLocalMax(c, i, 2)) peaks.push_back(i);
    
    if (peaks.size() < 3) return res;
    
    // iterate triplets from end
    for (int i = (int)peaks.size() - 1; i >= 2; --i)
    {
      int r = peaks[i]; // right shoulder
      int m = peaks[i-1]; // head
      int l = peaks[i-2]; // left shoulder
      
      if (!(l < m && m > r)) continue;
      
      double hl = PeakHeight(c[l]);
      double hm = PeakHeight(c[m]);
      double hr = PeakHeight(c[r]);
      
      // head must be clearly higher
      if (hm < hl * 1.03 || hm < hr * 1.03) continue;
      
      // shoulders should be similar
      double tol = 0.06 * std::max(hl, hr);
      if (std::fabs(hl - hr) > tol) continue;
      
      // neckline: line connecting troughs between shoulders-head and head-shoulder
      int trough1 = l + 1;
      int trough2 = m + 1;
      double n1 = c[trough1].low; for (int k = l+1; k <= m; ++k) n1 = std::min(n1, c[k].low);
      double n2 = c[trough2].low; for (int k = m+1; k <= r; ++k) n2 = std::min(n2, c[k].low);
      double neckline = (n1 + n2) / 2.0;
      
      double latestClose = c[searchEnd].close;
      bool confirmed = latestClose < neckline;
      
      res.type = ChartPatternType::HEAD_AND_SHOULDERS;
      res.name = "Head and Shoulders";
      res.startIndex = l;
      res.peakIndex = m;
      res.endIndex = r;
      res.confidence = confirmed ? 0.92 : 0.65;
      return res;
    }
    
    return res;
  }
  
  // ---------------- Inverse Head and Shoulders ----------------
  
  ChartPatternResult ChartPatternEngine::DetectInverseHeadAndShoulders(const std::vector<StockPoint>& c, int searchStart, int searchEnd)
  {
    ChartPatternResult res;
    if (searchEnd - searchStart < 8) return res;
    
    std::vector<int> troughs;
    for (int i = searchStart + 2; i <= searchEnd - 2; ++i)
      if (IsLocalMin(c, i, 2)) troughs.push_back(i);
    
    if (troughs.size() < 3) return res;
    
    for (int i = (int)troughs.size() - 1; i >= 2; --i)
    {
      int r = troughs[i];
      int m = troughs[i-1];
      int l = troughs[i-2];
      
      double ll = TroughHeight(c[l]);
      double lm = TroughHeight(c[m]);
      double lr = TroughHeight(c[r]);
      
      if (!(lm < ll && lm < lr)) continue; // middle is bottom
      
      // shoulders similar
      double tol = 0.06 * std::max(ll, lr);
      if (std::fabs(ll - lr) > tol) continue;
      
      // neckline
      double n1 = c[l+1].high; for (int k = l+1; k <= m; ++k) n1 = std::max(n1, c[k].high);
      double n2 = c[m+1].high; for (int k = m+1; k <= r; ++k) n2 = std::max(n2, c[k].high);
      double neckline = (n1 + n2) / 2.0;
      
      double latestClose = c[searchEnd].close;
      bool confirmed = latestClose > neckline;
      
      res.type = ChartPatternType::INVERSE_HEAD_AND_SHOULDERS;
      res.name = "Inverse Head and Shoulders";
      res.startIndex = l;
      res.peakIndex = m;
      res.endIndex = r;
      res.confidence = confirmed ? 0.92 : 0.65;
      return res;
    }
    
    return res;
  }
  
  // ---------------- Cup and Handle ----------------
  
  ChartPatternResult ChartPatternEngine::DetectCupAndHandle(const std::vector<StockPoint>& c, int searchStart, int searchEnd)
  {
    ChartPatternResult res;
    [[maybe_unused]] int N = (int)c.size();
    if (searchEnd - searchStart < 30) return res; // cup takes time
    
    // naive approach: look for a large rounding bottom (cup) followed by small pullback (handle)
    // find left highs and right highs roughly equal within tolerance
    int window = std::min(200, searchEnd - searchStart);
    int s = std::max(searchStart, searchEnd - window + 1);
    
    // find global max inside window (possible right/left rim)
    double globalMax = -1e18; int globalMaxIdx = s;
    for (int i = s; i <= searchEnd; ++i) { if (c[i].high > globalMax) { globalMax = c[i].high; globalMaxIdx = i; } }
    
    // attempt to split into left rim and right rim around mid
    int mid = (s + searchEnd) / 2;
    // find left rim as max in [s, mid]
    double leftMax = -1e18; int leftIdx = s;
    for (int i = s; i <= mid; ++i) if (c[i].high > leftMax) { leftMax = c[i].high; leftIdx = i; }
    double rightMax = -1e18; int rightIdx = mid;
    for (int i = mid; i <= searchEnd; ++i) if (c[i].high > rightMax) { rightMax = c[i].high; rightIdx = i; }
    
    if (leftIdx >= rightIdx) return res;
    
    double rimTol = 0.07 * std::max(leftMax, rightMax);
    if (std::fabs(leftMax - rightMax) > rimTol) return res; // rims not similar
    
    // find bottom between rims
    double bottom = 1e18; int bottomIdx = leftIdx;
    for (int i = leftIdx; i <= rightIdx; ++i) if (c[i].low < bottom) { bottom = c[i].low; bottomIdx = i; }
    
    // ensure cup depth reasonable (not too shallow, not too deep)
    double depth = std::min(leftMax - bottom, rightMax - bottom);
    if (depth / std::max(leftMax, rightMax) < 0.05) return res; // too shallow
    
    // handle: small consolidation after right rim: check small pullback after rightIdx
    int handleStart = rightIdx + 1;
    if (handleStart + 3 >= searchEnd) return res;
    double handleMax = -1e18; for (int i = handleStart; i <= std::min(searchEnd, handleStart + 10); ++i) handleMax = std::max(handleMax, c[i].high);
    // handle pullback depth
    double hDepth = rightMax - handleMax;
    if (hDepth / rightMax > 0.12) return res; // too deep handle
    
    // success
    res.type = ChartPatternType::CUP_AND_HANDLE;
    res.name = "Cup and Handle";
    res.startIndex = leftIdx;
    res.peakIndex = bottomIdx;
    res.endIndex = std::min(searchEnd, handleStart + 10);
    res.confidence = 0.75;
    return res;
  }
  
  // ---------------- Triangles ----------------
  
  ChartPatternResult ChartPatternEngine::DetectTriangles(const std::vector<StockPoint>& c, int searchStart, int searchEnd)
  {
    ChartPatternResult res;
    [[maybe_unused]] int N = (int)c.size();
    if (searchEnd - searchStart < 10) return res;
    
    // analyze last window
    int window = std::min(60, searchEnd - searchStart);
    int s = std::max(searchStart, searchEnd - window + 1);
    
    // compute arrays of highs and lows
    std::vector<double> highs, lows;
    for (int i = s; i <= searchEnd; ++i) { highs.push_back(c[i].high); lows.push_back(c[i].low); }
    
    // fit simple linear regressions (slope) for highs and lows
    auto slope = [&](const std::vector<double>& arr)->double {
      int n = (int)arr.size();
      double xMean = (n-1)/2.0;
      double num = 0.0, den = 0.0;
      double yMean = std::accumulate(arr.begin(), arr.end(), 0.0)/n;
      for (int i = 0; i < n; ++i) { num += (i - xMean) * (arr[i] - yMean); den += (i - xMean)*(i - xMean); }
      if (den == 0) return 0.0; return num/den;
    };
    
    double slopeHigh = slope(highs);
    double slopeLow = slope(lows);
    
    // Heuristics:
    // ascending triangle: highs roughly flat (slopeHigh ≈ 0 or negative small) and lows slope positive
    if (slopeHigh > -0.0001 && slopeHigh < 0.0001 && slopeLow > 0.00001)
    {
      res.type = ChartPatternType::TRIANGLE_ASCENDING;
      res.name = "Ascending Triangle";
      res.startIndex = s;
      res.endIndex = searchEnd;
      res.confidence = 0.6;
      return res;
    }
    
    // descending triangle: lows roughly flat and highs slope negative
    if (slopeLow > -0.0001 && slopeLow < 0.0001 && slopeHigh < -0.00001)
    {
      res.type = ChartPatternType::TRIANGLE_DESCENDING;
      res.name = "Descending Triangle";
      res.startIndex = s;
      res.endIndex = searchEnd;
      res.confidence = 0.6;
      return res;
    }
    
    // symmetrical: highs slope negative, lows slope positive
    if (slopeHigh < -0.00001 && slopeLow > 0.00001)
    {
      res.type = ChartPatternType::TRIANGLE_SYMMETRICAL;
      res.name = "Symmetrical Triangle";
      res.startIndex = s;
      res.endIndex = searchEnd;
      res.confidence = 0.55;
      return res;
    }
    return res;
  }
} // namespace KanVerse
