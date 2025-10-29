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
    KanViz::RendererType backendRendererType = KanViz::RendererType::OpenGL;
    KanViz::ApplicationSpecification appSpec =
    {
      .appName = "KanVerse",
      .rendererType = backendRendererType,
      .windowSpec =
      {
        .title = "KanVerse",
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
    
    return KanViz::ApplicationFactory::CreateApplication<KanViz::Application>(appSpec);
  }
} // namespace KanVest
