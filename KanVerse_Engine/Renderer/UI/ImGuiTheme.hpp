/*!
 * @file ImGuiTheme.hpp
 * @brief Custom Blender/Unreal-inspired charcoal dark theme for KanVerse Studio.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

namespace KanVerse {
  
  /*!
   * @class ImGuiTheme
   * @brief Configures global Dear ImGui style variables, padding, rounding, and color palettes.
   */
  class ImGuiTheme {
  public:
    /*!
     * @brief Applies the signature KanVerse Studio charcoal dark theme to the active ImGui context.
     */
    static void ApplyDarkTheme() noexcept;
  };
  
} // namespace KanVerse
