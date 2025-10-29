//
//  ApplicationFactory.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

namespace KanViz
{
  class Application;
  template <typename T> concept DerivedApplication = std::is_base_of_v<Application, T>;
  
  /// This is the factory class to create application instance
  struct ApplicationFactory
  {
    /// This function creates the application instance of type T class
    /// - Parameter args: arguments to be passed in application class
    /// - Returns: A unique pointer to an instance of Application<T>.
    template <DerivedApplication T, typename... Args> [[nodiscard("Created application not used")]]
    static Scope<Application> CreateApplication(Args... args)
    {
      return CreateScope<T>(std::forward<Args>(args)...);
    }
  };
} // namespace KanViz
