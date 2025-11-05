//
//  UserManager.cpp
//  KanVest
//
//  Created by Ashish . on 05/11/25.
//

#include "UserManager.hpp"

namespace KanVest
{
  void UserManager::SetDatabaseFilePath(const std::filesystem::path& databaseDirectory)
  {
    s_databaseDirectory = databaseDirectory;
    if (!exists(s_databaseDirectory))
    {
      std::filesystem::create_directory(s_databaseDirectory);
    }
    
    s_databaseFilePath = s_databaseDirectory / "UserDatabase.yaml";
    if (exists(s_databaseFilePath))
    {
      UserManager::LoadDatabase();
    }
    else
    {
      UserManager::SaveDatabase();
    }

  }
  const std::filesystem::path& UserManager::GetDatabaseFilePath()
  {
    return s_databaseFilePath;
  }
  
  bool UserManager::LoadDatabase()
  {
    s_userProfileMap.clear();
    return UserDatabaseSerializer::LoadFromYAML(s_userProfileMap, s_databaseFilePath);
  }
  
  bool UserManager::SaveDatabase()
  {
    return UserDatabaseSerializer::SaveToYAML(s_userProfileMap, s_databaseFilePath);
  }
  
  const User& UserManager::GetUser(const std::string& username)
  {
    auto it = s_userProfileMap.find(username);
    if (it == s_userProfileMap.end())
    {
      throw std::runtime_error("User not found: " + username);
    }
    return it->second;
  }
  
  void UserManager::AddUser(const User& user)
  {
    s_userProfileMap[user.username] = user;
    SaveDatabase(); // update YAML whenever new user is added
  }
  
  bool UserManager::HasUser(const std::string& username)
  {
    return s_userProfileMap.find(username) != s_userProfileMap.end();
  }
  
  void UserManager::SetCurrentUser(const User& user)
  {
    s_currentUser = user;
  }
  
  User& UserManager::GetCurrentUser()
  {
    return s_currentUser;
  }
  
  const std::unordered_map<std::string, User>& UserManager::GetAllUsers()
  {
    return s_userProfileMap;
  }
  
  bool UserDatabaseSerializer::SaveToYAML(
                                          const std::unordered_map<std::string, User>& map,
                                          const std::string& filePath)
  {
    YAML::Emitter out;
    out << YAML::BeginSeq;
    
    for (const auto& [name, profile] : map)
    {
      out << YAML::BeginMap;
      out << YAML::Key << "username" << YAML::Value << profile.username;
      out << YAML::Key << "passwordHash" << YAML::Value << profile.passwordHash;
      out << YAML::Key << "portfolioPath" << YAML::Value << profile.portfolioPath;
      out << YAML::Key << "lastLoginTime" << YAML::Value << profile.lastLoginTime;
      out << YAML::EndMap;
    }
    
    out << YAML::EndSeq;
    
    std::ofstream fout(filePath);
    if (!fout.is_open())
      return false;
    
    fout << out.c_str();
    return true;
  }
  
  bool UserDatabaseSerializer::LoadFromYAML(
                                            std::unordered_map<std::string, User>& map,
                                            const std::string& filePath)
  {
    std::ifstream fin(filePath);
    if (!fin.is_open())
      return false;
    
    YAML::Node node = YAML::LoadFile(filePath);
    if (!node.IsSequence())
      return false;
    
    for (const auto& n : node)
    {
      if (!n["username"] || !n["passwordHash"])
        continue;
      
      User profile;
      profile.username = n["username"].as<std::string>();
      profile.passwordHash = n["passwordHash"].as<std::string>();
      profile.portfolioPath = n["portfolioPath"] ? n["portfolioPath"].as<std::string>() : "";
      profile.lastLoginTime = n["lastLoginTime"] ? n["lastLoginTime"].as<std::string>() : "";
      
      map[profile.username] = profile;
    }
    
    return true;
  }

  bool UserManager::HandleLogin(const std::string& username, const std::string& password, std::string& loginMessage)
  {
    // Ensure database is loaded (do this once at startup)
    if (!LoadDatabase())
    {
      loginMessage = "User database not found";
      return false;
    }
    
    // Empty user name
    if (username == "")
    {
      loginMessage = "Username can not be empty";
      return false;
    }
    
    // Step 1: Check if user exists
    if (UserManager::HasUser(username))
    {
      // Password Empty
      if (password == "")
      {
        loginMessage = "Password can not be empty";
        return false;
      }
      
      // Get the existing profile
      const auto& user = UserManager::GetUser(username);
      
      // Verify password
      if (user.VerifyPassword(password))
      {
        UserManager::SetCurrentUser(user);
        
        auto& currentUser = UserManager::GetCurrentUser();
        currentUser.LoadPortfolio();
        
        return true;
      }
      else
      {
        loginMessage = "Invalid password !!!";
        return false;
      }
    }
    else
    {
      loginMessage = "User not found. Sign-up to create new account";
      return false;
    }
    return true;
  }
  
  bool UserManager::HandleSignUp(const std::string& username, const std::string& password, std::string& signUpMessage)
  {
    // Ensure database is loaded (do this once at startup)
    if (!LoadDatabase())
    {
      signUpMessage = "User database not found";
      return false;
    }
    
    // Empty Username
    if (username == "")
    {
      signUpMessage = "Username can not be empty";
      return false;
    }
    
    // Password Username
    if (password == "")
    {
      signUpMessage = "Password can not be empty";
      return false;
    }
    
    // Step 1: Check if user exists
    if (UserManager::HasUser(username))
    {
      signUpMessage = "User already exists.";
      return false;
    }
    else
    {
      User newUser(username, "");
      newUser.SetPassword(password);
      
      std::filesystem::path portfolioDirectory =  s_databaseDirectory / "Portfolios";
      std::string portfolioFilePath =  username + "_portfolio.yaml";
      if (!std::filesystem::exists(portfolioDirectory))
      {
        std::filesystem::create_directory(portfolioDirectory);
      }
      
      newUser.portfolioPath = portfolioDirectory / portfolioFilePath;
      
      UserManager::AddUser(newUser);
    }
    
    return true;
  }

} // namespace KanVest
