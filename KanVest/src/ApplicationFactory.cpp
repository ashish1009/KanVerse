//
//  ApplicationFactory.cpp
//  KanVest
//
//  Created by Ashish . on 29/10/25.
//

#include "ApplicationFactory.hpp"

namespace KanVest
{
  KanViz::Scope<KanViz::Application> CreateApplication(std::span<const char*> args)
  {
    return KanViz::CreateScope<KanViz::Application>(KanViz::ApplicationSpecification());
  }
} // namespace KanVest
