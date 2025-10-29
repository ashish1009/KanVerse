//
//  ApplicationSpecification.h
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Core/WindowSpecification.h"

namespace KanViz
{
  /// Stores application-wide settings
  struct ApplicationSpecification
  {
    std::string appName {"KanViz"};                       ///< Application name
    WindowSpecification windowSpec {};                    ///< Primary window specification
  };
} // namespace KanViz
