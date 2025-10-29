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
    KanViz::ApplicationSpecification appSpec;
    return KanViz::ApplicationFactory::CreateApplication<KanViz::Application>(appSpec);
  }
} // namespace KanVest
