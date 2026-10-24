/*!
 * @file ImGuiTheme.cpp
 * @brief Implementation of multi-theme palettes, dynamic highlighter colors, and custom font loading.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/UI/ImGuiTheme.hpp"
#include "Core/Diagnostics/Log.hpp"
#include <imgui.h>
#include <algorithm>

namespace KanVerse {
  
  /*
   * ImGuiTheme Implementation Notes:
   * --------------------------------
   * 1. Portable Asset Path Resolution (Option 2):
   *    `__FILE__` is `.../KanVerse/KanVerse_Engine/Renderer/UI/ImGuiTheme.cpp`.
   *    Navigating up 4 parent directories reaches the repository root `.../KanVerse/`,
   *    from which `KanVerse_App/Assets` is resolved automatically on any Mac.
   *
   * 2. Font Atlas Discovery:
   *    Scans `Assets/Fonts/Opensans`, `Assets/Fonts/HfMonorita`, and `Assets/Fonts/Headers`
   *    for `.ttf` and `.otf` files, rasterizing them with 3x horizontal/vertical oversampling
   *    for razor-sharp Retina typography.
   */
  
  std::filesystem::path ImGuiTheme::GetAssetsDirectory() noexcept {
    // ImGuiTheme.cpp is at: KanVerse/KanVerse_Engine/Renderer/UI/ImGuiTheme.cpp
    // parent_path() x4 -> KanVerse/
    const std::filesystem::path repoRoot = std::filesystem::path(__FILE__)
      .parent_path() // UI
      .parent_path() // Renderer
      .parent_path() // KanVerse_Engine
      .parent_path(); // KanVerse
    
    return repoRoot / "KanVerse_App" / "Assets";
  }
  
  void ImGuiTheme::LoadStudioFonts(float baseFontSizePx) {
    ImGuiIO& io = ImGui::GetIO();
    s_LoadedFonts.clear();
    
    const std::filesystem::path fontsRoot = GetAssetsDirectory() / "Fonts";
    if (!std::filesystem::exists(fontsRoot)) {
      KV_CORE_WARN("ImGuiTheme: Fonts directory not found at '{}'. Falling back to default ImGui font.",
                   fontsRoot.string());
      io.Fonts->AddFontDefault();
      return;
    }
    
    ImFontConfig fontCfg{};
    fontCfg.OversampleH = 3;
    fontCfg.OversampleV = 2;
    fontCfg.PixelSnapH  = true;
    
    const std::vector<std::filesystem::path> fontFolders = {
      fontsRoot / "Opensans",
      fontsRoot / "HfMonorita",
      fontsRoot / "Headers"
    };
    
    // Collect and sort all .ttf and .otf files across your 3 font folders
    std::vector<std::filesystem::path> discoveredFiles;
    for (const auto& folder : fontFolders) {
      if (!std::filesystem::exists(folder)) {
        continue;
      }
      for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (!entry.is_regular_file()) continue;
        const auto ext = entry.path().extension().string();
        if (ext == ".ttf" || ext == ".TTF" || ext == ".otf" || ext == ".OTF") {
          discoveredFiles.push_back(entry.path());
        }
      }
    }
    
    std::sort(discoveredFiles.begin(), discoveredFiles.end());
    
    for (const auto& filePath : discoveredFiles) {
      const std::string stem      = filePath.stem().string();
      const std::string folderTag = filePath.parent_path().filename().string();
      const std::string label     = folderTag + " / " + stem;
      
      // Render Headers slightly larger (17.5px) for distinct section titles
      const float targetSize = (folderTag == "Headers") ? (baseFontSizePx + 2.0f) : baseFontSizePx;
      
      ImFont* loadedFont = io.Fonts->AddFontFromFileTTF(filePath.string().c_str(), targetSize, &fontCfg);
      if (loadedFont) {
        s_LoadedFonts.push_back(EditorFontEntry{
          .Name     = label,
          .Filepath = filePath.string(),
          .Font     = loadedFont
        });
        
        // Auto-assign semantic roles based on filename/folder
        if (stem.find("Regular") != std::string::npos && folderTag == "Opensans") {
          s_RegularFont     = loadedFont;
          io.FontDefault    = loadedFont;
          s_ActiveFontIndex = static_cast<int>(s_LoadedFonts.size() - 1);
        } else if ((stem.find("SemiBold") != std::string::npos || stem == "OpenSans-Bold") && !s_BoldFont) {
          s_BoldFont = loadedFont;
        } else if (folderTag == "Headers" && !s_HeaderFont) {
          s_HeaderFont = loadedFont;
        } else if (folderTag == "HfMonorita" && !s_MonoFont) {
          s_MonoFont = loadedFont;
        }
        
        KV_CORE_INFO("ImGuiTheme: Loaded Studio Font '{}' ({:.1f} px)", label, targetSize);
      } else {
        KV_CORE_WARN("ImGuiTheme: Failed to load font file '{}'", filePath.string());
      }
    }
    
    if (s_LoadedFonts.empty()) {
      io.Fonts->AddFontDefault();
    } else if (!io.FontDefault) {
      io.FontDefault    = s_LoadedFonts.front().Font;
      s_RegularFont     = io.FontDefault;
      s_ActiveFontIndex = 0;
    }
    
    if (!s_BoldFont)   s_BoldFont   = io.FontDefault;
    if (!s_HeaderFont) s_HeaderFont = s_BoldFont;
    if (!s_MonoFont)   s_MonoFont   = io.FontDefault;
  }
  
  static ImVec4 ScaleColor(const Math::Vec3& c, float multiplier, float alpha = 1.0f) noexcept {
    return ImVec4{
      std::clamp(c.x * multiplier, 0.0f, 1.0f),
      std::clamp(c.y * multiplier, 0.0f, 1.0f),
      std::clamp(c.z * multiplier, 0.0f, 1.0f),
      alpha
    };
  }
  
  void ImGuiTheme::ApplyDarkTheme() noexcept {
    s_CurrentMode   = EditorThemeMode::DarkCharcoal;
    s_CurrentPreset = EditorAccentPreset::AzureBlue;
    s_AccentColor   = Math::MakeVec3(0.20f, 0.52f, 0.95f);
    RefreshStyleAndColors();
  }
  
  void ImGuiTheme::SetThemeMode(EditorThemeMode mode) noexcept {
    s_CurrentMode = mode;
    RefreshStyleAndColors();
  }
  
  void ImGuiTheme::SetAccentPreset(EditorAccentPreset preset) noexcept {
    s_CurrentPreset = preset;
    switch (preset) {
      case EditorAccentPreset::AzureBlue:
        s_AccentColor = Math::MakeVec3(0.20f, 0.52f, 0.95f);
        break;
      case EditorAccentPreset::BlenderOrange:
        s_AccentColor = Math::MakeVec3(0.92f, 0.48f, 0.12f);
        break;
      case EditorAccentPreset::EmeraldGreen:
        s_AccentColor = Math::MakeVec3(0.15f, 0.78f, 0.45f);
        break;
      case EditorAccentPreset::CrimsonRed:
        s_AccentColor = Math::MakeVec3(0.88f, 0.20f, 0.26f);
        break;
      case EditorAccentPreset::AmethystPurple:
        s_AccentColor = Math::MakeVec3(0.62f, 0.32f, 0.92f);
        break;
      case EditorAccentPreset::CyberNeonCyan:
        s_AccentColor = Math::MakeVec3(0.05f, 0.82f, 0.90f);
        break;
      case EditorAccentPreset::Goldenrod:
        s_AccentColor = Math::MakeVec3(0.92f, 0.70f, 0.14f);
        break;
      case EditorAccentPreset::Custom:
        break;
    }
    RefreshStyleAndColors();
  }
  
  void ImGuiTheme::SetCustomAccentColor(const Math::Vec3& rgb) noexcept {
    s_CurrentPreset = EditorAccentPreset::Custom;
    s_AccentColor   = rgb;
    RefreshStyleAndColors();
  }
  
  void ImGuiTheme::RefreshStyleAndColors() noexcept {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding     = ImVec2(8.0f, 8.0f);
    style.FramePadding      = ImVec2(6.0f, 4.0f);
    style.ItemSpacing       = ImVec2(6.0f, 6.0f);
    style.WindowRounding    = 4.0f;
    style.FrameRounding     = 3.0f;
    style.PopupRounding     = 4.0f;
    style.GrabRounding      = 2.0f;
    style.TabRounding       = 4.0f;
    style.WindowBorderSize  = 1.0f;
    style.FrameBorderSize   = 1.0f;
    
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      style.WindowRounding = 0.0f;
    }
    
    if (s_CurrentMode == EditorThemeMode::ClassicImGui) {
      ImGui::StyleColorsClassic();
    } else if (s_CurrentMode == EditorThemeMode::LightStudio) {
      ImGui::StyleColorsLight();
    } else {
      ImGui::StyleColorsDark();
    }
    
    ImVec4* colors = style.Colors;
    
    switch (s_CurrentMode) {
      case EditorThemeMode::DarkCharcoal: {
        colors[ImGuiCol_Text]                 = ImVec4{ 0.92f, 0.93f, 0.95f, 1.0f };
        colors[ImGuiCol_TextDisabled]         = ImVec4{ 0.50f, 0.52f, 0.55f, 1.0f };
        colors[ImGuiCol_WindowBg]             = ImVec4{ 0.11f, 0.115f, 0.125f, 1.0f };
        colors[ImGuiCol_ChildBg]              = ImVec4{ 0.10f, 0.105f, 0.115f, 1.0f };
        colors[ImGuiCol_PopupBg]              = ImVec4{ 0.13f, 0.135f, 0.15f,  0.98f };
        colors[ImGuiCol_Border]               = ImVec4{ 0.22f, 0.23f,  0.25f,  1.0f };
        colors[ImGuiCol_FrameBg]              = ImVec4{ 0.07f, 0.075f, 0.085f, 1.0f };
        colors[ImGuiCol_FrameBgHovered]       = ImVec4{ 0.15f, 0.16f,  0.18f,  1.0f };
        colors[ImGuiCol_FrameBgActive]        = ImVec4{ 0.19f, 0.21f,  0.24f,  1.0f };
        colors[ImGuiCol_TitleBg]              = ImVec4{ 0.08f, 0.085f, 0.095f, 1.0f };
        colors[ImGuiCol_TitleBgActive]        = ImVec4{ 0.09f, 0.095f, 0.11f,  1.0f };
        colors[ImGuiCol_TitleBgCollapsed]     = ImVec4{ 0.08f, 0.085f, 0.095f, 0.75f };
        colors[ImGuiCol_MenuBarBg]            = ImVec4{ 0.10f, 0.105f, 0.115f, 1.0f };
        colors[ImGuiCol_Button]               = ImVec4{ 0.20f, 0.21f,  0.24f,  1.0f };
        colors[ImGuiCol_Header]               = ImVec4{ 0.20f, 0.21f,  0.23f,  1.0f };
        colors[ImGuiCol_Tab]                  = ImVec4{ 0.13f, 0.14f,  0.155f, 1.0f };
        colors[ImGuiCol_TabUnfocused]         = ImVec4{ 0.11f, 0.115f, 0.125f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive]   = ImVec4{ 0.17f, 0.18f,  0.20f,  1.0f };
        break;
      }
      case EditorThemeMode::LightStudio: {
        colors[ImGuiCol_Text]                 = ImVec4{ 0.10f, 0.11f, 0.13f, 1.0f };
        colors[ImGuiCol_TextDisabled]         = ImVec4{ 0.52f, 0.54f, 0.58f, 1.0f };
        colors[ImGuiCol_WindowBg]             = ImVec4{ 0.93f, 0.94f, 0.95f, 1.0f };
        colors[ImGuiCol_ChildBg]              = ImVec4{ 0.90f, 0.91f, 0.93f, 1.0f };
        colors[ImGuiCol_PopupBg]              = ImVec4{ 0.96f, 0.97f, 0.98f, 0.98f };
        colors[ImGuiCol_Border]               = ImVec4{ 0.76f, 0.78f, 0.81f, 1.0f };
        colors[ImGuiCol_FrameBg]              = ImVec4{ 1.00f, 1.00f, 1.00f, 1.0f };
        colors[ImGuiCol_FrameBgHovered]       = ImVec4{ 0.86f, 0.88f, 0.91f, 1.0f };
        colors[ImGuiCol_FrameBgActive]        = ImVec4{ 0.80f, 0.83f, 0.87f, 1.0f };
        colors[ImGuiCol_TitleBg]              = ImVec4{ 0.84f, 0.85f, 0.88f, 1.0f };
        colors[ImGuiCol_TitleBgActive]        = ImVec4{ 0.80f, 0.82f, 0.85f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed]     = ImVec4{ 0.84f, 0.85f, 0.88f, 0.75f };
        colors[ImGuiCol_MenuBarBg]            = ImVec4{ 0.88f, 0.89f, 0.91f, 1.0f };
        colors[ImGuiCol_Button]               = ImVec4{ 0.82f, 0.84f, 0.87f, 1.0f };
        colors[ImGuiCol_Header]               = ImVec4{ 0.82f, 0.84f, 0.87f, 1.0f };
        colors[ImGuiCol_Tab]                  = ImVec4{ 0.84f, 0.86f, 0.89f, 1.0f };
        colors[ImGuiCol_TabUnfocused]         = ImVec4{ 0.86f, 0.87f, 0.90f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive]   = ImVec4{ 0.78f, 0.80f, 0.84f, 1.0f };
        break;
      }
      case EditorThemeMode::MidnightOLED: {
        colors[ImGuiCol_Text]                 = ImVec4{ 0.95f, 0.96f, 0.98f, 1.0f };
        colors[ImGuiCol_TextDisabled]         = ImVec4{ 0.45f, 0.47f, 0.50f, 1.0f };
        colors[ImGuiCol_WindowBg]             = ImVec4{ 0.03f, 0.03f, 0.035f, 1.0f };
        colors[ImGuiCol_ChildBg]              = ImVec4{ 0.02f, 0.02f, 0.025f, 1.0f };
        colors[ImGuiCol_PopupBg]              = ImVec4{ 0.05f, 0.05f, 0.06f,  0.98f };
        colors[ImGuiCol_Border]               = ImVec4{ 0.16f, 0.17f, 0.19f,  1.0f };
        colors[ImGuiCol_FrameBg]              = ImVec4{ 0.08f, 0.085f, 0.095f, 1.0f };
        colors[ImGuiCol_FrameBgHovered]       = ImVec4{ 0.13f, 0.14f,  0.16f,  1.0f };
        colors[ImGuiCol_FrameBgActive]        = ImVec4{ 0.17f, 0.18f,  0.21f,  1.0f };
        colors[ImGuiCol_TitleBg]              = ImVec4{ 0.01f, 0.01f,  0.015f, 1.0f };
        colors[ImGuiCol_TitleBgActive]        = ImVec4{ 0.02f, 0.02f,  0.03f,  1.0f };
        colors[ImGuiCol_TitleBgCollapsed]     = ImVec4{ 0.01f, 0.01f,  0.015f, 0.75f };
        colors[ImGuiCol_MenuBarBg]            = ImVec4{ 0.04f, 0.04f,  0.05f,  1.0f };
        colors[ImGuiCol_Button]               = ImVec4{ 0.12f, 0.13f,  0.15f,  1.0f };
        colors[ImGuiCol_Header]               = ImVec4{ 0.12f, 0.13f,  0.15f,  1.0f };
        colors[ImGuiCol_Tab]                  = ImVec4{ 0.07f, 0.075f, 0.09f,  1.0f };
        colors[ImGuiCol_TabUnfocused]         = ImVec4{ 0.04f, 0.04f,  0.05f,  1.0f };
        colors[ImGuiCol_TabUnfocusedActive]   = ImVec4{ 0.11f, 0.12f,  0.14f,  1.0f };
        break;
      }
      case EditorThemeMode::CyberpunkSlate: {
        colors[ImGuiCol_Text]                 = ImVec4{ 0.90f, 0.94f, 1.00f, 1.0f };
        colors[ImGuiCol_TextDisabled]         = ImVec4{ 0.48f, 0.52f, 0.65f, 1.0f };
        colors[ImGuiCol_WindowBg]             = ImVec4{ 0.08f, 0.09f, 0.15f, 1.0f };
        colors[ImGuiCol_ChildBg]              = ImVec4{ 0.07f, 0.08f, 0.13f, 1.0f };
        colors[ImGuiCol_PopupBg]              = ImVec4{ 0.10f, 0.11f, 0.18f, 0.98f };
        colors[ImGuiCol_Border]               = ImVec4{ 0.22f, 0.25f, 0.38f, 1.0f };
        colors[ImGuiCol_FrameBg]              = ImVec4{ 0.05f, 0.06f, 0.10f, 1.0f };
        colors[ImGuiCol_FrameBgHovered]       = ImVec4{ 0.13f, 0.15f, 0.24f, 1.0f };
        colors[ImGuiCol_FrameBgActive]        = ImVec4{ 0.17f, 0.20f, 0.32f, 1.0f };
        colors[ImGuiCol_TitleBg]              = ImVec4{ 0.05f, 0.06f, 0.11f, 1.0f };
        colors[ImGuiCol_TitleBgActive]        = ImVec4{ 0.07f, 0.08f, 0.14f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed]     = ImVec4{ 0.05f, 0.06f, 0.11f, 0.75f };
        colors[ImGuiCol_MenuBarBg]            = ImVec4{ 0.07f, 0.08f, 0.14f, 1.0f };
        colors[ImGuiCol_Button]               = ImVec4{ 0.15f, 0.17f, 0.28f, 1.0f };
        colors[ImGuiCol_Header]               = ImVec4{ 0.15f, 0.17f, 0.28f, 1.0f };
        colors[ImGuiCol_Tab]                  = ImVec4{ 0.11f, 0.13f, 0.22f, 1.0f };
        colors[ImGuiCol_TabUnfocused]         = ImVec4{ 0.08f, 0.09f, 0.16f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive]   = ImVec4{ 0.15f, 0.18f, 0.29f, 1.0f };
        break;
      }
      case EditorThemeMode::ClassicImGui:
        break;
    }
    
    const ImVec4 accentBase    = ScaleColor(s_AccentColor, 1.00f, 1.00f);
    const ImVec4 accentHovered = ScaleColor(s_AccentColor, 1.18f, 1.00f);
    const ImVec4 accentActive  = ScaleColor(s_AccentColor, 0.85f, 1.00f);
    
    colors[ImGuiCol_CheckMark]            = accentBase;
    colors[ImGuiCol_SliderGrab]           = accentBase;
    colors[ImGuiCol_SliderGrabActive]     = accentHovered;
    colors[ImGuiCol_ButtonHovered]        = accentHovered;
    colors[ImGuiCol_ButtonActive]         = accentActive;
    colors[ImGuiCol_HeaderHovered]        = ScaleColor(s_AccentColor, 0.95f, 0.80f);
    colors[ImGuiCol_HeaderActive]         = accentBase;
    colors[ImGuiCol_SeparatorHovered]     = accentHovered;
    colors[ImGuiCol_SeparatorActive]      = accentBase;
    colors[ImGuiCol_ResizeGrip]           = ScaleColor(s_AccentColor, 0.80f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]    = accentHovered;
    colors[ImGuiCol_ResizeGripActive]     = accentBase;
    colors[ImGuiCol_TabHovered]           = accentHovered;
    colors[ImGuiCol_TabActive]            = accentBase;
    colors[ImGuiCol_DockingPreview]       = ScaleColor(s_AccentColor, 1.00f, 0.70f);
    colors[ImGuiCol_TextSelectedBg]       = ScaleColor(s_AccentColor, 1.00f, 0.38f);
    colors[ImGuiCol_NavHighlight]         = accentBase;
    colors[ImGuiCol_PlotLinesHovered]     = accentHovered;
    colors[ImGuiCol_PlotHistogram]        = accentBase;
    colors[ImGuiCol_PlotHistogramHovered] = accentHovered;
  }
  
  void ImGuiTheme::DrawThemeSelectorUI() noexcept {
    if (ImGui::CollapsingHeader("Studio Theme, Highlighter & Fonts", ImGuiTreeNodeFlags_DefaultOpen)) {
      const char* themeNames[] = {
        "Dark Charcoal (Default)",
        "Light Studio (Daylight)",
        "Midnight OLED (True Black)",
        "Cyberpunk Slate",
        "Classic ImGui"
      };
      int currentTheme = static_cast<int>(s_CurrentMode);
      if (ImGui::Combo("UI Theme", &currentTheme, themeNames, IM_ARRAYSIZE(themeNames))) {
        SetThemeMode(static_cast<EditorThemeMode>(currentTheme));
      }
      
      const char* presetNames[] = {
        "Azure Blue",
        "Blender Orange",
        "Emerald Green",
        "Crimson Red",
        "Amethyst Purple",
        "Cyber Neon Cyan",
        "Goldenrod",
        "Custom Color..."
      };
      int currentPreset = static_cast<int>(s_CurrentPreset);
      if (ImGui::Combo("Highlighter Preset", &currentPreset, presetNames, IM_ARRAYSIZE(presetNames))) {
        SetAccentPreset(static_cast<EditorAccentPreset>(currentPreset));
      }
      
      float accentRGB[3] = { s_AccentColor.x, s_AccentColor.y, s_AccentColor.z };
      if (ImGui::ColorEdit3("Highlighter Color", accentRGB)) {
        SetCustomAccentColor(Math::MakeVec3(accentRGB[0], accentRGB[1], accentRGB[2]));
      }
      
      // --- Live Custom Font Selector ---
      if (!s_LoadedFonts.empty()) {
        ImGui::Separator();
        const char* previewName = s_LoadedFonts[s_ActiveFontIndex].Name.c_str();
        if (ImGui::BeginCombo("Studio UI Font", previewName)) {
          for (int i = 0; i < static_cast<int>(s_LoadedFonts.size()); ++i) {
            const bool isSelected = (s_ActiveFontIndex == i);
            ImGui::PushFont(s_LoadedFonts[i].Font);
            if (ImGui::Selectable(s_LoadedFonts[i].Name.c_str(), isSelected)) {
              s_ActiveFontIndex         = i;
              ImGui::GetIO().FontDefault = s_LoadedFonts[i].Font;
            }
            if (isSelected) {
              ImGui::SetItemDefaultFocus();
            }
            ImGui::PopFont();
          }
          ImGui::EndCombo();
        }
        
        if (ImGui::SliderFloat("Font Scale", &s_GlobalFontScale, 0.75f, 1.50f, "%.2fx")) {
          ImGui::GetIO().FontGlobalScale = s_GlobalFontScale;
        }
      }
    }
  }
  
} // namespace KanVerse
