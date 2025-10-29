//
//  FileSystemUtils.cpp
//  KanViz
//
//  Created by Ashish . on 21/10/24.
//

#include "FileSystemUtils.hpp"

#include "Utils/StringUtils.hpp"

namespace KanViz::Utils::FileSystem
{
  /// This function returns the absolute (canonical) path of a given path
  /// - Parameter path: input relative or absolute path
  /// - Returns: Canonical absolute path, or empty if invalid
  std::filesystem::path Absolute(const std::filesystem::path& path)
  {
    // Return empty path if the input path is empty
    if (path.empty())
    {
      return {};
    }
    
    try
    {
      // Resolve and return the canonical (absolute) path
      return std::filesystem::canonical(path);
    }
    catch (const std::filesystem::filesystem_error&)
    {
      // Return empty if the path is invalid or file doesn't exist
      return {};
    }
  }
  
  /// This function copies a file or directory to a new destination
  /// - Parameters:
  ///   - oldFilepath: source path
  ///   - newFilepath: destination path
  /// - Returns: true if copy was successful, false if destination exists
  bool Copy(const std::filesystem::path& oldFilepath, const std::filesystem::path& newFilepath)
  {
    IK_PROFILE();
    
    if (std::filesystem::exists(newFilepath))
    {
      return false;
    }
    
    std::error_code ec;
    std::filesystem::copy(oldFilepath, newFilepath,
                          std::filesystem::copy_options::recursive,
                          ec);
    return !ec;
  }
  
  /// This function renames a file or directory (alias for Move)
  /// - Returns: true if rename was successful
  bool Rename(const std::filesystem::path& oldFilepath, const std::filesystem::path& newFilepath)
  {
    IK_PROFILE();
    return Move(oldFilepath, newFilepath);
  }
  
  /// This function moves a file or directory to a new destination
  /// - Returns: true if move was successful, false if destination exists
  bool Move(const std::filesystem::path& oldFilepath, const std::filesystem::path& newFilepath)
  {
    IK_PROFILE();
    
    if (std::filesystem::exists(newFilepath))
    {
      return false;
    }
    
    std::error_code ec;
    std::filesystem::rename(oldFilepath, newFilepath, ec);
    return !ec;
  }
  
  /// This function moves a file to a destination directory, preserving its filename
  /// - Returns: true if move was successful
  bool MoveFile(const std::filesystem::path& filepath, const std::filesystem::path& dest)
  {
    IK_PROFILE();
    return Move(filepath, dest / filepath.filename());
  }
  
  /// This function deletes a file or directory
  /// - Returns: true if deletion was successful
  bool Delete(const std::filesystem::path& filepath)
  {
    IK_PROFILE();
    
    if (!std::filesystem::exists(filepath))
    {
      return false;
    }
    
    std::error_code ec;
    if (std::filesystem::is_directory(filepath))
    {
      return std::filesystem::remove_all(filepath, ec) > 0 && !ec;
    }
    
    return std::filesystem::remove(filepath, ec) && !ec;
  }
} // namespace KanViz::Utils::FileSystem
