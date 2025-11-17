//
//  StockParser.cpp
//  KanVest
//
//  Created by Ashish . on 17/11/25.
//

#include "StockParser.hpp"

#include "URL_API/APIProvider.hpp"

namespace KanVest
{
  double StockParser::ExtractValue(const std::string& text, const std::string& key)
  {
    std::string patternStr = API_Provider::GetValueParserPattern(key);
    std::regex pattern(patternStr);
    std::smatch match;
    
    if (std::regex_search(text, match, pattern))
    {
      return std::stod(match[1].str());
    }
    
    return -1;
  }
  
  std::string StockParser::ExtractString(const std::string& text, const std::string& key)
  {
    std::string patternStr = API_Provider::GetStringParserPattern(key);
    std::regex pattern(patternStr);
    std::smatch match;
    
    if (std::regex_search(text, match, pattern))
    {
      return match[1].str();
    }
    
    return "";
  }
  
  std::vector<double> StockParser::ExtractArray(const std::string& text, const std::string& key)
  {
    std::vector<double> values;
    std::string patternStr = API_Provider::GetArrayParserPattern(key);
    std::regex pattern(patternStr);
    std::smatch match;
    if (std::regex_search(text, match, pattern))
    {
      std::string arr = match[1].str();
      std::regex numPattern("([-+]?[0-9]*\\.?[0-9]+)");
      for (std::sregex_iterator it(arr.begin(), arr.end(), numPattern), end; it != end; ++it)
      {
        values.push_back(std::stod((*it)[1].str()));
      }
    }
    return values;
  }
  
  
  time_t StockParser::ParseDateYYYYMMDD(const std::string &timeString)
  {
    // Accepts "YYYY-MM-DD", returns time_t for 00:00:00 local time on that date
    std::tm tm = {};
    if (timeString.size() != 10)
    {
      return 0;
    }
    
    std::istringstream iss(timeString);
    iss >> std::get_time(&tm, "%Y-%m-%d");
    if (iss.fail())
    {
      return 0;
    }
    
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    
    // mktime assumes localtime. Yahoo wants UTC timestamps, but using local is usually fine for daily ranges.
    return mktime(&tm);
  }
  
} // namespace KanVest
