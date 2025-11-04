//
//  LoginManager.cpp
//  KanVest
//
//  Created by Ashish . on 04/11/25.
//

#include "LoginManager.hpp"

#include "Portfolio/UserDatabase.hpp"

namespace KanVest
{
  // TODO: Fix this
  static const std::filesystem::path UserDataPath = "../../../KanVest/UserData";
  
  bool LoginManager::HandleLogin(const std::string& username, const std::string& password, std::string& loginMessage)
  {
    // Ensure database is loaded (do this once at startup)
    if (!UserDatabase::LoadDatabase())
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
    if (UserDatabase::HasUser(username))
    {
      // Password Empty
      if (password == "")
      {
        loginMessage = "Password can not be empty";
        return false;
      }

//      IK_ASSERT(false);
//      // Step 2: Get the existing profile
//      const auto& user = UserDatabase::GetUser(username);
//      
//      // Step 3: Verify password
//      if (user.VerifyPassword(password))
//      {
//        std::cout << "✅ Login successful for " << username << "\n";
//        
//        // Example: You can now load portfolio or set current user
//        // g_CurrentUser = user;
//      }
//      else
//      {
//        std::cout << "❌ Invalid password for " << username << "\n";
//      }
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
    if (!UserDatabase::LoadDatabase())
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
    if (UserDatabase::HasUser(username))
    {
      signUpMessage = "User already exists.";
      return false;
    }
    else
    {
      UserProfile newUser(username, "");
      newUser.SetPassword(password);
      newUser.portfolioPath = UserDataPath / "Portfolios" / username / "_portfolio.yaml";

      UserDatabase::AddUser(newUser);
    }
    
    return true;
  }
} // namespace KanVest
