//
//  FileSystemUtils.hpp
//  KanViz
//
//  Created by Ashish . on 21/10/24.
//

#pragma once

namespace KanViz::Utils::FileSystem
{
  /// This function returns the absolute File/Directory path for KanViz
  /// - Parameter path: Current Relative File/Directory path passed
  std::filesystem::path Absolute(const std::filesystem::path& path);
  /// This function Copies the File/Directory
  /// - Parameters:
  ///   - oldFilepath: Old Name of File/Directory
  ///   - newFilepath: New Name of File/Directory
  bool Copy(const std::filesystem::path& oldFilepath, const std::filesystem::path& newFilepath);
  /// This function rename the File/Folder
  /// - Parameters:
  ///   - oldName: Old Name of File/Folder
  ///   - newName: New Name of File/Folder
  bool Rename(const std::filesystem::path& oldFilepath, const std::filesystem::path& newFilepath);
  /// This function Moves the File/Directory
  /// - Parameters:
  ///   - oldFilepath: Old Name of File/Directory
  ///   - newFilepath: New Name of File/Directory
  bool Move(const std::filesystem::path& oldFilepath, const std::filesystem::path& newFilepath);
  /// This function moves the file
  /// - Parameters:
  ///   - filepath: source
  ///   - dest: destination
  bool MoveFile(const std::filesystem::path& filepath, const std::filesystem::path& dest);
  /// This function deletes the File/Directory
  /// - Parameter filepath: File/Directory path to be deleted
  bool Delete(const std::filesystem::path& filepath);
} // namespace KanViz::Utils::FileSystem
