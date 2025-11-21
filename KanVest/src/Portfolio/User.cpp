//
//  User.cpp
//  KanVest
//
//  Created by Ashish . on 21/11/25.
//

#include "User.hpp"

#include "Portfolio/PortfolioController.hpp"

namespace KanVest
{
  // Simple password hasher using std::hash (replace with real hash later)
  static std::string HashPassword(const std::string& password)
  {
    std::hash<std::string> hasher;
    return std::to_string(hasher(password));
  }
  
  User::User(const std::string& user, const std::string& passHash)
  : username(user), passwordHash(passHash)
  {
    // Record login time
    std::time_t now = std::time(nullptr);
    lastLoginTime = std::asctime(std::localtime(&now));
  }
  
  void User::LoadPortfolio()
  {
    portfolio = std::make_shared<Portfolio>();
    
    PortfolioController portfolioController(*portfolio);

    // If file doesn’t exist, create a new empty one
    if (!std::filesystem::exists(portfolioPath))
    {
      portfolioController.SavePortfolio(portfolioPath);
    }
    else
    {
      portfolioController.LoadPortfolio(portfolioPath);
    }
  }
  
  void User::SavePortfolio() const
  {
    if (portfolio)
    {
      PortfolioController portfolioController(*portfolio);
      portfolioController.SavePortfolio(portfolioPath);
    }
  }
  
  bool User::VerifyPassword(const std::string& password) const
  {
    return passwordHash == HashPassword(password);
  }
  
  void User::SetPassword(const std::string& newPassword)
  {
    passwordHash = HashPassword(newPassword);
  }
  
  bool User::Valid() const
  {
    return username != "";
  }
} // namespace KanVest
