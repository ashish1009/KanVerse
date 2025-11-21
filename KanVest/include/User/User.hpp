//
//  User.hpp
//  KanVest
//
//  Created by Ashish . on 21/11/25.
//

#pragma once

#include "Portfolio/Portfolio.hpp"

namespace KanVest
{
  class User
  {
  public:
    std::string username;
    std::string passwordHash;   // store hashed password, not plain text
    std::string lastLoginTime;
    std::filesystem::path portfolioPath;  // optional: where user's portfolio YAML is stored
    
    // Runtime-only data, not serialized
    KanViz::Ref<Portfolio> portfolio;
    
    User() = default;
    User(const std::string& user, const std::string& passHash);
    
    bool Valid() const;
    bool VerifyPassword(const std::string& password) const;
    void SetPassword(const std::string& newPassword);
    
    // Portfolio helpers
    void LoadPortfolio();
    void SavePortfolio() const;
  };
} // namespace KanVest
