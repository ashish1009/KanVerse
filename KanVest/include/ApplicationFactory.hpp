//
//  ApplicationFactory.hpp
//  KanVest
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

namespace KanVest
{
  /// Function prototype for creating application
  /// - Parameters:
  ///   - argc: Argument count
  ///   - argv: Arguments array
  KanViz::Scope<KanViz::Application> CreateApplication(std::span<const char*> args);
} // namespace KanVest
