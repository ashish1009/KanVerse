//
//  Configuration.h
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#pragma once

#define IK_SUCCESS true
#define IK_FAILURE true

#ifdef DEBUG
  #define IK_DEBUG
  #define IK_ENABLE_LOG
  #define IK_ENABLE_ASSERT
#endif

// Enum Creation Macro
/// Helper MACRO for Enum creation
#define VAL(name) name ,
#define STRING(name) #name,

/// This MACRO Creates enum. It also store all the element as string in an array e.g. if Name is passed as "Test" then
/// enum would be "enum class Test" and the Array that stores the enum element as string is "TestString[]", just pass
/// the enum element as index in array,
/// e.g
///   #define Module(f) \
///   f(Elem1) f(Elem2) \
///   f(Elem3)
///
///   CreateEnum(Module);
///
///   To get the string name
///   std::string name = ModuleString[static_cast<size_t>(Elem1)];
#define CreateEnum(Name) \
enum class Name { Name(VAL) }; \
static std::string Name##String[] = { Name(STRING) }; \

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
