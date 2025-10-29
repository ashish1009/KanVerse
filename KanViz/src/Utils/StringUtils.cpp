//
//  StringUtils.cpp
//  KanViz
//
//  Created by Ashish . on 21/10/24.
//

#include "StringUtils.hpp"

namespace KanViz::Utils::String
{
  /// Splits the input string using the specified delimiter
  /// - Parameters:
  ///   - string: The full string to be split
  ///   - delimiter: The character(s) to split the string on
  /// - Returns: A vector of substrings split by the delimiter
  std::vector<std::string> SplitString(const std::string& string, std::string_view delimiter)
  {
    IK_PROFILE();
    std::vector<std::string> result;
    
    // Return immediately if delimiter is empty
    if (delimiter.empty())
    {
      return result;
    }
    
    size_t start = 0;
    size_t end;
    
    // Split based on delimiter
    while ((end = string.find_first_of(delimiter, start)) != std::string::npos)
    {
      if (end > start)
      {
        result.emplace_back(string.substr(start, end - start));
      }
      start = end + 1;
    }
    
    // Add remaining string after last delimiter
    if (start < string.length())
    {
      result.emplace_back(string.substr(start));
    }
    
    return result;
  }
  
  /// Splits string using a single character as delimiter
  std::vector<std::string> SplitString(const std::string& string, const char delimiter)
  {
    return SplitString(string, std::string(1, delimiter));
  }
  
  /// Splits string using a C-string as delimiter
  std::vector<std::string> SplitString(const std::string& string, const char* delimiter)
  {
    return SplitString(string, std::string(delimiter));
  }
  
  /// Splits string based on whitespace and newlines
  std::vector<std::string> Tokenize(const std::string& string)
  {
    return SplitString(string, " \t\n");
  }
  
  /// Splits string into lines based on newline character
  std::vector<std::string> GetLines(const std::string& string)
  {
    return SplitString(string, "\n");
  }
  
  /// Reads an entire file from disk and returns its contents as a string
  std::string ReadFromFile(const std::filesystem::path& filepath)
  {
    IK_PROFILE();
    
    std::string result {};
    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    
    // Assert if file cannot be opened
    if (!in)
    {
      assert(false);
    }
    
    // Seek to end and determine file size
    in.seekg(0, std::ios::end);
    size_t size = static_cast<size_t>(in.tellg());
    
    // Assert if size is invalid
    if (size == static_cast<size_t>(-1))
    {
      assert(false);
    }
    
    // Read file contents
    result.resize(size);
    in.seekg(0, std::ios::beg);
    in.read(result.data(), static_cast<std::streamsize>(size));
    in.close();
    
    return result;
  }
  
  /// Finds a token in a string if it's a whole word (not part of another word)
  const char* FindToken(std::string_view str, std::string_view token)
  {
    IK_PROFILE();
    const char* t = str.data();
    
    while ((t = strstr(t, token.data())))
    {
      // Check if left side is at start or valid separator
      bool left = (t == str.data()) || std::isspace(t[-1]) || t[-1] == '/';
      // Check if right side ends or has a separator
      bool right = !t[token.size()] || std::isspace(t[token.size()]);
      
      if (left && right)
      {
        return t;
      }
      
      t += token.size();
    }
    
    return nullptr;
  }
  
  /// Extracts a code block from a string (delimited by braces)
  /// - Parameter outPosition: updated to point at closing brace
  std::string GetBlock(std::string_view str, const char** outPosition)
  {
    IK_PROFILE();
    const char* end = strstr(str.data(), "}");
    
    if (!end)
    {
      return str.data();
    }
    
    if (outPosition)
    {
      *outPosition = end;
    }
    
    uint32_t length = static_cast<uint32_t>(end - str.data() + 1);
    return std::string(str.data(), length);
  }
  
  /// Extracts a single statement (ending in ';') from string
  std::string GetStatement(std::string_view str, const char** outPosition)
  {
    IK_PROFILE();
    const char* end = strstr(str.data(), ";");
    
    if (!end)
    {
      return str.data();
    }
    
    if (outPosition)
    {
      *outPosition = end;
    }
    
    uint32_t length = static_cast<uint32_t>(end - str.data() + 1);
    return std::string(str.data(), length);
  }
  
  /// Converts a string to lowercase
  std::string ToLower(const std::string& string)
  {
    std::string result;
    result.reserve(string.size());
    
    for (const char c : string)
    {
      result += static_cast<char>(std::tolower(c));
    }
    
    return result;
  }
  
  /// Converts a string_view to lowercase string
  std::string ToLowerCopy(const std::string_view& string)
  {
    std::string result(string);
    return ToLower(result);
  }
  
  /// Converts a string to uppercase
  std::string ToUpper(const std::string_view& string)
  {
    std::string result;
    result.reserve(string.size());
    
    for (const char c : string)
    {
      result += static_cast<char>(std::toupper(c));
    }
    
    return result;
  }
  
  /// Removes the extension from a file path (e.g. ".png")
  std::string RemoveExtension(const std::filesystem::path& filePath)
  {
    if (filePath.has_extension())
    {
      std::string str = filePath.string();
      return str.substr(0, str.find_last_of('.'));
    }
    return filePath.string();
  }
  
} // namespace KanViz::Utils::String
