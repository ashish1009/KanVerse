//
//  ApplicationFactory.cpp
//  KanVest
//
//  Created by Ashish . on 29/10/25.
//

#include "ApplicationFactory.hpp"
#include "KanVestApp.hpp"

namespace KanVest
{
  KanViz::Scope<KanViz::Application> CreateApplication(std::span<const char*> args)
  {
    KanViz::RendererType backendRendererType = KanViz::RendererType::OpenGL;
    KanViz::ApplicationSpecification appSpec =
    {
      .appName = "KanVest",
      .rendererType = backendRendererType,
      .windowSpec =
      {
        .title = "KanVest",
        .rendererType = backendRendererType,
        .width = 1600,
        .height = 900,
        .samples = 8,
        .isFullScreen = false,
        .resizable = true,
        .maximized = false,
        .hideTitleBar = true,
      },
      .iniFilePath = "../../../KanVest/KanVest.ini"
    };
    
    return KanViz::ApplicationFactory::CreateApplication<KanVest::Application>(appSpec);
  }
} // namespace KanVest
