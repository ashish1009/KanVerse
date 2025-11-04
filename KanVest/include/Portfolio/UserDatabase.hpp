//
//  UserDatabase.hpp
//  KanVest
//
//  Created by Ashish . on 04/11/25.
//

#pragma once

#include "Portfolio/UserProfile.hpp"

namespace KanVest {
  
  /// Database for all user profiles (loaded from YAML)
  class UserDatabase
  {
  public:
    /// Loads all users from YAML file
    static bool LoadDatabase(const std::string& filePath);
    
    /// Saves all users back to YAML file
    static bool SaveDatabase(const std::string& filePath);
    
    /// Returns a const reference to a user (throws if not found)
    static const UserProfile& GetUser(const std::string& username);
    
    /// Adds a new user (and updates the database)
    static void AddUser(const UserProfile& user, const std::string& filePath);
    
    /// Checks if a user exists
    static bool HasUser(const std::string& username);
    
    /// Returns all users (read-only)
    static const std::unordered_map<std::string, UserProfile>& GetAllUsers();
    
  private:
    inline static std::unordered_map<std::string, UserProfile> s_userProfileMap;
  };
  
  /// Handles YAML serialization/deserialization of UserDatabase
  class UserDatabaseSerializer
  {
  public:
    static bool SaveToYAML(const std::unordered_map<std::string, UserProfile>& map,
                           const std::string& filePath);
    
    static bool LoadFromYAML(std::unordered_map<std::string, UserProfile>& map,
                             const std::string& filePath);
  };
  
} // namespace KanVest
