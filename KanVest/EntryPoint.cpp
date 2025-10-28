//
//  EntryPoint.cpp
//  KanVest
//
//  Created by Ashish . on 28/10/25.
//

/// This function is the Entry point of the Studio
/// - Parameters:
///   - argc: Number of arguments passed from binary
///   - argv: Arguments ...
[[nodiscard]] int main(int argc, const char* argv[])
{
  std::cout << std::format("  Executing : {}\n", argv[0]);
  
  std::span args(argv, argv + argc);
  for (size_t i = 1; i < args.size(); ++i)
  {
    std::cout << std::format("    Arg[{}]   : {}\n", i, args[i]);
  }
  
  return 0;
}
