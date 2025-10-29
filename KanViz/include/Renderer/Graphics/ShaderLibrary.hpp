//
//  ShaderLibrary.hpp
//  KanViz
//
//  Created by Ashish . on 18/11/24.
//

#pragma once

#include "Renderer/Graphics/Shader.hpp"

namespace KanViz
{
  /// This class stores the loaded compiled shaders in a library (map)
  class ShaderLibrary
  {
  public:
    /// This function returns the pre loaded shader from library
    /// - Parameter shaderFilePath: shader file path
    /// - Note: Creates new if not present in map
    static Ref<Shader> Get(const std::filesystem::path& shaderFilePath);
    /// This function returns all the shader map
    static const std::unordered_map<std::filesystem::path, Ref<Shader>>& Get();
    /// This function clear the shaders loaded in library
    static void Clear();
    
    DELETE_ALL_CONSTRUCTORS(ShaderLibrary);
    
  private:
    inline static std::unordered_map<std::filesystem::path, Ref<Shader>> s_shaders;
  };
} // namespace KanViz

