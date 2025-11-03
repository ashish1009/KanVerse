//
//  StockManager.hpp
//  KanVest
//
//  Created by Ashish . on 02/11/25.
//

#pragma once

#include "Stocks/StockData.hpp"

namespace KanVest
{
  inline static const char* ValidIntervals[] =
  {
    "1m", "2m", "5m", "15m", "30m", "60m", "90m", "1h", "1d", "5d", "1wk", "1mo", "3mo"
  };
  
  inline static const char* ValidRange[] =
  {
    "1d", "5d", "1mo", "6mo", "ytd", "1y", "5y", "max"
  };
  
  inline static std::unordered_map<std::string, std::vector<std::string>> RangeIntervalMap =
  {
    {"1d",  {"1m", "2m", "5m", "15m", "30m", "60m"}},
    {"5d",  {"1m", "2m", "5m", "15m", "30m", "60m"}},
    {"1mo", {"5m", "15m", "30m", "60m", "1d"}},
    {"6mo", {"1h", "1d", "1wk"}},
    {"ytd", {"1h", "1wk", "1mo"}},
    {"1y",  {"1d", "1wk", "1mo"}},
    {"5y",  {"1d", "1wk", "1mo"}},
    {"max", {"1d", "1wk", "1mo"}},
  };

  /// This class Manager all stocks
  class StockManager
  {
  public:
    static void SetActiveStockData(const StockData& stockData);

    static void SetCurrentInterval(const char* interval);
    static void SetCurrentRange(const char* range);

    static const char* GetCurrentInterval();
    static const char* GetCurrentRange();
    
    static const StockData& GetActiveStockData();
    
  private:
    inline static const char* s_currentInterval = "1m";
    inline static const char* s_currentRange = "1d";
    
    inline static StockData s_activeStockData = {"Nifty"};
  };
} // namespace KanVest
