//
//  ImGuiLayer.cpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#include "ImGuiLayer.hpp"

#include <backends/imgui_impl_opengl3.cpp>
#include <backends/imgui_impl_glfw.cpp>

#include <GLFW/glfw3.h>

#include "Core/Application.hpp"

namespace KanViz::UI
{
  ImGuiLayer::ImGuiLayer(const Window* windowPointer)
  : Layer("ImGui"), m_window(windowPointer)
  {
    IK_PROFILE();
    IK_LOG_INFO(LogModule::UI, "Creating '{0}' Layer instance.", GetName());
  }
  
  ImGuiLayer::~ImGuiLayer()
  {
    IK_PROFILE();
    IK_LOG_WARN(LogModule::UI, "Destroying '{0}' Layer instance.", GetName());
  }
  
  void ImGuiLayer::OnAttach()
  {
    IK_PROFILE();
    IK_LOG_INFO(LogModule::UI, "Attaching '{0}' Layer to application", GetName());
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    // Setting up Dear ImGui style
    ImGui::StyleColorsDark();
    
    // When viewports are enabled we tweak WindowRounding/WindowBg so platform
    // windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      style.WindowRounding = 0.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    
    GLFWwindow* window = static_cast<GLFWwindow*>(m_window->GetNativeWindow());
    
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    
    ImGui::LoadIniSettingsFromDisk(m_initFilePath.string().c_str());
  }
  
  void ImGuiLayer::OnDetach()
  {
    IK_PROFILE();
    IK_LOG_WARN(LogModule::UI, "Detaching '{0}' Layer from application", GetName());
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }
  
  void ImGuiLayer::OnEvent(Event& event)
  {
    if (m_blockEvents)
    {
      ImGuiIO& io = ImGui::GetIO();
      event.isHandled |= event.IsInCategory(EventCategory::EventCategoryMouse) & io.WantCaptureMouse;
      event.isHandled |= event.IsInCategory(EventCategory::EventCategoryKeyboard) & io.WantCaptureKeyboard;
    }
  }
  
  void ImGuiLayer::Begin()
  {
    IK_PERFORMANCE_FUNC("ImGuiLayer::Begin");
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    
    ImGui::SaveIniSettingsToDisk(m_initFilePath.string().c_str());
    
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
  }
  
  void ImGuiLayer::End()
  {
    IK_PERFORMANCE_FUNC("ImGuiLayer::End");
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)m_window->GetWidth(), (float)m_window->GetHeight());
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      GLFWwindow* backupCurrentContext = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backupCurrentContext);
    }
  }
  
  void ImGuiLayer::SetIniFilePath(const std::filesystem::path& iniFilepath)
  {
    IK_PROFILE();
    m_initFilePath = iniFilepath;
    
    IK_LOG_INFO(LogModule::UI, "ImGui .ini file path {0}", m_initFilePath.string());
    
    if (m_initFilePath.empty())
    {
      IK_LOG_ERROR(LogModule::UI, "Invalid .ini file path.");
      return;
    }
    
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = m_initFilePath.c_str();
    
    ImGui::LoadIniSettingsFromDisk(m_initFilePath.string().c_str());
  }
  
  void ImGuiLayer::LoadFonts(const std::vector<UI::ImGuiFont>& otherFonts)
  {
    IK_PROFILE();
    if (otherFonts.size() == 0)
    {
      return;
    }
    
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->ClearFonts();
    
    IK_LOG_INFO(LogModule::UI, "Loading ImGui Fonts");
    for (const auto& font : otherFonts)
    {
      if (io.Fonts->AddFontFromFileTTF(font.filePath.c_str(), font.size) == nullptr)
      {
        IK_LOG_ERROR(LogModule::UI, "Failed to load font from {0}", font.filePath.string());
      }
      else
      {
        IK_LOG_INFO(LogModule::UI, "  Font    {0} Size {1}", Utils::FileSystem::Absolute(font.filePath).string(), font.size);
      }
    }
    
    // Default font is Regular
    io.FontDefault = io.Fonts->Fonts[0];
  }
} // namespace KanViz::UI
