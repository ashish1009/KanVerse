/*!
 * @file WorldSettingsPanel.cpp
 * @brief Implementation of the WorldSettingsPanel UI controls.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Editor/Panels/WorldSettingsPanel.hpp"
#include "Renderer/Renderer/Renderer.hpp"
#include "Renderer/UI/UIWidgets.hpp"
#include <imgui.h>

namespace KanVerse {
  
  /*
   * WorldSettingsPanel Implementation Notes:
   * ----------------------------------------
   * Directly mutates the active `DirectionalLight`, `EnvironmentLighting`, and `Renderer`
   * post-processing parameters (`BloomPass` threshold, knee, intensity, and ACES exposure)
   * so artists can tune lighting and optical glow interactively at 120 FPS.
   */
  void WorldSettingsPanel::OnImGuiRender(DirectionalLight& sunLight, EnvironmentLighting& envLighting) {
    ImGui::Begin("World & Post-Process Settings");
    
    // -------------------------------------------------------------------------
    // 1. Directional Sun / Moon Light
    // -------------------------------------------------------------------------
    if (ImGui::CollapsingHeader("Directional Sun Light", ImGuiTreeNodeFlags_DefaultOpen)) {
      Math::Vec3 dir = sunLight.GetDirection();
      if (UI::DrawVec3Control("Direction", dir, -0.5f)) {
        if (simd::length_squared(dir) > 1e-4f) {
          sunLight.SetDirection(dir);
        }
      }
      
      float color[3] = {sunLight.GetColor().x, sunLight.GetColor().y, sunLight.GetColor().z};
      if (ImGui::ColorEdit3("Sun Color", color)) {
        sunLight.SetColor(Math::MakeVec3(color[0], color[1], color[2]));
      }
      
      float intensity = sunLight.GetIntensity();
      if (ImGui::SliderFloat("Sun Intensity", &intensity, 0.0f, 10.0f, "%.2f")) {
        sunLight.SetIntensity(intensity);
      }
      
      float orthoExtent = sunLight.GetOrthoExtent();
      if (ImGui::SliderFloat("Shadow Bounds", &orthoExtent, 75.0f, 300.0f, "%.1f m")) {
        sunLight.SetOrthoExtent(orthoExtent);
      }
    }
    
    // -------------------------------------------------------------------------
    // 2. Environment & Image-Based Lighting (IBL)
    // -------------------------------------------------------------------------
    if (ImGui::CollapsingHeader("Environment & Skybox (IBL)", ImGuiTreeNodeFlags_DefaultOpen)) {
      bool iblEnabled = Renderer::IsIBLEnabled();
      if (ImGui::Checkbox("Enable Image-Based Lighting", &iblEnabled)) {
        Renderer::SetIBLEnabled(iblEnabled);
      }
      
      const char* envModes[] = { "Crisp HDR", "Diffuse Irradiance", "Procedural Sky" };
      int currentMode = static_cast<int>(Renderer::GetEnvironmentMode());
      if (ImGui::Combo("Skybox Mode", &currentMode, envModes, IM_ARRAYSIZE(envModes))) {
        Renderer::SetEnvironmentMode(static_cast<EnvironmentMode>(currentMode));
      }
      
      // Use a temporary float array for ImGui color editing to avoid SIMD address errors
      float ambientCol[3] = { envLighting.AmbientColor.x, envLighting.AmbientColor.y, envLighting.AmbientColor.z };
      if (ImGui::ColorEdit3("Ambient Tint", ambientCol)) {
        envLighting.AmbientColor = Math::MakeVec3(ambientCol[0], ambientCol[1], ambientCol[2]);
      }
      
      ImGui::SliderFloat("Ambient Intensity", &envLighting.AmbientIntensity, 0.0f, 3.0f, "%.2f");
    }
    
    // -------------------------------------------------------------------------
    // 3. Dual-Kawase Compute Bloom & ACES Filmic Tonemapping
    // -------------------------------------------------------------------------
    if (ImGui::CollapsingHeader("Post-Processing (Bloom & ACES)", ImGuiTreeNodeFlags_DefaultOpen)) {
      auto& bloom = Renderer::GetBloomPass();
      
      if (ImGui::SliderFloat("Bloom Threshold", &m_BloomThreshold, 0.1f, 5.0f, "%.2f")) {
        bloom.SetThreshold(m_BloomThreshold);
      }
      if (ImGui::SliderFloat("Bloom Soft Knee", &m_BloomSoftKnee, 0.0f, 1.0f, "%.2f")) {
        bloom.SetSoftKnee(m_BloomSoftKnee);
      }
      
      float bloomIntensity = bloom.GetIntensity();
      if (ImGui::SliderFloat("Bloom Intensity", &bloomIntensity, 0.0f, 3.0f, "%.2f")) {
        bloom.SetIntensity(bloomIntensity);
      }
      
      float exposure = Renderer::GetExposure();
      if (ImGui::SliderFloat("Camera Exposure", &exposure, 0.1f, 4.0f, "%.2f")) {
        Renderer::SetExposure(exposure);
      }
    }
    
    ImGui::End();
  }
  
} // namespace KanVerse
