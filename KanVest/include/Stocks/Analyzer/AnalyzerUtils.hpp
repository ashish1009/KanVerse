//
//  AnalyzerUtils.hpp
//  KanVest
//
//  Created by Ashish . on 07/11/25.
//

#pragma once

namespace KanVest::Analysis::Utils
{
  template<typename T> T mean(const std::vector<T>& v);
  template<typename T> std::optional<double> rolling_mean_last(const std::vector<T>& v, size_t n);
  inline bool approx_equal(double a, double b, double relTol = 1e-6);
} // namespace KanVest::Analysis::Utils

