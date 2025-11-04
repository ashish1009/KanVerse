//
//  UserDatabase.hpp
//  KanVest
//
//  Created by Ashish . on 04/11/25.
//

#pragma once

#include "Portfolio/User.hpp"

namespace KanVest {
  
  /// Database for all user profiles (loaded from YAML)
  class UserManager
  {
  public:
    /// This function sets the data base file path
    /// - Parameter filePath: new file path
    static void SetDatabaseFilePath(const std::filesystem::path& filePath);
    
    /// This function returns the file path
    static const std::filesystem::path& GetDatabaseFilePath();
    
    /// Loads all users from YAML file
    static bool LoadDatabase();
    
    /// Saves all users back to YAML file
    static bool SaveDatabase();
    
    /// Returns a const reference to a user (throws if not found)
    static const User& GetUser(const std::string& username);
    
    /// Adds a new user (and updates the database)
    static void AddUser(const User& user);
    
    /// Checks if a user exists
    static bool HasUser(const std::string& username);

    /// This function updates current user
    static void SetCurrentUser(const User& user);

    /// This function returns current user
    static User& GetCurrentUser();

    /// Returns all users (read-only)
    static const std::unordered_map<std::string, User>& GetAllUsers();
    
  private:
    inline static std::unordered_map<std::string, User> s_userProfileMap;
    inline static User s_currentUser;
    inline static std::filesystem::path s_databaseFilePath;
  };
  
  /// Handles YAML serialization/deserialization of UserDatabase
  class UserDatabaseSerializer
  {
  public:
    static bool SaveToYAML(const std::unordered_map<std::string, User>& map,
                           const std::string& filePath);
    
    static bool LoadFromYAML(std::unordered_map<std::string, User>& map,
                             const std::string& filePath);
  };
  
} // namespace KanVest
