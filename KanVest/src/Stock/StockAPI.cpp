//
//  StockAPI.cpp
//  KanVest
//
//  Created by Ashish . on 08/01/26.
//

#include "StockAPI.hpp"

namespace KanVest
{
  static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output)
  {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
  }
  
  std::string StockAPI::FetchLiveData(const std::string& symbolName, Range range, Interval interval)
  {
    CURL* curl = curl_easy_init();
    std::string response;
    if (curl)
    {
      std::string url = API_Provider::GetURL() + symbolName;
      url += "?interval=" + API_Provider::GetIntervalStringFromEnum(interval);
      url += "&range=" + API_Provider::GetRangeStringFromEnum(range);
      
      std::cout << url << "\n";
      
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
      curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
      curl_easy_perform(curl);
      curl_easy_cleanup(curl);
    }
    return response;
  }
} // namespace KanVest
