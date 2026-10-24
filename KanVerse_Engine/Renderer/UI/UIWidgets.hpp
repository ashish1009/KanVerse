/*!
 * @file UIWidgets.hpp
 * @brief Reusable Dear ImGui property widgets (Vec3 X/Y/Z reset controls, labeled sliders).
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Core/Math/Math.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <string>

namespace KanVerse::UI {
  
  /*!
   * @brief Renders an Unreal/Hazel-style 3-axis vector control with colored X/Y/Z reset buttons.
   *
   * @param label Property name displayed in the left column.
   * @param values Mutable reference to the 3D SIMD vector being edited.
   * @param resetValue Default scalar value applied when an X, Y, or Z button is clicked.
   * @param columnWidth Width in pixels of the left label column.
   * @return True if any component of `values` was modified this frame.
   */
  inline bool DrawVec3Control(const std::string& label,
                              Math::Vec3& values,
                              float resetValue = 0.0f,
                              float columnWidth = 100.0f) {
    bool modified = false;
    
    ImGui::PushID(label.c_str());
    
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();
    
    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 0.0f });
    
    const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    const ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
    
    // --- X Axis (Red) ---
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4{ 0.75f, 0.15f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.88f, 0.25f, 0.25f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{ 0.75f, 0.15f, 0.15f, 1.0f });
    if (ImGui::Button("X", buttonSize)) {
      values.x = resetValue;
      modified = true;
    }
    ImGui::PopStyleColor(3);
    
    ImGui::SameLine();
    if (ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f")) {
      modified = true;
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    
    // --- Y Axis (Green) ---
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4{ 0.20f, 0.65f, 0.20f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.30f, 0.78f, 0.30f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{ 0.20f, 0.65f, 0.20f, 1.0f });
    if (ImGui::Button("Y", buttonSize)) {
      values.y = resetValue;
      modified = true;
    }
    ImGui::PopStyleColor(3);
    
    ImGui::SameLine();
    if (ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f")) {
      modified = true;
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    
    // --- Z Axis (Blue) ---
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4{ 0.15f, 0.35f, 0.80f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.25f, 0.48f, 0.92f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{ 0.15f, 0.35f, 0.80f, 1.0f });
    if (ImGui::Button("Z", buttonSize)) {
      values.z = resetValue;
      modified = true;
    }
    ImGui::PopStyleColor(3);
    
    ImGui::SameLine();
    if (ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f")) {
      modified = true;
    }
    ImGui::PopItemWidth();
    
    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();
    
    return modified;
  }
  
} // namespace KanVerse::UI
