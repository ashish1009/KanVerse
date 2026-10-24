/*!
 * @file WorldSettingsPanel.hpp
 * @brief Dockable ImGui panel for tuning directional sunlight, IBL skyboxes, and Dual-Kawase bloom.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "Renderer/Lighting/DirectionalLight.hpp"
#include "Renderer/Lighting/EnvironmentLighting.hpp"

namespace KanVerse {
  
  /*!
   * @class WorldSettingsPanel
   * @brief Exposes real-time sliders for Time-of-Day, Sun vectors, IBL modes, Bloom, and ACES Exposure.
   */
  class WorldSettingsPanel {
  public:
    WorldSettingsPanel() = default;
    ~WorldSettingsPanel() = default;
    
    /*!
     * @brief Renders the World Settings & Post-Processing ImGui window.
     * @param sunLight Mutable reference to the active directional sun light.
     * @param envLighting Mutable reference to the active ambient environment settings.
     */
    void OnImGuiRender(DirectionalLight& sunLight, EnvironmentLighting& envLighting);
    
  private:
    float m_BloomThreshold{ 1.1f };
    float m_BloomSoftKnee{ 0.5f };
  };
  
} // namespace KanVerse
