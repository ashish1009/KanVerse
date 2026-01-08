//
//  StockManager.hpp
//  KanVest
//
//  Created by Ashish . on 08/01/26.
//

#pragma once

#include "Stock/StockMetadata.hpp"

#include "URL_API/API_Provider.hpp"

namespace KanVest
{
  /// This class managers stocks data
  class StockManager
  {
  public:
    /// This function intializes the stock manager data
    static void Initialize(int milliseconds = 10);
    /// This function shuts down the stock manager data
    static void Shutdown();
  };
} // namespace KanVest
