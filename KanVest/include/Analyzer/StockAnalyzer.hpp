//
//  StockAnalyzer.hpp
//  KanVest
//
//  Created by Ashish . on 14/01/26.
//

#pragma once

#include "Stock/StockMetadata.hpp"

#include "Analyzer/Indicators/MovingAverage.hpp"
#include "Analyzer/Indicators/Momentum.hpp"

namespace KanVest
{
  enum class TechnicalIndicators {DMA, EMA};

  struct StockReport
  {
    double score = 50.0;
    std::unordered_map<TechnicalIndicators, std::vector<std::pair<ImU32, std::string>>> summary;
  };

  class Analyzer
  {
  public:
    /// This function analyze stock from data
    /// - Parameter stockData: stock data
    static void AnalzeStock(const StockData& stockData);

    static const StockReport& GetReport();
    
    static const std::map<int, std::vector<double>>& GetDMAValues();
    static const std::map<int, std::vector<double>>& GetEMAValues();

    static const RSISeries& GetRSI();

  private:
    inline static StockReport s_stockReport;

    inline static MAResult s_maResults;
    inline static RSISeries s_rsiSeries;
  };
} // namespace KanVest
