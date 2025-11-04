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
  bool LoginManager::HandleLogin(const std::string& username, const std::string& password, std::string& loginMessage)
  {
    if (username == "")
    {
      loginMessage = "Username can not be empty";
      return false;
    }

    if (password == "")
    {
      loginMessage = "Password can not be empty";
      return false;
    }

    // Ensure database is loaded (do this once at startup)
    if (!UserDatabase::LoadDatabase())
    {
      loginMessage = "User database not found";
      return false;
    }
    
    // Step 1: Check if user exists
    if (UserDatabase::HasUser(username))
    {
      IK_ASSERT(false);
      // Step 2: Get the existing profile
      const auto& user = UserDatabase::GetUser(username);
      
      // Step 3: Verify password
      if (user.VerifyPassword(password))
      {
        std::cout << "✅ Login successful for " << username << "\n";
        
        // Example: You can now load portfolio or set current user
        // g_CurrentUser = user;
      }
      else
      {
        std::cout << "❌ Invalid password for " << username << "\n";
      }
    }
    else
    {
      loginMessage = "User not found. Sign-up to create new account";
      return false;
//      // Step 4: Handle new user registration
//      std::cout << "🆕 User not found. Creating new account for: " << username << "\n";
//      
//      UserProfile newUser(username, "");
//      newUser.SetPassword(password);
//      newUser.portfolioPath = "./data/portfolios/" + username + "_portfolio.yaml";
//      
//      UserDatabase::AddUser(newUser);
//      std::cout << "✅ User " << username << " created successfully.\n";
    }
    return true;
  }
} // namespace KanVest
