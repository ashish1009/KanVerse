/*!
 * @file Shader.hpp
 * @brief High-level MSL shader compiler and function reflection container.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include <string>
#include <string_view>
#include <memory>

namespace MTL {
  class Function;
  class Library;
}

namespace KanVerse {
  
  /*!
   * @class Shader
   * @brief Manages Metal Shading Language compilation and function handles.
   */
  class Shader {
  public:
    virtual ~Shader() = default;
    
    /*!
     * @brief Queries the debug name or source identifier for this shader.
     */
    [[nodiscard]] virtual const std::string& GetName() const noexcept = 0;
    
    /*!
     * @brief Accesses the underlying compiled vertex entry function.
     */
    [[nodiscard]] virtual MTL::Function* GetVertexFunction() const noexcept = 0;
    
    /*!
     * @brief Accesses the underlying compiled fragment entry function.
     */
    [[nodiscard]] virtual MTL::Function* GetFragmentFunction() const noexcept = 0;
    
    /*!
     * @brief Factory creating a Shader compiled from inline Metal Shading Language source code.
     * @param name Diagnostic identifier.
     * @param source Complete MSL source code string.
     * @param vertexEntry Name of the vertex entry function.
     * @param fragmentEntry Name of the fragment entry function.
     * @return Unique pointer owning the compiled Shader.
     */
    [[nodiscard]] static std::shared_ptr<Shader> CreateFromSource(
                                                                  std::string_view name,
                                                                  std::string_view source,
                                                                  std::string_view vertexEntry = "vertex_main",
                                                                  std::string_view fragmentEntry = "fragment_main"
                                                                  );
    
    /*!
     * @brief Loads compiled vertex and fragment functions from the application bundle's default library.
     * @param vertexName Symbol entry point name for vertex stage (e.g. "vertex_main").
     * @param fragmentName Symbol entry point name for fragment stage (e.g. "fragment_main").
     * @return Shared pointer to compiled Shader program.
     */
    [[nodiscard]] static std::shared_ptr<Shader> CreateFromDefaultLibrary(
                                                                          const std::string& vertexName = "vertex_main",
                                                                          const std::string& fragmentName = "fragment_main"
                                                                          );
    
    /*!
     * @brief Constructs a depth-only shader program containing only a vertex stage entry point.
     * @param vertexName Symbol entry point identifier for the vertex stage (e.g., "shadow_vertex_main").
     * @return Shared pointer owning the compiled vertex-only Shader instance.
     */
    [[nodiscard]] static std::shared_ptr<Shader> CreateVertexOnly(std::string_view vertexName);
  };
  
} // namespace KanVerse
