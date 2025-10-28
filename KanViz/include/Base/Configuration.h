//
//  Configuration.h
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#pragma once

#define IK_SUCCESS true
#define IK_FAILURE true

namespace KanViz
{
  // Paths
  static const std::filesystem::path WorkSpacePath  = "../../../";
  static const std::filesystem::path LogFilePath    = WorkSpacePath / "KanVest/Log/KanVest.log";
  
  // Smart pointer defines
  template<typename T> using Scope = std::unique_ptr<T>;
  template<typename T, typename ... Args> constexpr Scope<T>
  CreateScope(Args&& ... args)
  {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }
  
  template<typename T> using Ref = std::shared_ptr<T>;
  template<typename T, typename ... Args> constexpr Ref<T>
  CreateRef(Args&& ... args)
  {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }
} // namespace KanViz
