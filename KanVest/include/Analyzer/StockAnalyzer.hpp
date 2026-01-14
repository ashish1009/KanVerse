//
//  StockAnalyzer.hpp
//  KanVest
//
//  Created by Ashish . on 14/01/26.
//

#pragma once

#include "Stock/StockMetadata.hpp"

namespace KanVest
{
  enum class TechnicalIndicators {DMA};

  struct StockReport
  {
    double score = 50.0;
    std::unordered_map<TechnicalIndicators, std::string> summary;
  };

  class Analyzer
  {
  public:
    /// This function analyze stock from data
    /// - Parameter stockData: stock data
    static void AnalzeStock(const StockData& stockData);

    static const StockReport& GetReport();

  private:
    inline static StockReport s_stockReport;
  };
} // namespace KanVest
