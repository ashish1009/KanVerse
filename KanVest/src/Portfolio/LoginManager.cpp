//
//  LoginManager.cpp
//  KanVest
//
//  Created by Ashish . on 04/11/25.
//

#include "LoginManager.hpp"

#include "Portfolio/UserManager.hpp"
#include "Portfolio/Portfolio.hpp"

namespace KanVest
{
  // TODO: Fix this
  static const std::filesystem::path UserDataPath = "../../../KanVest/UserData";
  
  bool LoginManager::HandleLogin(const std::string& username, const std::string& password, std::string& loginMessage)
  {
    // Ensure database is loaded (do this once at startup)
    if (!UserManager::LoadDatabase())
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
  
  bool LoginManager::HandleSignUp(const std::string& username, const std::string& password, std::string& signUpMessage)
  {
    // Ensure database is loaded (do this once at startup)
    if (!UserManager::LoadDatabase())
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

      std::filesystem::path directory =  UserDataPath / "Portfolios";
      std::string filePath =  username + "_portfolio.yaml";
      if (!std::filesystem::exists(directory))
      {
        std::filesystem::create_directory(directory);
      }
      
      newUser.portfolioPath = directory / filePath;

      UserManager::AddUser(newUser);
    }
    
    return true;
  }
} // namespace KanVest
