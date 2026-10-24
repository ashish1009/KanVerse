/*!
 * @file EditorLayer.mm
 * @brief Implementation of the KanVerse Studio Editor workspace, DockSpace, and 3D Viewport.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Layers/EditorLayer.hpp"
#include "Renderer/UI/UIWidgets.hpp"
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <imgui.h>

namespace KanVerse {
  
  /*
   * EditorLayer Implementation Notes:
   * ---------------------------------
   * 1. Renders the 3D scene into `Renderer::GetHDRFramebuffer()`.
   * 2. Registers a UI render callback with `Renderer::SetUIRenderCallback` so that during
   *    `Renderer::EndScene()`, after Compute Bloom and ACES Tonemapping finish, Dear ImGui
   *    is encoded directly into the final swapchain command buffer.
   * 3. Displays the HDR color attachment inside the dockable "3D Viewport" ImGui window
   *    and automatically resizes the framebuffer when the panel bounds change.
   */
  
  EditorLayer::EditorLayer()
  : Layer("EditorLayer"),
  m_SunLight(Math::MakeVec3(-0.45f, -1.0f, -0.35f), Math::MakeVec3(1.0f, 0.96f, 0.90f), 2.6f) {}
  
  void EditorLayer::OnAttach() {
    KV_INFO("EditorLayer: Bootstrapping KanVerse Studio...");
    
    m_ImGuiLayer = std::make_unique<ImGuiLayer>();
    m_ImGuiLayer->OnAttach();
    
    // Configure default procedural skybox for Phase 5 preview
    Renderer::SetEnvironmentMode(EnvironmentMode::Procedural);
    Renderer::SetIBLEnabled(true);
    
    // Create preview meshes and PBR materials to verify the Editor Viewport
    const BufferLayout layout = Renderer::GetStandardMeshLayout();
    m_PreviewBoxMesh    = Mesh::CreateChamferBox(0.75f, 0.08f, layout);
    m_PreviewGroundMesh = Mesh::CreateChamferBox(8.0f,  0.02f, layout);
    
    auto shader = Renderer::GetStandardPipeline()->GetSpecification().ShaderProgram;
    
    m_PreviewBoxMaterial = Material::Create(shader, "PreviewGoldBox");
    m_PreviewBoxMaterial->SetBaseColor(Math::MakeVec4(1.0f, 0.78f, 0.34f, 1.0f));
    m_PreviewBoxMaterial->SetRoughness(0.22f);
    m_PreviewBoxMaterial->SetMetallic(0.88f);
    
    m_PreviewGroundMaterial = Material::Create(shader, "PreviewCheckerGround");
    m_PreviewGroundMaterial->SetAlbedoTexture(Texture2D::CreateCheckerboard(512));
    m_PreviewGroundMaterial->SetTilingFactor(Math::MakeVec3(8.0f, 1.0f, 8.0f));
    m_PreviewGroundMaterial->SetRoughness(0.65f);
    m_PreviewGroundMaterial->SetMetallic(0.05f);
    
    // Register ImGui rendering hook into Renderer's swapchain pass
    Renderer::SetUIRenderCallback([this](MTL::CommandBuffer* cmdBuf,
                                         MTL::RenderCommandEncoder* encoder,
                                         MTL::RenderPassDescriptor* passDesc) {
      m_ImGuiLayer->Begin(passDesc);
      RenderDockSpace();
      m_ImGuiLayer->End(cmdBuf, encoder);
    });
  }
  
  void EditorLayer::OnDetach() {
    if (m_ImGuiLayer) {
      m_ImGuiLayer->OnDetach();
      m_ImGuiLayer.reset();
    }
  }
  
  void EditorLayer::OnUpdate(TimeStep ts) {
    m_LastFrameTimeMs = ts.GetMilliseconds();
    
    // Resize offscreen framebuffer and camera projection if the ImGui Viewport tab was resized
    auto hdrFb = Renderer::GetHDRFramebuffer();
    if (hdrFb) {
      const auto& spec = hdrFb->GetSpecification();
      if (m_ViewportSize.x > 1.0f && m_ViewportSize.y > 1.0f &&
          (spec.Width != static_cast<uint32_t>(m_ViewportSize.x) ||
           spec.Height != static_cast<uint32_t>(m_ViewportSize.y))) {
        hdrFb->Resize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
        m_EditorCamera.SetViewportSize(static_cast<uint32_t>(m_ViewportSize.x),
                                       static_cast<uint32_t>(m_ViewportSize.y));
      }
    }
    
    // Block ImGui from swallowing events when the user is actively navigating inside the 3D Viewport
    m_ImGuiLayer->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);
    m_EditorCamera.OnUpdate(ts, m_ViewportFocused || m_ViewportHovered);
  }
  
  void EditorLayer::OnRender() {
    // Build instance transforms for the preview ground and chamfer box
    Math::Mat4 groundModel = Math::Identity();
    groundModel.columns[1].y = 0.05f;
    groundModel.columns[3]   = Math::MakeVec4(0.0f, -0.4f, 0.0f, 1.0f);
    InstanceData groundInst{
      .ModelMatrix  = groundModel,
      .NormalMatrix = Math::Transpose(Math::Inverse(groundModel))
    };
    
    Math::Mat4 boxTrans = Math::Identity();
    boxTrans.columns[3] = Math::MakeVec4(m_BoxPosition.x, m_BoxPosition.y, m_BoxPosition.z, 1.0f);
    const Math::Mat4 rotX = Math::Rotate(m_BoxRotation.x, Math::MakeVec3(1.0f, 0.0f, 0.0f));
    const Math::Mat4 rotY = Math::Rotate(m_BoxRotation.y, Math::MakeVec3(0.0f, 1.0f, 0.0f));
    const Math::Mat4 rotZ = Math::Rotate(m_BoxRotation.z, Math::MakeVec3(0.0f, 0.0f, 1.0f));
    Math::Mat4 boxScale = Math::Identity();
    boxScale.columns[0].x = m_BoxScale.x;
    boxScale.columns[1].y = m_BoxScale.y;
    boxScale.columns[2].z = m_BoxScale.z;
    const Math::Mat4 boxModel = Math::Multiply(boxTrans, Math::Multiply(rotZ, Math::Multiply(rotY, Math::Multiply(rotX, boxScale))));
    InstanceData boxInst{
      .ModelMatrix  = boxModel,
      .NormalMatrix = Math::Transpose(Math::Inverse(boxModel))
    };
    
    // 1. Directional Shadow Pass
    Renderer::BeginShadowPass(m_SunLight, Renderer::GetShadowMapFramebuffer());
    Renderer::SubmitInstanced(Renderer::GetShadowPipeline(), nullptr, m_PreviewGroundMesh, std::span(&groundInst, 1));
    Renderer::SubmitInstanced(Renderer::GetShadowPipeline(), nullptr, m_PreviewBoxMesh,    std::span(&boxInst, 1));
    Renderer::EndScene();
    
    // 2. Main HDR Forward Pass + Compute Bloom + Tonemap + ImGui Overlay
    Renderer::BeginScene(m_EditorCamera.GetCamera(), m_SunLight, m_Environment, Renderer::GetHDRFramebuffer());
    Renderer::SubmitInstanced(Renderer::GetStandardPipeline(), m_PreviewGroundMaterial, m_PreviewGroundMesh, std::span(&groundInst, 1));
    Renderer::SubmitInstanced(Renderer::GetStandardPipeline(), m_PreviewBoxMaterial,    m_PreviewBoxMesh,    std::span(&boxInst, 1));
    Renderer::EndScene();
  }
  
  void EditorLayer::OnEvent(Event& event) {
    m_ImGuiLayer->OnEvent(event);
    if (!event.IsHandled()) {
      m_EditorCamera.OnEvent(event);
    }
  }
  
  void EditorLayer::RenderDockSpace() {
    static bool dockspaceOpen = true;
    static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
    
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoResize   | ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("KanVerseStudioDockSpace", &dockspaceOpen, windowFlags);
    ImGui::PopStyleVar(3);
    
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
      const ImGuiID dockspaceId = ImGui::GetID("KanVerseStudioDockSpaceID");
      ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
    }
    
    RenderMenuBar();
    RenderViewportPanel();
    RenderTelemetryPanel();
    m_WorldSettingsPanel.OnImGuiRender(m_SunLight, m_Environment);
    
    if (m_ShowDemoWindow) {
      ImGui::ShowDemoWindow(&m_ShowDemoWindow);
    }
    
    ImGui::End();
  }
  
  void EditorLayer::RenderMenuBar() {
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Exit", "Cmd+Q")) {
          Application::Get().Close();
        }
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Window")) {
        ImGui::MenuItem("ImGui Demo Window", nullptr, &m_ShowDemoWindow);
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }
  }
  
  void EditorLayer::RenderViewportPanel() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
    ImGui::Begin("3D Viewport");
    
    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();
    
    const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    m_ViewportSize = Math::MakeVec2(viewportPanelSize.x, viewportPanelSize.y);
    
    if (auto hdrFb = Renderer::GetHDRFramebuffer()) {
      MTL::Texture* colorTex = hdrFb->GetColorAttachment(0);
      if (colorTex) {
        ImGui::Image(reinterpret_cast<ImTextureID>(colorTex),
                     ImVec2{ m_ViewportSize.x, m_ViewportSize.y });
      }
    }
    
    ImGui::End();
    ImGui::PopStyleVar();
  }
  
  void EditorLayer::RenderTelemetryPanel() {
    ImGui::Begin("Preview Object & Telemetry");
    
    const float fps = (m_LastFrameTimeMs > 0.0001f) ? (1000.0f / m_LastFrameTimeMs) : 0.0f;
    ImGui::Text("Frame Time: %.2f ms (%.1f FPS)", m_LastFrameTimeMs, fps);
    
    const auto mem = MemoryTracker::GetStats();
    ImGui::Text("Active GPU/Heap Memory: %.2f MB (Peak: %.2f MB)",
                static_cast<double>(mem.CurrentUsage) / (1024.0 * 1024.0),
                static_cast<double>(mem.PeakUsage) / (1024.0 * 1024.0));
    
    ImGui::Separator();
    ImGui::Text("Preview ChamferBox Transform");
    UI::DrawVec3Control("Translation", m_BoxPosition, 0.0f);
    UI::DrawVec3Control("Rotation",    m_BoxRotation, 0.0f);
    UI::DrawVec3Control("Scale",       m_BoxScale,    1.0f);
    
    if (m_PreviewBoxMaterial) {
      ImGui::Separator();
      ImGui::Text("Preview PBR Material");
      Math::Vec4 baseColor = m_PreviewBoxMaterial->GetBaseColor();
      float color[3] = {baseColor.x, baseColor.y, baseColor.z};
      if (ImGui::ColorEdit3("Albedo", color)) {
        m_PreviewBoxMaterial->SetBaseColor(Math::MakeVec4(color[0], color[1], color[2], 1.0f));
      }
      float roughness = m_PreviewBoxMaterial->GetRoughness();
      if (ImGui::SliderFloat("Roughness", &roughness, 0.05f, 1.0f)) {
        m_PreviewBoxMaterial->SetRoughness(roughness);
      }
      float metallic = m_PreviewBoxMaterial->GetMetallic();
      if (ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f)) {
        m_PreviewBoxMaterial->SetMetallic(metallic);
      }
    }
    
    ImGui::End();
  }
  
} // namespace KanVerse
