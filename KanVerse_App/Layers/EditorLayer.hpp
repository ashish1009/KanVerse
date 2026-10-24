/*!
 * @file EditorLayer.hpp
 * @brief Master KanVerse Studio Editor layer hosting the DockSpace, 3D Viewport, and tool panels.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#pragma once

#include "KanVerse_Headers.h"
#include "Renderer/UI/ImGuiLayer.hpp"
#include "Editor/Viewport/EditorCamera.hpp"
#include "Editor/Panels/WorldSettingsPanel.hpp"

namespace KanVerse {
  
  /*!
   * @class EditorLayer
   * @brief Coordinates the 3D scene preview, offscreen viewport framebuffer, and Dear ImGui workspace.
   */
  class EditorLayer : public Layer {
  public:
    EditorLayer();
    ~EditorLayer() override = default;
    
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(TimeStep ts) override;
    void OnRender() override;
    void OnEvent(Event& event) override;
    
  private:
    void RenderDockSpace();
    void RenderMenuBar();
    void RenderViewportPanel();
    void RenderTelemetryPanel();
    
    std::unique_ptr<ImGuiLayer>   m_ImGuiLayer;
    EditorCamera                  m_EditorCamera{ 60.0f, 1280.0f / 720.0f, 0.1f, 1000.0f };
    WorldSettingsPanel            m_WorldSettingsPanel;
    
    // Preview Scene Resources (before Phase 6 ECS Scene integration)
    DirectionalLight              m_SunLight;
    EnvironmentLighting           m_Environment;
    std::shared_ptr<Mesh>         m_PreviewBoxMesh;
    std::shared_ptr<Mesh>         m_PreviewGroundMesh;
    std::shared_ptr<Material>     m_PreviewBoxMaterial;
    std::shared_ptr<Material>     m_PreviewGroundMaterial;
    Math::Vec3                    m_BoxPosition{ 0.0f, 0.8f, 0.0f };
    Math::Vec3                    m_BoxRotation{ 0.0f, 0.0f, 0.0f };
    Math::Vec3                    m_BoxScale{ 1.0f, 1.0f, 1.0f };
    
    Math::Vec2                    m_ViewportSize{ 1280.0f, 720.0f };
    bool                          m_ViewportFocused{ false };
    bool                          m_ViewportHovered{ false };
    bool                          m_ShowDemoWindow{ false };
    float                         m_LastFrameTimeMs{ 0.0f };
  };
  
} // namespace KanVerse
