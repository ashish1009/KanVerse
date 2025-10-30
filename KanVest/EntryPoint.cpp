//
//  EntryPoint.cpp
//  KanVest
//
//  Created by Ashish . on 28/10/25.
//

#if 1
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <curl/curl.h>
#include <algorithm>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
  size_t totalSize = size * nmemb;
  output->append((char*)contents, totalSize);
  return totalSize;
}

std::string fetchURL(const std::string& url) {
  CURL* curl = curl_easy_init();
  std::string response;
  if (curl) {
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

double extractValue(const std::string& text, const std::string& key) {
  std::string patternStr = "\"" + key + "\"\\s*:\\s*([-+]?[0-9]*\\.?[0-9]+)";
  std::regex pattern(patternStr);
  std::smatch match;
  if (std::regex_search(text, match, pattern))
    return std::stod(match[1].str());
  return -1;
}

std::string extractString(const std::string& text, const std::string& key) {
  std::string patternStr = "\"" + key + "\"\\s*:\\s*\"([^\"]+)\"";
  std::regex pattern(patternStr);
  std::smatch match;
  if (std::regex_search(text, match, pattern))
    return match[1].str();
  return "";
}

std::vector<double> extractArray(const std::string& text, const std::string& key) {
  std::vector<double> values;
  std::string patternStr = "\"" + key + "\"\\s*:\\s*\\[([^\\]]+)\\]";
  std::regex pattern(patternStr);
  std::smatch match;
  if (std::regex_search(text, match, pattern)) {
    std::string arr = match[1].str();
    std::regex numPattern("([-+]?[0-9]*\\.?[0-9]+)");
    for (std::sregex_iterator it(arr.begin(), arr.end(), numPattern), end; it != end; ++it)
      values.push_back(std::stod((*it)[1].str()));
  }
  return values;
}

int main() {
  std::string symbol = "BEL.NS";
  
  // --- Live data ---
  std::string liveURL = "https://query1.finance.yahoo.com/v8/finance/chart/" + symbol;
  std::string liveData = fetchURL(liveURL);
  
  double price = extractValue(liveData, "regularMarketPrice");
  double high52 = extractValue(liveData, "fiftyTwoWeekHigh");
  double low52 = extractValue(liveData, "fiftyTwoWeekLow");
  double volume = extractValue(liveData, "regularMarketVolume");
  std::string currency = extractString(liveData, "currency");
  
  std::cout << "=== LIVE DATA ===\n";
  std::cout << "Symbol: " << symbol << "\n";
  std::cout << "Current Price: " << price << " " << currency << "\n";
  std::cout << "Volume: " << volume << "\n";
  std::cout << "52W High: " << high52 << " | 52W Low: " << low52 << "\n\n";
  
  // --- Custom date range (example: Jan 2024 - Mar 2024) ---
  long period1 = 1704067200; // 1 Jan 2024
  long period2 = 1711843200; // 31 Mar 2024
  std::string histURL =
  "https://query1.finance.yahoo.com/v8/finance/chart/" + symbol +
  "?period1=" + std::to_string(period1) +
  "&period2=" + std::to_string(period2) +
  "&interval=1d";
  
  std::string histData = fetchURL(histURL);
  std::vector<double> closes = extractArray(histData, "close");
  
  if (!closes.empty()) {
    double customHigh = *std::max_element(closes.begin(), closes.end());
    double customLow  = *std::min_element(closes.begin(), closes.end());
    std::cout << "=== CUSTOM RANGE (Jan–Mar 2024) ===\n";
    std::cout << "High: " << customHigh << " | Low: " << customLow << "\n";
  } else {
    std::cout << "No historical data available for custom range.\n";
  }
  
  return 0;
}


#else
namespace KanVest
{
  // External function to create application
  /// Externam function to create application
  /// - Parameters:
  ///   - argc: Number of arguments passed from binary
  ///   - argv: Arguments ...
  extern KanViz::Scope<KanViz::Application> CreateApplication(std::span<const char*> args);
} // namespace KanVest

[[nodiscard]] int ExecuteKanVest(std::span<const char*> args)
{
  if (!KanViz::CoreEngine::Initialize())
  {
    std::cerr << "Failed to initialize the KanViz engine.\n";
    return EXIT_FAILURE;
  }
     
  if (KanViz::Scope<KanViz::Application> application = KanVest::CreateApplication(args); application)
  {
    application->Run();
  }
  else
  {
    std::cerr << "Failed to create KanViz application.\n";
    return IK_FAILURE;
  }

  if (!KanViz::CoreEngine::Shutdown())
  {
    std::cerr << "Failed to shutdown the KanViz engine.\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/// This function is the Entry point of the Studio
/// - Parameters:
///   - argc: Number of arguments passed from binary
///   - argv: Arguments ...
[[nodiscard]] int main(int argc, const char* argv[])
{
  std::cout << std::format("  Executing : {}\n", argv[0]);
  
  std::span args(argv, argv + argc);
  for (size_t i = 1; i < args.size(); ++i)
  {
    std::cout << std::format("    Arg[{}]   : {}\n", i, args[i]);
  }
  
  return ExecuteKanVest(args);
}
#endif
