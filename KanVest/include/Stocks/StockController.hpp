//
//  StockController.hpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#pragma once

#include "Stocks/StockData.hpp"
#include "Stocks/StockSummary.hpp"

namespace KanVest
{            
  /// This class manages stock data updation
  class StockController
  {
  public:
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

    /// This function fetches real time data for stock and returns the stock data
    /// - Parameter symbolName: symbol name
    /// - Parameter interval: intervel of data
    /// - Parameter range: range of data
    static StockData UpdateStockData(const std::string& symbolName, const std::string& interval, const std::string& range);
    
    /// This function analyze the stock
   static StockSummary AnalyzeStock();

    /// This function update stock data
    /// - Parameter stockData: stock data
    static void SetActiveStockData(const StockData& stockData);
    /// This function returns the stock data
    static const StockData& GetActiveStockData();

    /// This function update refersh time
    /// - Parameter refreshInterval: time interval
    static void SetRefreshInterval(float refreshInterval);
    /// This function returns the update refresh time
    static float GetRefreshInterval();
    
    /// This function update url time itnerval
    /// - Parameter interval:  url range
    static void SetCurrentInterval(const char* interval);
    /// This function returns the update refresh time
    static const char* GetCurrentInterval();

    /// This function update url range
    /// - Parameter range: new range
    static void SetCurrentRange(const char* range);
    /// This function returns the url range
    static const char* GetCurrentRange();
    
  private:
    inline static float s_refreshInterval = 1.0f;
    
    inline static const char* s_currentInterval = "1d";
    inline static const char* s_currentRange = "1mo";
    
    inline static StockData s_activeStockData = {"Nifty"};
  };
} // namespace KanVest
