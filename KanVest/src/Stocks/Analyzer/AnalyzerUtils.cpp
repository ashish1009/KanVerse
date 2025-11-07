//
//  AnalyzerUtils.cpp
//  KanVest
//
//  Created by Ashish . on 07/11/25.
//

#include "AnalyzerUtils.hpp"

namespace KanVest::Analysis::Utils
{
  template<typename T> T mean(const std::vector<T>& v)
  {
    if (v.empty()) return T(0);
    T s = std::accumulate(v.begin(), v.end(), T(0));
    return s / static_cast<T>(v.size());
  }

  template<typename T> std::optional<double> rolling_mean_last(const std::vector<T>& v, size_t n)
  {
    if (v.size() < n || n == 0) return std::nullopt;
    double s = 0.0;
    for (size_t i = v.size()-n; i < v.size(); ++i) s += static_cast<double>(v[i]);
    return s / static_cast<double>(n);
  }

  inline bool approx_equal(double a, double b, double relTol)
  {
    return std::fabs(a-b) <= relTol * std::max(1.0, std::max(std::fabs(a), std::fabs(b)));
  }
} // namespace KanVest::Analysis::Utils
