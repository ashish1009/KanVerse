//
//  ChartPattern.cpp
//  KanVest
//
//  Created by Ashish . on 07/11/25.
//

#include "ChartPattern.hpp"

#include "Stocks/Analyzer/AnalyzerStrucutre.hpp"
#include "Stocks/StockManager.hpp"

namespace KanVest::Analysis::ChartPatterns
{
  // --------------------------- Configuration / helpers ---------------------------
  static constexpr double DEFAULT_PEAK_TOL = 0.03; // 3% tolerance for "similar" peaks/lows
  static constexpr size_t DEFAULT_MIN_PATTERN_SPAN = 6; // minimum bars covering a pattern
  [[maybe_unused]] static constexpr size_t DEFAULT_MIN_PIVOTS = 3; // minimal pivots to search
  
  inline double nan() { return std::numeric_limits<double>::quiet_NaN(); }
  
  static std::vector<double> highsFrom(const std::vector<StockPoint>& h) {
    std::vector<double> out; out.reserve(h.size()); for (auto &p : h) out.push_back(p.high); return out;
  }
  static std::vector<double> lowsFrom(const std::vector<StockPoint>& h) {
    std::vector<double> out; out.reserve(h.size()); for (auto &p : h) out.push_back(p.low); return out;
  }
  static std::vector<double> closesFrom(const std::vector<StockPoint>& h) {
    std::vector<double> out; out.reserve(h.size()); for (auto &p : h) out.push_back(p.close); return out;
  }
  [[maybe_unused]] static std::vector<double> opensFrom(const std::vector<StockPoint>& h) {
    std::vector<double> out; out.reserve(h.size()); for (auto &p : h) out.push_back(p.open); return out;
  }
  [[maybe_unused]] static std::vector<uint64_t> volsFrom(const std::vector<StockPoint>& h) {
    std::vector<uint64_t> out; out.reserve(h.size()); for (auto &p : h) out.push_back(p.volume); return out;
  }
  
  static double pctDiff(double a, double b) {
    if (std::isnan(a) || std::isnan(b) || a == 0.0) return std::abs((b - a) / std::max(1e-9, std::abs(a)));
    return std::abs((b - a) / std::max(1e-9, std::abs(a)));
  }
  
  static std::string fmtDouble(double v, int prec = 4) {
    if (std::isnan(v)) return std::string("N/A");
    std::ostringstream ss; ss.setf(std::ios::fixed); ss.precision(prec); ss << v; return ss.str();
  }
  
  // Find local pivot peaks/troughs using simple neighborhood check (radius = 1..r)
  static std::vector<size_t> FindPivotPeaks(const std::vector<double>& series, size_t radius = 2) {
    std::vector<size_t> out;
    if (series.size() < 3) return out;
    for (size_t i = radius; i + radius < series.size(); ++i) {
      bool ok = true;
      for (size_t r = 1; r <= radius; ++r) {
        if (!(series[i] > series[i - r] && series[i] >= series[i + r])) { ok = false; break; }
      }
      if (ok) out.push_back(i);
    }
    return out;
  }
  static std::vector<size_t> FindPivotTroughs(const std::vector<double>& series, size_t radius = 2) {
    std::vector<size_t> out;
    if (series.size() < 3) return out;
    for (size_t i = radius; i + radius < series.size(); ++i) {
      bool ok = true;
      for (size_t r = 1; r <= radius; ++r) {
        if (!(series[i] < series[i - r] && series[i] <= series[i + r])) { ok = false; break; }
      }
      if (ok) out.push_back(i);
    }
    return out;
  }
  
  // Linear regression slope (least squares) on x=0..n-1 vs y series (returns slope)
  static double LinearSlope(const std::vector<double>& y, size_t start, size_t end) {
    if (end <= start+1) return nan();
    double n = static_cast<double>(end - start);
    double sumx = 0.0, sumy = 0.0, sumxy = 0.0, sumx2 = 0.0;
    for (size_t i = start; i < end; ++i) {
      double x = static_cast<double>(i - start);
      double v = y[i];
      sumx += x;
      sumy += v;
      sumxy += x * v;
      sumx2 += x * x;
    }
    double denom = n*sumx2 - sumx*sumx;
    if (std::abs(denom) < 1e-12) return nan();
    return (n*sumxy - sumx*sumy) / denom;
  }
  
  // average slope magnitude
  [[maybe_unused]] static double AvgSlopeAbs(const std::vector<double>& y, size_t start, size_t end) {
    double s = LinearSlope(y, start, end);
    if (std::isnan(s)) return 0.0;
    return std::abs(s);
  }
  
  // measure volatility (stddev of returns)
  static double ReturnStdDev(const std::vector<double>& series, size_t start, size_t end) {
    if (end <= start+1) return 0.0;
    std::vector<double> r;
    for (size_t i = start+1; i < end; ++i) r.push_back((series[i] - series[i-1]) / std::max(1e-9, series[i-1]));
    if (r.empty()) return 0.0;
    double mean = std::accumulate(r.begin(), r.end(), 0.0) / r.size();
    double var = 0.0;
    for (auto v : r) var += (v - mean)*(v - mean);
    var /= r.size();
    return std::sqrt(var);
  }
  
  // --------------------------- Pattern implementations ---------------------------
  
  // Helper to create PatternHit with explanation
  static PatternHit MakeHit(const std::string& name, size_t start, size_t end, double strength, const std::string& rationale) {
    PatternHit ph; ph.name = name; ph.startIndex = start; ph.endIndex = end; ph.strength = std::max(0.0, std::min(1.0, strength)); ph.rationale = rationale; return ph;
  }
  
  // ---------------- Double Top / Bottom ----------------
  std::vector<PatternHit> DetectDoubleTopBottom(const std::vector<StockPoint>& history) {
    std::vector<PatternHit> out;
    size_t n = history.size();
    if (n < DEFAULT_MIN_PATTERN_SPAN) return out;
    
    auto highs = highsFrom(history);
    auto lows  = lowsFrom(history);
    
    // find peak pivots and trough pivots
    auto peaks = FindPivotPeaks(highs, 2);
    auto troughs = FindPivotTroughs(lows, 2);
    // Need at least two peaks/troughs separated by valley/peak
    if (peaks.size() >= 2) {
      // scan adjacent peak pairs
      for (size_t i = 0; i + 1 < peaks.size(); ++i) {
        size_t left = peaks[i], right = peaks[i+1];
        if (right - left < 3) continue; // too close
        double leftVal = highs[left], rightVal = highs[right];
        double tol = DEFAULT_PEAK_TOL * std::max(leftVal, rightVal);
        if (std::abs(leftVal - rightVal) <= tol) {
          // valley between
          size_t valleyIdx = left+1;
          double valley = std::numeric_limits<double>::infinity();
          for (size_t k = left+1; k < right; ++k) if (lows[k] < valley) { valley = lows[k]; valleyIdx = k; }
          // neckline at valley; require valley sufficiently lower
          if (valley < std::min(leftVal, rightVal) * 0.97) {
            double measured = std::min(leftVal, rightVal) - valley;
            size_t start = left, end = right;
            double strength = 0.8 * (1.0 - pctDiff(leftVal, rightVal));
            std::ostringstream r;
            r << "DoubleTop peaks at " << fmtDouble(leftVal) << " and " << fmtDouble(rightVal)
            << ", neckline " << fmtDouble(valley) << ", measured move " << fmtDouble(measured);
            out.push_back(MakeHit("DoubleTop", start, end, strength, r.str()));
          }
        }
      }
    }
    // Double Bottom - symmetrical
    if (troughs.size() >= 2) {
      for (size_t i = 0; i + 1 < troughs.size(); ++i) {
        size_t left = troughs[i], right = troughs[i+1];
        if (right - left < 3) continue;
        double leftVal = lows[left], rightVal = lows[right];
        double tol = DEFAULT_PEAK_TOL * std::max(std::abs(leftVal), std::abs(rightVal));
        if (std::abs(leftVal - rightVal) <= tol) {
          // peak between
          size_t peakIdx = left+1; double peak = -std::numeric_limits<double>::infinity();
          for (size_t k = left+1; k < right; ++k) if (highs[k] > peak) { peak = highs[k]; peakIdx = k; }
          if (peak > std::max(leftVal, rightVal) * 1.03) {
            double measured = peak - std::max(leftVal, rightVal);
            size_t start = left, end = right;
            double strength = 0.8 * (1.0 - pctDiff(leftVal, rightVal));
            std::ostringstream r;
            r << "DoubleBottom lows at " << fmtDouble(leftVal) << " and " << fmtDouble(rightVal)
            << ", neckline " << fmtDouble(peak) << ", measured move " << fmtDouble(measured);
            out.push_back(MakeHit("DoubleBottom", start, end, strength, r.str()));
          }
        }
      }
    }
    
    return out;
  }
  
  // ---------------- Triple Top / Bottom ----------------
  std::vector<PatternHit> DetectTripleTopBottom(const std::vector<StockPoint>& history) {
    std::vector<PatternHit> out;
    size_t n = history.size(); if (n < 9) return out;
    auto highs = highsFrom(history);
    auto lows = lowsFrom(history);
    
    auto peaks = FindPivotPeaks(highs, 2);
    if (peaks.size() >= 3) {
      for (size_t i = 0; i + 2 < peaks.size(); ++i) {
        size_t a = peaks[i], b = peaks[i+1], c = peaks[i+2];
        if (b - a < 3 || c - b < 3) continue;
        double va = highs[a], vb = highs[b], vc = highs[c];
        double maxv = std::max({va, vb, vc});
        double tol = DEFAULT_PEAK_TOL * maxv;
        if (std::abs(va - vb) <= tol && std::abs(vb - vc) <= tol) {
          double strength = 0.75;
          std::ostringstream ss; ss << "TripleTop peaks at " << fmtDouble(va) << "," << fmtDouble(vb) << "," << fmtDouble(vc);
          out.push_back(MakeHit("TripleTop", a, c, strength, ss.str()));
        }
      }
    }
    
    auto troughs = FindPivotTroughs(lows, 2);
    if (troughs.size() >= 3) {
      for (size_t i = 0; i + 2 < troughs.size(); ++i) {
        size_t a = troughs[i], b = troughs[i+1], c = troughs[i+2];
        if (b - a < 3 || c - b < 3) continue;
        double va = lows[a], vb = lows[b], vc = lows[c];
        double tol = DEFAULT_PEAK_TOL * std::max({std::abs(va), std::abs(vb), std::abs(vc)});
        if (std::abs(va - vb) <= tol && std::abs(vb - vc) <= tol) {
          double strength = 0.75;
          std::ostringstream ss; ss << "TripleBottom at " << fmtDouble(va) << "," << fmtDouble(vb) << "," << fmtDouble(vc);
          out.push_back(MakeHit("TripleBottom", a, c, strength, ss.str()));
        }
      }
    }
    
    return out;
  }
  
  // ---------------- Head & Shoulders ----------------
  std::vector<PatternHit> DetectHeadAndShoulders(const std::vector<StockPoint>& history) {
    std::vector<PatternHit> out;
    size_t n = history.size(); if (n < 9) return out;
    auto highs = highsFrom(history);
    auto lows  = lowsFrom(history);
    
    // Use pivot peaks to find left shoulder, head, right shoulder pattern
    auto peaks = FindPivotPeaks(highs, 2);
    // scan triples of peaks
    for (size_t i = 0; i + 2 < peaks.size(); ++i) {
      size_t ls = peaks[i], head = peaks[i+1], rs = peaks[i+2];
      if (head - ls < 3 || rs - head < 3) continue;
      double lval = highs[ls], hval = highs[head], rval = highs[rs];
      // require head noticeably higher than shoulders and shoulders similar
      if (hval > lval * 1.03 && hval > rval * 1.03 && pctDiff(lval, rval) < 0.06) {
        // neckline: line between troughs between LS-head and head-RS
        size_t troughA = ls+1; double minA = std::numeric_limits<double>::infinity();
        for (size_t k = ls+1; k < head; ++k) if (lows[k] < minA) { minA = lows[k]; troughA = k; }
        size_t troughB = head+1; double minB = std::numeric_limits<double>::infinity();
        for (size_t k = head+1; k < rs; ++k) if (lows[k] < minB) { minB = lows[k]; troughB = k; }
        double neckline = (minA + minB) * 0.5;
        double measured = hval - neckline;
        double strength = 0.9 * (1.0 - pctDiff(lval, rval)); // penalty if shoulders differ
        std::ostringstream ss; ss << "H&S LS=" << fmtDouble(lval) << " Head=" << fmtDouble(hval) << " RS=" << fmtDouble(rval)
        << ", neckline=" << fmtDouble(neckline) << ", measured=" << fmtDouble(measured);
        out.push_back(MakeHit("HeadAndShoulders", ls, rs, strength, ss.str()));
      }
    }
    
    // Inverse Head & Shoulders on troughs
    auto troughs = FindPivotTroughs(lows, 2);
    for (size_t i = 0; i + 2 < troughs.size(); ++i) {
      size_t ls = troughs[i], head = troughs[i+1], rs = troughs[i+2];
      if (head - ls < 3 || rs - head < 3) continue;
      double lval = lows[ls], hval = lows[head], rval = lows[rs];
      if (hval < lval * 0.97 && hval < rval * 0.97 && pctDiff(lval, rval) < 0.06) {
        size_t peakA = ls+1; double maxA = -std::numeric_limits<double>::infinity();
        for (size_t k = ls+1; k < head; ++k) if (highs[k] > maxA) { maxA = highs[k]; peakA = k; }
        size_t peakB = head+1; double maxB = -std::numeric_limits<double>::infinity();
        for (size_t k = head+1; k < rs; ++k) if (highs[k] > maxB) { maxB = highs[k]; peakB = k; }
        double neckline = (maxA + maxB) * 0.5;
        double measured = neckline - hval;
        double strength = 0.9 * (1.0 - pctDiff(lval, rval));
        std::ostringstream ss; ss << "Inverse H&S lows " << fmtDouble(lval) << "," << fmtDouble(hval) << "," << fmtDouble(rval)
        << " neckline=" << fmtDouble(neckline) << " measured=" << fmtDouble(measured);
        out.push_back(MakeHit("InverseHeadAndShoulders", ls, rs, strength, ss.str()));
      }
    }
    
    return out;
  }
  
  // ---------------- Rounding Bottom / Saucer ----------------
  std::vector<PatternHit> DetectRoundingBottom(const std::vector<StockPoint>& history) {
    std::vector<PatternHit> out;
    size_t n = history.size(); if (n < 20) return out; // need long span
    auto closes = closesFrom(history);
    
    // rough heuristic: find a long span where slope goes negative -> near-zero -> positive and curvature low (low return stddev)
    size_t span = std::min(n/2, static_cast<size_t>(40));
    for (size_t start = 0; start + span < n; start += span/3) {
      size_t mid = start + span/2;
      double s1 = LinearSlope(closes, start, mid);
      double s2 = LinearSlope(closes, mid, start + span);
      if (std::isnan(s1) || std::isnan(s2)) continue;
      if (s1 < 0.0 && s2 > 0.0) {
        double vol = ReturnStdDev(closes, start, start+span);
        if (vol < 0.02) {
          double lowVal = *std::min_element(closes.begin()+start, closes.begin()+start+span);
          double highVal = *std::max_element(closes.begin()+start, closes.begin()+start+span);
          double strength = 0.7 + 0.3 * (1.0 - vol/0.02);
          std::ostringstream ss; ss << "RoundingBottom span [" << start << "," << start+span << "] min=" << fmtDouble(lowVal) << " max=" << fmtDouble(highVal);
          out.push_back(MakeHit("RoundingBottom", start, start+span-1, strength, ss.str()));
        }
      }
    }
    return out;
  }
  
  // ---------------- Cup & Handle ----------------
  std::vector<PatternHit> DetectCupAndHandle(const std::vector<StockPoint>& history) {
    std::vector<PatternHit> out;
    size_t n = history.size(); if (n < 30) return out;
    auto closes = closesFrom(history);
    // heuristic: find a U-shaped cup (like RoundingBottom) followed by short consolidation (handle) pulling back less than ~1/3 of cup depth
    // scan windows up to size 60
    for (size_t window = 20; window <= std::min(n, static_cast<size_t>(80)); window += 5) {
      for (size_t start = 0; start + window < n; start += window/4) {
        [[maybe_unused]] size_t end = start + window;
        // candidate cup
        [[maybe_unused]] double maxLeft = *std::max_element(closes.begin()+start, closes.begin()+end);
        size_t minIdx = std::distance(closes.begin(), std::min_element(closes.begin()+start, closes.begin()+end));
        double minVal = closes[minIdx];
        // need left rim and right rim near same level (within 10%)
        double leftRim = closes[start];
        double rightRim = closes[end-1];
        if (pctDiff(leftRim, rightRim) > 0.10) continue;
        double depth = std::max({1e-9, std::abs(leftRim - minVal), std::abs(rightRim - minVal)});
        // find handle: small consolidation on right ~5-15% of window length
        size_t handleStart = end - std::min(static_cast<size_t>(std::max(3.0, window*0.15)), end-start-1);
        if (handleStart + 2 >= end) continue;
        double handleMax = *std::max_element(closes.begin()+handleStart, closes.begin()+end);
        double handleMin = *std::min_element(closes.begin()+handleStart, closes.begin()+end);
        if ((handleMax - handleMin) > 0.33 * depth) continue; // too deep handle
        // validate cup shape by slopes
        double slopeLeft = LinearSlope(closes, start, minIdx+1);
        double slopeRight = LinearSlope(closes, minIdx, end);
        if (slopeLeft < 0.0 && slopeRight > 0.0) {
          double strength = 0.75;
          std::ostringstream ss; ss << "Cup (start=" << start << " end=" << end-1 << ") depth=" << fmtDouble(depth) << " handleRange=" << fmtDouble(handleMax-handleMin);
          out.push_back(MakeHit("CupAndHandle", start, end-1, strength, ss.str()));
        }
      }
    }
    return out;
  }
  
  // ---------------- Wedges (Rising / Falling) ----------------
  std::vector<PatternHit> DetectWedges(const std::vector<StockPoint>& history) {
    std::vector<PatternHit> out;
    size_t n = history.size(); if (n < 12) return out;
    auto highs = highsFrom(history);
    auto lows  = lowsFrom(history);
    // Use linear slopes of highs and lows on sliding windows: if both slopes converge (one up, one down; magnitudes decreasing) -> wedge
    size_t maxWindow = std::min(n, static_cast<size_t>(40));
    for (size_t win = 8; win <= maxWindow; win += 4) {
      for (size_t start = 0; start + win <= n; start += std::max((size_t)1, win/4)) {
        size_t end = start + win;
        double slopeHigh = LinearSlope(highs, start, end);
        double slopeLow = LinearSlope(lows, start, end);
        if (std::isnan(slopeHigh) || std::isnan(slopeLow)) continue;
        // Rising wedge (bearish reversal): both slopes positive but highs slope < lows slope (converging upwards)
        if (slopeHigh > 0 && slopeLow > 0 && std::abs(slopeLow) > std::abs(slopeHigh) && std::abs(slopeLow - slopeHigh) / std::max(1e-9, std::abs(slopeLow)) > 0.1) {
          double strength = 0.7;
          std::ostringstream ss; ss << "RisingWedge window[" << start << "," << end-1 << "] slopes H=" << fmtDouble(slopeHigh) << " L=" << fmtDouble(slopeLow);
          out.push_back(MakeHit("RisingWedge", start, end-1, strength, ss.str()));
        }
        // Falling wedge (bullish reversal)
        if (slopeHigh < 0 && slopeLow < 0 && std::abs(slopeHigh) > std::abs(slopeLow) && std::abs(slopeHigh - slopeLow) / std::max(1e-9, std::abs(slopeHigh)) > 0.1) {
          double strength = 0.7;
          std::ostringstream ss; ss << "FallingWedge window[" << start << "," << end-1 << "] slopes H=" << fmtDouble(slopeHigh) << " L=" << fmtDouble(slopeLow);
          out.push_back(MakeHit("FallingWedge", start, end-1, strength, ss.str()));
        }
      }
    }
    return out;
  }
  
  // ---------------- Broadening / Megaphone ----------------
  std::vector<PatternHit> DetectBroadeningFormation(const std::vector<StockPoint>& history) {
    std::vector<PatternHit> out;
    size_t n = history.size(); if (n < 12) return out;
    auto highs = highsFrom(history);
    auto lows  = lowsFrom(history);
    
    // detect pattern where highs trend up while lows trend down (divergent)
    for (size_t win = 8; win <= std::min(n, static_cast<size_t>(40)); win += 4) {
      for (size_t start = 0; start + win <= n; start += std::max((size_t)1, win/4)) {
        size_t end = start + win;
        double slopeHigh = LinearSlope(highs, start, end);
        double slopeLow = LinearSlope(lows, start, end);
        if (slopeHigh > 0.0 && slopeLow < 0.0) {
          double strength = 0.6;
          std::ostringstream ss; ss << "Broadening (megaphone) [" << start << "," << end-1 << "] highSlope=" << fmtDouble(slopeHigh) << " lowSlope=" << fmtDouble(slopeLow);
          out.push_back(MakeHit("BroadeningFormation", start, end-1, strength, ss.str()));
        }
      }
    }
    
    return out;
  }
  
  // ---------------- Diamond Top/Bottom ----------------
  std::vector<PatternHit> DetectDiamondPatterns(const std::vector<StockPoint>& history) {
    std::vector<PatternHit> out;
    size_t n = history.size(); if (n < 14) return out;
    auto highs = highsFrom(history);
    auto lows  = lowsFrom(history);
    
    // diamond approx: initial broadening then contracting; combine broadening detection followed by wedge detection on same span
    for (size_t win = 12; win <= std::min(n, static_cast<size_t>(60)); win += 6) {
      for (size_t start = 0; start + win <= n; start += std::max((size_t)1, win/6)) {
        size_t mid = start + win/2;
        double slopeHighLeft = LinearSlope(highs, start, mid);
        double slopeLowLeft = LinearSlope(lows, start, mid);
        double slopeHighRight = LinearSlope(highs, mid, start+win);
        double slopeLowRight = LinearSlope(lows, mid, start+win);
        if (slopeHighLeft > 0 && slopeLowLeft < 0 && slopeHighRight < 0 && slopeLowRight > 0) {
          double strength = 0.7;
          std::ostringstream ss; ss << "Diamond pattern approx [" << start << "," << start+win-1 << "]";
          out.push_back(MakeHit("DiamondPattern", start, start+win-1, strength, ss.str()));
        }
      }
    }
    
    return out;
  }
  
  // ---------------- Triangles (sym/asc/desc), Flags, Pennants ----------------
  std::vector<PatternHit> DetectTrianglesFlags(const std::vector<StockPoint>& history) {
    std::vector<PatternHit> out;
    size_t n = history.size(); if (n < 8) return out;
    auto highs = highsFrom(history);
    auto lows  = lowsFrom(history);
    auto closes = closesFrom(history);
    
    // sliding windows
    for (size_t win = 8; win <= std::min(n, static_cast<size_t>(60)); win += 4) {
      for (size_t start = 0; start + win <= n; start += std::max((size_t)1, win/4)) {
        size_t end = start + win;
        // get peaks/troughs inside window
        auto peakIdx = FindPivotPeaks(highs, 1);
        auto troughIdx = FindPivotTroughs(lows, 1);
        // compute slope lines for highs and lows inside window
        double slopeHigh = LinearSlope(highs, start, end);
        double slopeLow = LinearSlope(lows, start, end);
        double maxHigh = *std::max_element(highs.begin()+start, highs.begin()+end);
        double minLow = *std::min_element(lows.begin()+start, lows.begin()+end);
        
        // Symmetrical triangle: highs slope down slightly, lows slope up slightly (converging)
        if (slopeHigh < 0.02 && slopeLow > -0.02 && slopeHigh < 0.0 && slopeLow > 0.0 && (maxHigh - minLow) / minLow < 0.25) {
          double strength = 0.75;
          std::ostringstream ss; ss << "SymmetricTriangle [" << start << "," << end-1 << "] slopes H=" << fmtDouble(slopeHigh) << " L=" << fmtDouble(slopeLow);
          out.push_back(MakeHit("SymmetricTriangle", start, end-1, strength, ss.str()));
        }
        // Ascending triangle: highs roughly flat, lows slope up
        if (std::abs(slopeHigh) < 0.01 && slopeLow > 0.005) {
          double strength = 0.75;
          std::ostringstream ss; ss << "AscendingTriangle [" << start << "," << end-1 << "]";
          out.push_back(MakeHit("AscendingTriangle", start, end-1, strength, ss.str()));
        }
        // Descending triangle: lows roughly flat, highs slope down
        if (std::abs(slopeLow) < 0.01 && slopeHigh < -0.005) {
          double strength = 0.75;
          std::ostringstream ss; ss << "DescendingTriangle [" << start << "," << end-1 << "]";
          out.push_back(MakeHit("DescendingTriangle", start, end-1, strength, ss.str()));
        }
        
        // Flags / Pennants: short consolidation after steep move - high slope then small pennant
        double preSlope = LinearSlope( closes, std::max<size_t>(0, (start > win ? start - win : 0)), start );
        if (!std::isnan(preSlope) && std::abs(preSlope) > 0.02 && (end - start) <= 20) { // previous strong leg
          double pennantHighSlope = LinearSlope(highs, start, end);
          double pennantLowSlope = LinearSlope(lows, start, end);
          if (std::abs(pennantHighSlope) < 0.01 && std::abs(pennantLowSlope) < 0.01 && (maxHigh - minLow)/minLow < 0.10) {
            std::string nm = ((preSlope > 0) ? "BullFlag" : "BearFlag");
            double strength = 0.7;
            out.push_back(MakeHit(nm, start, end-1, strength, "Flag or pennant after strong leg"));
          }
        }
      }
    }
    
    return out;
  }
  
  // ---------------- Rectangle / Channel ----------------
  std::vector<PatternHit> DetectRectangleChannel(const std::vector<StockPoint>& history) {
    std::vector<PatternHit> out;
    size_t n = history.size(); if (n < 8) return out;
    auto highs = highsFrom(history);
    auto lows  = lowsFrom(history);
    
    // find horizontal range where highs cluster near a resistance and lows cluster near support
    for (size_t win = 8; win <= std::min(n, static_cast<size_t>(60)); win += 4) {
      for (size_t start = 0; start + win <= n; start += std::max((size_t)1, win/4)) {
        size_t end = start + win;
        double highMed = std::accumulate(highs.begin()+start, highs.begin()+end, 0.0) / static_cast<double>(win);
        double lowMed = std::accumulate(lows.begin()+start, lows.begin()+end, 0.0) / static_cast<double>(win);
        // compute variance around theses medians
        double varHigh = 0.0, varLow = 0.0;
        for (size_t i = start; i < end; ++i) {
          varHigh += std::abs(highs[i] - highMed);
          varLow  += std::abs(lows[i]  - lowMed);
        }
        varHigh /= static_cast<double>(win);
        varLow  /= static_cast<double>(win);
        if (varHigh / std::max(1e-9, highMed) < 0.02 && varLow / std::max(1e-9, lowMed) < 0.02) {
          double strength = 0.75;
          std::ostringstream ss; ss << "Rectangle/Channel [" << start << "," << end-1 << "] resistance~" << fmtDouble(highMed) << " support~" << fmtDouble(lowMed);
          out.push_back(MakeHit("RectangleChannel", start, end-1, strength, ss.str()));
        }
      }
    }
    return out;
  }
  
  // ---------------- Measured Move ----------------
  std::vector<PatternHit> DetectMeasuredMove(const std::vector<StockPoint>& history) {
    // Measured moves are usually derived from other patterns. We'll attempt to generate measured target for double & H&S.
    std::vector<PatternHit> out;
    auto dt = DetectDoubleTopBottom(history);
    for (auto &p : dt) {
      // compute neckline (we attempted to describe it in rationale earlier) — here estimate as valley between start and end
      auto closes = closesFrom(history); auto highs = highsFrom(history); auto lows = lowsFrom(history);
      size_t left = p.startIndex, right = p.endIndex;
      double peak = -1.0, valley = 1e12;
      for (size_t i = left; i <= right; ++i) { peak = std::max(peak, highs[i]); valley = std::min(valley, lows[i]); }
      double target = (peak - valley); // measured height
      std::ostringstream ss; ss << "MeasuredMove from " << fmtDouble(peak) << " to " << fmtDouble(valley) << " height=" << fmtDouble(target);
      out.push_back(MakeHit(std::string("MeasuredMove_for_")+p.name, p.startIndex, p.endIndex, p.strength * 0.9, ss.str()));
    }
    auto hs = DetectHeadAndShoulders(history);
    for (auto &p : hs) {
      // approximate neckline between start and end
      size_t left = p.startIndex, right = p.endIndex;
      auto highs = highsFrom(history); auto lows = lowsFrom(history);
      double head = -1.0, neckline = 0.0;
      for (size_t i = left; i <= right; ++i) head = std::max(head, highs[i]);
      [[maybe_unused]] double minA = 1e12, minB = 1e12;
      for (size_t i = left; i <= right; ++i) minA = std::min(minA, lows[i]);
      neckline = minA;
      double target = head - neckline;
      std::ostringstream ss; ss << "H&S measured move height=" << fmtDouble(target);
      out.push_back(MakeHit("MeasuredMove_for_HS", left, right, p.strength * 0.9, ss.str()));
    }
    return out;
  }
  
  // ---------------- Trend Structure (HH/HL / LH/LL) ----------------
  PatternHit DetectTrendStructure(const std::vector<StockPoint>& history) {
    PatternHit ph; ph.name = "TrendStructure"; ph.startIndex = 0; ph.endIndex = history.size() ? history.size()-1 : 0; ph.strength = 0.0;
    if (history.size() < 5) { ph.rationale = "insufficient data"; return ph; }
    auto highs = highsFrom(history); auto lows = lowsFrom(history);
    // count simple pivots
    int hh = 0, hl = 0, lh = 0, ll = 0;
    for (size_t i = 2; i + 2 < history.size(); ++i) {
      if (highs[i] > highs[i-1] && highs[i] > highs[i+1]) {
        // local peak
        if (highs[i] > highs[i-1]) ++hh;
        if (lows[i] > lows[i-1]) ++hl;
      }
      if (highs[i] < highs[i-1] && highs[i] < highs[i+1]) ++lh;
      if (lows[i] < lows[i-1] && lows[i] < lows[i+1]) ++ll;
    }
    if (hh >= 2 && hl >= 2) { ph.strength = 0.9; ph.rationale = "Higher highs and higher lows detected (uptrend)"; }
    else if (lh >=2 && ll >=2) { ph.strength = 0.9; ph.rationale = "Lower highs and lower lows detected (downtrend)"; }
    else { ph.strength = 0.5; ph.rationale = "No clear multi-pivot trend"; }
    return ph;
  }
  
  // ---------------- Support & Resistance detection ----------------
  std::vector<PatternHit> DetectSupportResistance(const std::vector<StockPoint>& history) {
    std::vector<PatternHit> out;
    size_t n = history.size(); if (n < 6) return out;
    auto highs = highsFrom(history); auto lows = lowsFrom(history);
    
    // cluster top N local highs and lows into levels by proximity
    auto peaks = FindPivotPeaks(highs, 2);
    auto troughs = FindPivotTroughs(lows, 2);
    
    // cluster function
    auto clusterLevels = [&](const std::vector<size_t>& idxs, const std::vector<double>& values, size_t maxLevels) {
      std::vector<double> cand;
      for (auto id : idxs) cand.push_back(values[id]);
      if (cand.empty()) return std::vector<double>{};
      std::sort(cand.begin(), cand.end());
      // simple clustering by greedy grouping within 2% proximity
      std::vector<double> levels;
      double clusterSum = cand[0]; size_t clusterCount = 1;
      for (size_t i = 1; i < cand.size(); ++i) {
        if (std::abs(cand[i] - cand[i-1]) / std::max(1e-9, cand[i-1]) < 0.02) {
          clusterSum += cand[i]; ++clusterCount;
        } else {
          levels.push_back(clusterSum / static_cast<double>(clusterCount));
          clusterSum = cand[i]; clusterCount = 1;
        }
      }
      levels.push_back(clusterSum / static_cast<double>(clusterCount));
      if (levels.size() > maxLevels) {
        // pick most dense by frequency (approx pick median ones)
        std::vector<double> outv;
        for (size_t k = 0; k < maxLevels; ++k) {
          size_t idx = (levels.size() * (k+1)) / (maxLevels+1);
          outv.push_back(levels[idx]);
        }
        return outv;
      }
      return levels;
    };
    
    auto resLevels = clusterLevels(peaks, highs, 4);
    auto supLevels = clusterLevels(troughs, lows, 4);
    
    for (double r : resLevels) {
      std::ostringstream ss; ss << "Resistance level ~ " << fmtDouble(r);
      out.push_back(MakeHit("Resistance", 0, n-1, 0.6, ss.str()));
    }
    for (double s : supLevels) {
      std::ostringstream ss; ss << "Support level ~ " << fmtDouble(s);
      out.push_back(MakeHit("Support", 0, n-1, 0.6, ss.str()));
    }
    return out;
  }
  
  // ---------------- Breakout events (SR breakout) ----------------
  std::vector<PatternHit> DetectBreakoutEvents(const std::vector<StockPoint>& history) {
    std::vector<PatternHit> out;
    size_t n = history.size(); if (n < 10) return out;
    auto highs = highsFrom(history); auto lows = lowsFrom(history); auto closes = closesFrom(history);
    // compute last N range and SR levels from previous window
    size_t lookback = std::min(n/2, static_cast<size_t>(30));
    size_t start = n - lookback - 1;
    auto sres = DetectSupportResistance(std::vector<StockPoint>(history.begin() + start, history.end()-1));
    // Check if last close breaks above/below those levels (with confirmation)
    double lastClose = closes.back();
    for (const auto &ph : sres) {
      if (ph.name == "Resistance") {
        // parse approximate level from rationale (we added it earlier)
        // fallback: use mean of prior highs
        double level = 0.0;
        // approximate: compute recent max high
        level = *std::max_element(highs.begin()+start, highs.begin()+start+lookback);
        if (lastClose > level * 1.002) {
          std::ostringstream ss; ss << "Breakout above resistance ~" << fmtDouble(level);
          out.push_back(MakeHit("BreakoutUp", start, n-1, 0.7, ss.str()));
        }
      } else if (ph.name == "Support") {
        double level = *std::min_element(lows.begin()+start, lows.begin()+start+lookback);
        if (lastClose < level * 0.998) {
          std::ostringstream ss; ss << "Breakdown below support ~" << fmtDouble(level);
          out.push_back(MakeHit("BreakoutDown", start, n-1, 0.7, ss.str()));
        }
      }
    }
    return out;
  }
  
  // ---------------- Harmonic-ish simple AB=CD detection ----------------
  std::vector<PatternHit> DetectHarmonicPatterns(const std::vector<StockPoint>& history) {
    // Full harmonic patterns require precise fib ratios and point labelling; here we'll detect simple AB=CD & 1.27/1.618 extension candidates
    std::vector<PatternHit> out;
    size_t n = history.size(); if (n < 12) return out;
    auto closes = closesFrom(history); auto highs = highsFrom(history); auto lows = lowsFrom(history);
    
    // generate pivot sequence of alternating peaks/troughs with radius 2
    std::vector<size_t> peakIdx = FindPivotPeaks(highs, 2);
    std::vector<size_t> troughIdx = FindPivotTroughs(lows, 2);
    // merge pivots into a sorted index list
    std::vector<size_t> piv;
    piv.reserve(peakIdx.size() + troughIdx.size());
    for (auto i : peakIdx) piv.push_back(i);
    for (auto i : troughIdx) piv.push_back(i);
    std::sort(piv.begin(), piv.end());
    // search for sequential 4-point patterns A-B-C-D where AB ~= CD
    for (size_t i = 0; i + 3 < piv.size(); ++i) {
      size_t A = piv[i], B = piv[i+1], C = piv[i+2], D = piv[i+3];
      if (!(A < B && B < C && C < D)) continue;
      double valA = closes[A], valB = closes[B], valC = closes[C], valD = closes[D];
      double ab = std::abs(valB - valA);
      double cd = std::abs(valD - valC);
      if (ab < 1e-9 || cd < 1e-9) continue;
      double ratio = cd / ab;
      if (std::abs(ratio - 1.0) < 0.15) {
        std::ostringstream ss; ss << "AB=CD approximated ratio=" << fmtDouble(ratio) << " points=" << A << "," << B << "," << C << "," << D;
        out.push_back(MakeHit("AB=CD", A, D, 0.7 * (1.0 - std::abs(ratio-1.0)), ss.str()));
      } else if (std::abs(ratio - 1.27) < 0.08 || std::abs(ratio - 1.618) < 0.08) {
        std::ostringstream ss; ss << "AB-CD Extension ratio=" << fmtDouble(ratio) << " points=" << A << "," << B << "," << C << "," << D;
        out.push_back(MakeHit("HarmonicExtension", A, D, 0.65 * (1.0 - std::abs(ratio-1.27)), ss.str()));
      }
    }
    return out;
  }
  
  // ---------------- Parabolic / Blow-off top detection ----------------
  std::vector<PatternHit> DetectParabolicMove(const std::vector<StockPoint>& history) {
    std::vector<PatternHit> out;
    size_t n = history.size(); if (n < 10) return out;
    auto closes = closesFrom(history);
    // detect consecutive accelerating returns: last K returns increasing and cumulative return large
    for (size_t window = 6; window <= std::min(n, static_cast<size_t>(20)); ++window) {
      size_t start = n - window;
      // compute returns
      std::vector<double> ret;
      for (size_t i = start+1; i < n; ++i) ret.push_back((closes[i] - closes[i-1]) / std::max(1e-9, closes[i-1]));
      bool accel = true;
      for (size_t i = 1; i < ret.size(); ++i) if (ret[i] <= ret[i-1]) { accel = false; break; }
      double cum = (closes.back() - closes[start]) / std::max(1e-9, closes[start]);
      if (accel && cum > 0.2) { // >20% in window with accelerating returns
        std::ostringstream ss; ss << "ParabolicMove last" << window << "bars cumReturn=" << fmtDouble(cum);
        out.push_back(MakeHit("ParabolicMove", start, n-1, 0.85, ss.str()));
      }
    }
    return out;
  }
  
  // ---------------- Adam & Eve (sharp vs rounded double bottom) ----------------
  std::vector<PatternHit> DetectAdamEvePatterns(const std::vector<StockPoint>& history) {
    std::vector<PatternHit> out;
    size_t n = history.size(); if (n < 20) return out;
    auto lows = lowsFrom(history); auto closes = closesFrom(history);
    // find double bottom candidates with left trough sharp (adam) right trough rounded (eve)
    auto troughs = FindPivotTroughs(lows, 2);
    if (troughs.size() < 2) return out;
    for (size_t i = 0; i + 1 < troughs.size(); ++i) {
      size_t left = troughs[i], right = troughs[i+1];
      if (right - left < 4 || right - left > 40) continue;
      // left trough depth narrow (sharp), right trough width larger (rounded)
      // sharpness: low surrounded by high neighbors -> small local width
      size_t leftWidth = 1;
      for (size_t k = left; k+1 < lows.size() && lows[k+1] >= lows[k]; ++k) leftWidth++;
      size_t rightWidth = 1;
      for (size_t k = right; k > 0 && lows[k-1] >= lows[k]; --k) rightWidth++;
      if (leftWidth <= 2 && rightWidth >= 4) {
        double strength = 0.75;
        std::ostringstream ss; ss << "Adam&Eve double bottom leftIdx=" << left << " rightIdx=" << right << " widths=" << leftWidth << "," << rightWidth;
        out.push_back(MakeHit("AdamEveDoubleBottom", left, right, strength, ss.str()));
      }
    }
    return out;
  }
  
  
  // ---------------- Main aggregator convenience ----------------
  // Returns a combined list of all supported chart patterns (deduplicated roughly)
  std::vector<PatternHit> DetectAllChartPatterns(const std::vector<StockPoint>& history) {
    std::vector<PatternHit> out;
    if (history.size() < 6) return out;
    auto append = [&](const std::vector<PatternHit>& v) {
      for (auto &p : v) out.push_back(p);
    };
    append(DetectDoubleTopBottom(history));
    append(DetectTripleTopBottom(history));
    append(DetectHeadAndShoulders(history));
    append(DetectRoundingBottom(history));
    append(DetectCupAndHandle(history));
    append(DetectWedges(history));
    append(DetectBroadeningFormation(history));
    append(DetectDiamondPatterns(history));
    append(DetectTrianglesFlags(history));
    append(DetectRectangleChannel(history));
    append(DetectMeasuredMove(history));
    append(DetectHarmonicPatterns(history));
    append(DetectParabolicMove(history));
    append(DetectAdamEvePatterns(history));
    // breakout/levels/trend
    auto sr = DetectSupportResistance(history); append(sr);
    auto bo = DetectBreakoutEvents(history); append(bo);
    auto trend = DetectTrendStructure(history); out.push_back(trend);
    return out;
  }
  
} // namespace KanVest::Analysis::ChartPatterns
