//
//  UserProfile.cpp
//  KanVest
//
//  Created by Ashish . on 04/11/25.
//

#include "UserProfile.hpp"

namespace KanVest
{
  // Simple password hasher using std::hash (replace with real hash later)
  static std::string HashPassword(const std::string& password)
  {
    std::hash<std::string> hasher;
    return std::to_string(hasher(password));
  }
  
  UserProfile::UserProfile(const std::string& user, const std::string& passHash)
  : username(user), passwordHash(passHash)
  {
    // Record login time
    std::time_t now = std::time(nullptr);
    lastLoginTime = std::asctime(std::localtime(&now));
  }
  
  bool UserProfile::VerifyPassword(const std::string& password) const
  {
    return passwordHash == HashPassword(password);
  }
  
  void UserProfile::SetPassword(const std::string& newPassword)
  {
    passwordHash = HashPassword(newPassword);
  }
    
  bool UserSerializer::SaveToYAML(const UserProfile& profile, const std::string& filePath)
  {
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "username" << YAML::Value << profile.username;
    out << YAML::Key << "passwordHash" << YAML::Value << profile.passwordHash;
    out << YAML::Key << "portfolioPath" << YAML::Value << profile.portfolioPath;
    out << YAML::Key << "lastLoginTime" << YAML::Value << profile.lastLoginTime;
    out << YAML::EndMap;
    
    std::ofstream fout(filePath);
    if (!fout.is_open())
      return false;
    
    fout << out.c_str();
    return true;
  }
  
  bool UserSerializer::LoadFromYAML(UserProfile& profile, const std::string& filePath)
  {
    std::ifstream fin(filePath);
    if (!fin.is_open())
      return false;
    
    YAML::Node node = YAML::LoadFile(filePath);
    if (!node["username"] || !node["passwordHash"])
      return false;
    
    profile.username = node["username"].as<std::string>();
    profile.passwordHash = node["passwordHash"].as<std::string>();
    profile.portfolioPath = node["portfolioPath"] ? node["portfolioPath"].as<std::string>() : "";
    profile.lastLoginTime = node["lastLoginTime"] ? node["lastLoginTime"].as<std::string>() : "";
    
    return true;
  }

} // namespace KanVest
