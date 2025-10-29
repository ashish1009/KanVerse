//
//  ShaderLibrary.cpp
//  KanViz
//
//  Created by Ashish . on 18/11/24.
//

#include "ShaderLibrary.hpp"

namespace KanViz
{
  Ref<Shader> ShaderLibrary::Get(const std::filesystem::path& shaderFilePath)
  {
    IK_PROFILE();
    if (s_shaders.find(shaderFilePath) == s_shaders.end() or !s_shaders.at(shaderFilePath))
    {
      s_shaders[shaderFilePath] = ShaderFactory::Create(shaderFilePath);
      IK_LOG_INFO(LogModule::Shader, "Shader '{0}' will be added to Shader Library. (Total Shaders {1})", shaderFilePath.filename().string(), s_shaders.size());
    }
    else
    {
      IK_LOG_INFO(LogModule::Shader, "Returning Pre loaded Shader '{0}' from Shader Library", shaderFilePath.filename().string());
    }
    return s_shaders.at(shaderFilePath);
  }
  
  const std::unordered_map<std::filesystem::path, Ref<Shader>>& ShaderLibrary::Get()
  {
    return s_shaders;
  }
  
  void ShaderLibrary::Clear()
  {
    IK_PROFILE();
    IK_LOG_INFO(LogModule::Shader, "Removing all shaders from Shader Library");
    s_shaders.clear();
  }
} // namespace KanViz
