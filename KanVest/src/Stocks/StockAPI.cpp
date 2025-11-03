//
//  StockAPI.cpp
//  KanVest
//
//  Created by Ashish . on 31/10/25.
//

#include "StockAPI.hpp"

#include "Stocks/API_Provider.hpp"

namespace KanVest
{
  static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output)
  {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
  }

  std::string StockAPI::FetchLiveData(const std::string& symbolName, const std::string& interval, const std::string& range)
  {
    CURL* curl = curl_easy_init();
    std::string response;
    if (curl)
    {
      std::string url = API_Provider::GetURL() + symbolName + "?interval=" + interval + "&range=" + range;
      std::cout << "URL : " << url << "\n";
      
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
  
  std::string StockAPI::FetchHistoryData(const std::string& symbolName, const std::string& period1, const std::string& period2)
  {
    CURL* curl = curl_easy_init();
    std::string response;
    if (curl)
    {
      std::string url = API_Provider::GetURL() + symbolName + "?period1=" + period1 + "&period2=" + period2 + "&interval=1d";
      
      std::cout << url;
      
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
