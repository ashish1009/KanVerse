//
//  UserProfile.hpp
//  KanVest
//
//  Created by Ashish . on 04/11/25.
//

#pragma once

namespace KanVest
{
  class UserProfile
  {
  public:
    std::string username;
    std::string passwordHash;   // store hashed password, not plain text
    std::string lastLoginTime;
    std::filesystem::path portfolioPath;  // optional: where user's portfolio YAML is stored

    UserProfile() = default;
    UserProfile(const std::string& user, const std::string& passHash);

    bool Valid() const;
    bool VerifyPassword(const std::string& password) const;
    void SetPassword(const std::string& newPassword);
  };
  
  // Helper class to handle YAML serialization/deserialization
  class UserSerializer
  {
  public:
    static bool SaveToYAML(const UserProfile& profile, const std::string& filePath);
    static bool LoadFromYAML(UserProfile& profile, const std::string& filePath);
  };
} // namespace KanVest
