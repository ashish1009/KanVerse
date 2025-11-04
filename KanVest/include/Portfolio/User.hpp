//
//  UserProfile.hpp
//  KanVest
//
//  Created by Ashish . on 04/11/25.
//

#pragma once

namespace KanVest
{
  class User
  {
  public:
    std::string username;
    std::string passwordHash;   // store hashed password, not plain text
    std::string lastLoginTime;
    std::filesystem::path portfolioPath;  // optional: where user's portfolio YAML is stored

    User() = default;
    User(const std::string& user, const std::string& passHash);

    bool Valid() const;
    bool VerifyPassword(const std::string& password) const;
    void SetPassword(const std::string& newPassword);
  };
  
  // Helper class to handle YAML serialization/deserialization
  class UserSerializer
  {
  public:
    static bool SaveToYAML(const User& profile, const std::string& filePath);
    static bool LoadFromYAML(User& profile, const std::string& filePath);
  };
} // namespace KanVest
