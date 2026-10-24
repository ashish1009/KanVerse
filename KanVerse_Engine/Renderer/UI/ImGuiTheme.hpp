/*!
 * @file ImGuiTheme.hpp
 * @brief Multi-theme, dynamic highlighter, and custom font manager for KanVerse Studio.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Core/Math/Math.hpp"
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

struct ImFont;

namespace KanVerse {
  
  /*!
   * @enum EditorThemeMode
   * @brief Base surface color palettes for the KanVerse Studio interface.
   */
  enum class EditorThemeMode : std::uint8_t {
    DarkCharcoal = 0, ///< Default Blender/Unreal neutral charcoal dark theme.
    LightStudio,      ///< High-contrast daylight studio theme with soft alabaster backgrounds.
    MidnightOLED,     ///< True-black OLED theme for maximum HDR viewport contrast.
    CyberpunkSlate,   ///< Deep navy-violet slate theme.
    ClassicImGui      ///< Standard Dear ImGui classic styling.
  };
  
  /*!
   * @enum EditorAccentPreset
   * @brief Quick-select highlighter / accent color presets.
   */
  enum class EditorAccentPreset : std::uint8_t {
    AzureBlue = 0,    ///< Default Studio Azure Blue (0.20, 0.52, 0.95)
    BlenderOrange,    ///< Blender 3D Amber Orange (0.92, 0.48, 0.12)
    EmeraldGreen,     ///< Vitality Emerald Green (0.15, 0.78, 0.45)
    CrimsonRed,       ///< High-energy Crimson Red (0.88, 0.20, 0.26)
    AmethystPurple,   ///< Royal Amethyst Purple (0.62, 0.32, 0.92)
    CyberNeonCyan,    ///< Electric Cyan Highlighter (0.05, 0.82, 0.90)
    Goldenrod,        ///< Warm Gold Highlighter (0.92, 0.70, 0.14)
    Custom            ///< User-defined custom RGB color picker value
  };
  
  /*!
   * @struct EditorFontEntry
   * @brief Metadata and ImGui handle for a loaded custom `.ttf`/`.otf` font.
   */
  struct EditorFontEntry {
    std::string Name;           ///< Display name (e.g., "OpenSans-Regular", "HfMonorita", "Headers").
    std::string Filepath;       ///< Absolute path on disk.
    ImFont*     Font{ nullptr };///< Compiled ImGui font atlas handle.
  };
  
  /*!
   * @class ImGuiTheme
   * @brief Manages global Dear ImGui themes, dynamic accent highlights, and custom Studio fonts.
   *
   * @note Thread-safety: Must be called on the main UI thread with an active ImGuiContext.
   */
  class ImGuiTheme {
  public:
    /*!
     * @brief Resolves the portable `KanVerse_App/Assets` directory path relative to the repository root.
     * @return Resolved `std::filesystem::path` pointing to `KanVerse_App/Assets`.
     */
    [[nodiscard]] static std::filesystem::path GetAssetsDirectory() noexcept;
    
    /*!
     * @brief Scans `KanVerse_App/Assets/Fonts/{Opensans, HfMonorita, Headers}` and loads all `.ttf`/`.otf` fonts.
     * @param baseFontSizePx Default pixel size for primary UI text (default: 15.5f for Retina clarity).
     */
    static void LoadStudioFonts(float baseFontSizePx = 15.5f);
    
    /*!
     * @brief Applies the default Dark Charcoal theme with Azure Blue highlights.
     */
    static void ApplyDarkTheme() noexcept;
    
    /*!
     * @brief Applies a specific base theme mode while preserving the current accent highlight color.
     * @param mode Target base theme mode.
     */
    static void SetThemeMode(EditorThemeMode mode) noexcept;
    
    /*!
     * @brief Queries the currently active base theme mode.
     * @return Active EditorThemeMode enum.
     */
    [[nodiscard]] static EditorThemeMode GetThemeMode() noexcept { return s_CurrentMode; }
    
    /*!
     * @brief Applies a predefined highlighter preset and updates all active ImGui accent slots.
     * @param preset Target accent preset.
     */
    static void SetAccentPreset(EditorAccentPreset preset) noexcept;
    
    /*!
     * @brief Queries the currently selected highlighter preset.
     * @return Active EditorAccentPreset enum.
     */
    [[nodiscard]] static EditorAccentPreset GetAccentPreset() noexcept { return s_CurrentPreset; }
    
    /*!
     * @brief Sets a custom RGB highlighter color and dynamically recomputes hover/active/dim shades.
     * @param rgb Linear or sRGB color vector in [0.0, 1.0].
     */
    static void SetCustomAccentColor(const Math::Vec3& rgb) noexcept;
    
    /*!
     * @brief Queries the current primary highlighter RGB color.
     * @return Current 3D color vector.
     */
    [[nodiscard]] static const Math::Vec3& GetAccentColor() noexcept { return s_AccentColor; }
    
    /*!
     * @brief Accesses the bold/header font handle (used for panel titles and section headers).
     * @return Pointer to the header `ImFont`, or the default font if unavailable.
     */
    [[nodiscard]] static ImFont* GetHeaderFont() noexcept { return s_HeaderFont; }
    
    /*!
     * @brief Accesses the monospace font handle (`HfMonorita` for console logs and numeric telemetry).
     * @return Pointer to the monospace `ImFont`, or the default font if unavailable.
     */
    [[nodiscard]] static ImFont* GetMonospaceFont() noexcept { return s_MonoFont; }
    
    /*!
     * @brief Accesses the bold OpenSans font handle.
     * @return Pointer to the bold `ImFont`, or the default font if unavailable.
     */
    [[nodiscard]] static ImFont* GetBoldFont() noexcept { return s_BoldFont; }
    
    /*!
     * @brief Renders an interactive "Studio Theme, Highlighter & Fonts" configuration section inside any ImGui panel.
     */
    static void DrawThemeSelectorUI() noexcept;
    
  private:
    static void RefreshStyleAndColors() noexcept;
    
    inline static EditorThemeMode              s_CurrentMode{ EditorThemeMode::DarkCharcoal };
    inline static EditorAccentPreset           s_CurrentPreset{ EditorAccentPreset::AzureBlue };
    inline static Math::Vec3                   s_AccentColor{ 0.20f, 0.52f, 0.95f };
    inline static std::vector<EditorFontEntry> s_LoadedFonts;
    inline static int                          s_ActiveFontIndex{ 0 };
    inline static float                        s_GlobalFontScale{ 1.0f };
    inline static ImFont*                      s_RegularFont{ nullptr };
    inline static ImFont*                      s_BoldFont{ nullptr };
    inline static ImFont*                      s_HeaderFont{ nullptr };
    inline static ImFont*                      s_MonoFont{ nullptr };
  };
  
} // namespace KanVerse
