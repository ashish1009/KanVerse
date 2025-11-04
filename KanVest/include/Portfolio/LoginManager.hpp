//
//  LoginManager.hpp
//  KanVest
//
//  Created by Ashish . on 04/11/25.
//

#pragma once

namespace KanVest
{
  class LoginManager
  {
  public:
    /// This function handles the login
    /// - Parameters:
    ///   - username: username
    ///   - password: password
    static bool HandleLogin(const std::string& username, const std::string& password, std::string& loginMessage);
    /// This function handles the login
    /// - Parameters:
    ///   - username: username
    ///   - password: password
    static bool HandleSignUp(const std::string& username, const std::string& password, std::string& signUpMessage);
  };
} // namespace KanVest
