/*!
 * @file ImGuiTheme.cpp
 * @brief Implementation of the KanVerse Studio dark color scheme and layout styling.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/UI/ImGuiTheme.hpp"
#include <imgui.h>

namespace KanVerse {
  
  /*
   * ImGuiTheme Implementation Notes:
   * --------------------------------
   * Tunes window padding, frame rounding, tab borders, and docking splitter thickness
   * for high-DPI Retina displays. Uses a neutral charcoal palette so HDR viewport colors
   * are perceived without chromatic adaptation bias.
   */
  void ImGuiTheme::ApplyDarkTheme() noexcept {
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
    style.FrameBorderSize   = 0.0f;
    
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      style.WindowRounding = 0.0f;
    }
    
    ImVec4* colors = style.Colors;
    
    // Primary backgrounds
    colors[ImGuiCol_WindowBg]             = ImVec4{ 0.11f, 0.115f, 0.125f, 1.0f };
    colors[ImGuiCol_ChildBg]              = ImVec4{ 0.10f, 0.105f, 0.115f, 1.0f };
    colors[ImGuiCol_PopupBg]              = ImVec4{ 0.13f, 0.135f, 0.15f,  0.98f };
    colors[ImGuiCol_Border]               = ImVec4{ 0.22f, 0.23f,  0.25f,  1.0f };
    
    // Headers & Accordions
    colors[ImGuiCol_Header]               = ImVec4{ 0.20f, 0.21f,  0.23f,  1.0f };
    colors[ImGuiCol_HeaderHovered]        = ImVec4{ 0.28f, 0.30f,  0.34f,  1.0f };
    colors[ImGuiCol_HeaderActive]         = ImVec4{ 0.16f, 0.46f,  0.78f,  1.0f };
    
    // Buttons
    colors[ImGuiCol_Button]               = ImVec4{ 0.20f, 0.21f,  0.24f,  1.0f };
    colors[ImGuiCol_ButtonHovered]        = ImVec4{ 0.28f, 0.31f,  0.36f,  1.0f };
    colors[ImGuiCol_ButtonActive]         = ImVec4{ 0.16f, 0.46f,  0.78f,  1.0f };
    
    // Frame inputs (Sliders, DragFloat, TextInput)
    colors[ImGuiCol_FrameBg]              = ImVec4{ 0.07f, 0.075f, 0.085f, 1.0f };
    colors[ImGuiCol_FrameBgHovered]       = ImVec4{ 0.15f, 0.16f,  0.18f,  1.0f };
    colors[ImGuiCol_FrameBgActive]        = ImVec4{ 0.19f, 0.21f,  0.24f,  1.0f };
    
    // Tabs & Docking
    colors[ImGuiCol_Tab]                  = ImVec4{ 0.13f, 0.14f,  0.155f, 1.0f };
    colors[ImGuiCol_TabHovered]           = ImVec4{ 0.26f, 0.52f,  0.88f,  0.80f };
    colors[ImGuiCol_TabActive]            = ImVec4{ 0.20f, 0.42f,  0.72f,  1.0f };
    colors[ImGuiCol_TabUnfocused]         = ImVec4{ 0.11f, 0.115f, 0.125f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive]   = ImVec4{ 0.17f, 0.18f,  0.20f,  1.0f };
    
    // Title Bars
    colors[ImGuiCol_TitleBg]              = ImVec4{ 0.08f, 0.085f, 0.095f, 1.0f };
    colors[ImGuiCol_TitleBgActive]        = ImVec4{ 0.08f, 0.085f, 0.095f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed]     = ImVec4{ 0.08f, 0.085f, 0.095f, 0.75f };
    
    // Accent highlights
    colors[ImGuiCol_CheckMark]            = ImVec4{ 0.26f, 0.59f,  0.98f,  1.0f };
    colors[ImGuiCol_SliderGrab]           = ImVec4{ 0.26f, 0.59f,  0.98f,  0.85f };
    colors[ImGuiCol_SliderGrabActive]     = ImVec4{ 0.36f, 0.69f,  1.00f,  1.0f };
    colors[ImGuiCol_DockingPreview]       = ImVec4{ 0.26f, 0.59f,  0.98f,  0.70f };
  }
  
} // namespace KanVerse
