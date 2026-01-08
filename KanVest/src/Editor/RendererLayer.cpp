//
//  RendererLayer.cpp
//  KanVest
//
//  Created by Ashish . on 29/10/25.
//

#include "RendererLayer.hpp"

#include "UI/UI_KanVestPanel.hpp"

#include "URL_API/API_Provider.hpp"

namespace KanVest
{
  static const std::filesystem::path KanVestResourcePath = "../../../KanVest/Resources";
  
  // Kretor Resource Path
#define KanVestResourcePath(path) std::filesystem::absolute(KanVestResourcePath / path)
  
  // Kreate Texture
#define CreateTexture(path) KanViz::TextureFactory::Create(KanVestResourcePath(path))
    
  using FontMap = std::unordered_map<UI::FontType, KanViz::UI::ImGuiFont>;

  inline std::pair<UI::FontType, KanViz::UI::ImGuiFont>
  Make(UI::FontType type, const std::string& path, uint32_t size)
  {
    return {type, KanViz::UI::ImGuiFont{ KanVestResourcePath(path), size }};
  }
  
  static FontMap GenerateFontsForRange(UI::FontType baseFont, const std::string& font, uint32_t minLim, uint32_t maxLim, uint32_t step = 2)
  {
    FontMap fonts;
    
    uint32_t index = 0;
    for (uint32_t size = minLim; size <= maxLim; size += step)
    {
      fonts.emplace(static_cast<UI::FontType>(static_cast<uint32_t>(baseFont) + index), KanViz::UI::ImGuiFont{KanVestResourcePath(font), size});
      ++index;
    }
    
    return fonts;
  }

  RendererLayer* RendererLayer::s_instance = nullptr;
  RendererLayer& RendererLayer::Get()
  {
    return *s_instance;
  }
  
  RendererLayer::RendererLayer()
  : KanViz::Layer("Kreator Renderer")
  {
    IK_PROFILE();
    IK_ASSERT(!s_instance, "RendererLayer instance already created");
    
    // Copy the single instance of application
    s_instance = this;
    
    IK_LOG_INFO("RendererLayer", "Creating '{0}' Layer instance", GetName());
    
    // Load Textures -----------------------------------------------------------------------------
    m_welcomeIcon = CreateTexture("Textures/Logo/WelcomeIKan.png");
    m_applicationIcon = CreateTexture("Textures/Logo/IKan.png");

    // Window Icons
    m_iconClose = CreateTexture("Textures/Icons/Close.png");
    m_iconMinimize = CreateTexture("Textures/Icons/Minimize.png");
    m_iconMaximize = CreateTexture("Textures/Icons/Maximize.png");
    m_iconRestore = CreateTexture("Textures/Icons/Restore.png");
    m_shadowTexture = CreateTexture("Textures/Editor/Shadow.png");
    
    // Widget Icons
    m_searchIcon = CreateTexture("Textures/Icons/Search.png");
    m_settingIcon = CreateTexture("Textures/Icons/Gear.png");

    m_reloadIcon = CreateTexture("Textures/Icons/Rotate.png");

    // Eye
    m_closeEyeIcon = CreateTexture("Textures/Icons/CloseEye.png");
    m_openEyeIcon = CreateTexture("Textures/Icons/Eye.png");
  }
  
  RendererLayer::~RendererLayer()
  {
    IK_PROFILE();
    IK_LOG_WARN("RendererLayer", "Destroying '{0}' Layer instance", GetName());
  }
  
  void RendererLayer::OnAttach() noexcept
  {
    IK_PROFILE();
    IK_LOG_INFO("RendererLayer", "Attaching '{0}' Layer to application", GetName());
    
    // Set the Imgui theme
    FontMap fonts = {
      // ───────────── OpenSans (Base UI) ─────────────
      Make(UI::FontType::Regular,         "Fonts/Opensans/Regular.ttf",         18),
      Make(UI::FontType::Light,           "Fonts/Opensans/Light.ttf",           18),
      Make(UI::FontType::Italic,          "Fonts/Opensans/Italic.ttf",          18),
      Make(UI::FontType::LightItalic,     "Fonts/Opensans/LightItalic.ttf",     18),
      Make(UI::FontType::SemiBold,        "Fonts/Opensans/SemiBold.ttf",        18),
      Make(UI::FontType::Bold,            "Fonts/Opensans/Bold.ttf",            18),
      Make(UI::FontType::SemiBoldItalic,  "Fonts/Opensans/SemiBoldItalic.ttf",  18),
      Make(UI::FontType::BoldItalic,      "Fonts/Opensans/BoldItalic.ttf",      18),
      Make(UI::FontType::ExtraBold,       "Fonts/Opensans/ExtraBold.ttf",       18),
      Make(UI::FontType::ExtraBoldItalic, "Fonts/Opensans/ExtraBoldItalic.ttf", 18),
      
      // ───────────── Fixed Width (Code / Debug) ─────────────
      Make(UI::FontType::FixedWidthRegular, "Fonts/HfMonorita/Regular.ttf", 10),
      Make(UI::FontType::FixedWidthLight,   "Fonts/HfMonorita/Light.ttf",   10),
      Make(UI::FontType::FixedWidthMedium,  "Fonts/HfMonorita/Medium.ttf",  10),
      Make(UI::FontType::FixedWidthBold,    "Fonts/HfMonorita/Bold.ttf",    10),
      
      // ───────────── Size Variants ─────────────
      Make(UI::FontType::Small,      "Fonts/Opensans/Regular.ttf", 16),
      Make(UI::FontType::Medium,     "Fonts/Opensans/Regular.ttf", 20),
      Make(UI::FontType::Large,      "Fonts/Opensans/Regular.ttf", 22),
      Make(UI::FontType::ExtraLarge, "Fonts/Opensans/Regular.ttf", 24),
      
      // ───────────── Headers (OpenSans) ─────────────
      Make(UI::FontType::SemiHeader,  "Fonts/Opensans/SemiBold.ttf",  28),
      Make(UI::FontType::Header,      "Fonts/Opensans/Bold.ttf",      32),
      Make(UI::FontType::LargeHeader, "Fonts/Opensans/ExtraBold.ttf", 40),
      Make(UI::FontType::HugeHeader,  "Fonts/Opensans/ExtraBold.ttf", 50),
    };
    
    // Append generated Noto headers
    FontMap notoHeaders = GenerateFontsForRange(UI::FontType::Header_02, "Fonts/Noto_Sans/static/NotoSans-Regular.ttf", 2, 60);
    fonts.insert(notoHeaders.begin(), notoHeaders.end());
    
    // Append generated fixed headers
    FontMap fixedHeaders = GenerateFontsForRange(UI::FontType::FixedWidthHeader_12, "Fonts/HfMonorita/Regular.ttf", 12, 40);
    fonts.insert(fixedHeaders.begin(), fixedHeaders.end());
    
    KanVest::UI::Font::Load(fonts);
    
    KanVasX::Color::Initialize();
    KanVasX::Panel::Initialize(KanVasX::UI::GetTextureID(m_shadowTexture->GetRendererID()));
    
    KanVasX::Widget::Initialize();
    KanVasX::Widget::SetSearchIcon(KanVasX::UI::GetTextureID(m_searchIcon->GetRendererID()));
    KanVasX::Widget::SetSettingIcon(KanVasX::UI::GetTextureID(m_settingIcon->GetRendererID()));
    
    // Intialize KanVest Data
    API_Provider::Initialize(StockAPIProvider::Yahoo);
  }
  
  void RendererLayer::OnDetach() noexcept
  {
    IK_PROFILE();
    IK_LOG_WARN("RendererLayer", "Detaching '{0}' Layer from application", GetName());
  }
  
  void RendererLayer::OnUpdate(const KanViz::TimeStep& ts)
  {

  }
  
  void RendererLayer::OnImGuiRender()
  {
    UI_StartMainWindowDocking();
    
    KanVest::UI::Panel::Show();

    UI_EndMainWindowDocking();
  }
  
  void RendererLayer::OnEvent(KanViz::Event& event)
  {
  }
  
  void RendererLayer::UI_StartMainWindowDocking()
  {
    IK_PERFORMANCE_FUNC("RendererLayer::UI_StartMainWindowDocking");
    static bool optFullscreenPersistant = true;
    static ImGuiDockNodeFlags optFlags = ImGuiDockNodeFlags_None;
    static bool optFullscreen = optFullscreenPersistant;
    
    // ImGui Input Output Fonts
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    
    io.ConfigWindowsResizeFromEdges = io.BackendFlags & ImGuiBackendFlags_HasMouseCursors;
    
    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
    if (optFullscreen)
    {
      ImGuiViewport* viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(viewport->Pos);
      ImGui::SetNextWindowSize(viewport->Size);
      ImGui::SetNextWindowViewport(viewport->ID);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
      windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
      windowFlags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
      windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
    ImGui::Begin("DockSpace Demo", nullptr, windowFlags);
    ImGui::PopStyleColor(); // MenuBarBg
    ImGui::PopStyleVar(2);
    
    if (optFullscreen)
    {
      ImGui::PopStyleVar(2);
    }
    
    // Render the title if original title bar is hidden
//    const auto& applicationSpec = KanViz::Application::Get().GetSpecification().windowSpec;
//    if (applicationSpec.hideTitleBar)
//    {
//      float titlebarHeight = UI_DrawTitlebar();
//      KanVasX::UI::SetCursorPosY(titlebarHeight + ImGui::GetCurrentWindow()->WindowPadding.y);
//    }
    
    // Dockspace
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 250.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
      ImGuiID dockspaceID = ImGui::GetID("MyDockspace");
      ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), optFlags);
    }
    style.WindowMinSize.x = minWinSizeX;
  }
  
  void RendererLayer::UI_EndMainWindowDocking()
  {
    IK_PERFORMANCE_FUNC("RendererLayer::UI_EndMainWindowDocking");
    ImGui::End();
  }
  
  void RendererLayer::UI_PerformancePanel()
  {
    KanVasX::Panel::Begin("Performance");
    {
      ImGui::Text("Frame Time: %.2f ms (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::Separator();
      
      const KanViz::PerfNode* root = KanViz::PerformanceProfile::Get().GetRoot(); // adjust to your API
      
      if (!root) {
        ImGui::TextDisabled("No performance data available.");
        ImGui::End();
        return;
      }
      
      std::function<void(const KanViz::PerfNode*, int)> drawNode;
      drawNode = [&](const KanViz::PerfNode* node, int depth)
      {
        if (!node) return;
        
        double ms = node->durationMicro / 1000.0;
        
        // Color by duration
        ImVec4 color;
        if (ms > 8.0)      color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); // slow (red)
        else if (ms > 3.0) color = ImVec4(1.0f, 0.8f, 0.3f, 1.0f); // medium (yellow)
        else               color = ImVec4(0.6f, 1.0f, 0.6f, 1.0f); // fast (green)
        
        bool open = ImGui::TreeNodeEx(
                                      (void*)node,
                                      ImGuiTreeNodeFlags_DefaultOpen | (node->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0),
                                      "%s", node->name.c_str()
                                      );
        
        ImGui::SameLine(550);
        ImGui::TextColored(color, "%6.3f ms", ms);
        
        if (open) {
          for (auto& child : node->children)
            drawNode(child.get(), depth + 1);
          ImGui::TreePop();
        }
      };
      
      drawNode(root, 0);
    }
    
    KanVasX::Panel::End();
  }
  
  float RendererLayer::UI_DrawTitlebar()
  {
    IK_PERFORMANCE_FUNC("RendererLayer::UI_DrawTitlebar");
    
    static constexpr float titleBarHeight = 48.0f;
    const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;
    
    // Drag and Control the window with user title bar ----------------------------------
    UI_TitlebarDragArea(titleBarHeight);
    
    // Title bar rectangle --------------------------------------------------------------
    KanVasX::UI::SetCursorPos(windowPadding);
    KanVasX::UI::DrawFilledRect(KanVasX::Color::Background, titleBarHeight);
    
    // Draw KanVest Logo ----------------------------------------------------------------
    ImGui::SetItemAllowOverlap();
    KanVasX::UI::SetCursorPos(windowPadding);
    
    static const ImVec2 size = {titleBarHeight - 10.0f, titleBarHeight - 10.0f};
    if (KanVasX::UI::DrawButtonImage("MainMenu", KanVasX::UI::GetTextureID(m_applicationIcon->GetRendererID()), false, size, {10.0f, 5.0f}, KanVasX::Color::White))
    {
      ImGui::OpenPopup("MainMenu");
    }
    
    // KanVest Name ---------------------------------------------------------------------
    KanVasX::UI::SetCursorPos({0.0f, 0.0f});
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header), "KanVest", KanVasX::UI::AlignX::Center, {0.0f, 10.0f}, KanVasX::Color::TextMuted);
    
    // Title Rectangles -----------------------------------------------------------------
    KanVasX::UI::SetCursorPos({ImGui::GetWindowWidth() / 4, windowPadding.y});
    KanVasX::UI::DrawRect(KanVasX::Color::Alpha(KanVasX::Color::HighlightMuted, 0.4f), titleBarHeight / 1.6, 0.5f, {0.0f, titleBarHeight / 4});
    
    // Render the Window Buttons --------------------------------------------------------
    KanVasX::UI::SetCursorPosX(ImGui::GetWindowWidth() - 78);
    KanVasX::UI::SetCursorPosY(10.0f);
    UI_WindowButtons();
    
    return titleBarHeight;
  }
  
  void RendererLayer::UI_TitlebarDragArea(float titlebarHeight)
  {
    static float moveOffsetX;
    static float moveOffsetY;
    
    auto* rootWindow = ImGui::GetCurrentWindow()->RootWindow;
    const float windowWidth = (int32_t)rootWindow->RootWindow->Size.x;
    
    if (ImGui::InvisibleButton("##titleBarDragZone", ImVec2(ImGui::GetContentRegionAvail().x, titlebarHeight * 2), ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_AllowItemOverlap))
    {
      ImVec2 point = ImGui::GetMousePos();
      ImRect rect = rootWindow->Rect();
      
      // Calculate the difference between the cursor pos and window pos
      moveOffsetX = point.x - rect.Min.x;
      moveOffsetY = point.y - rect.Min.y;
    }
    
    const KanViz::Window* window = KanViz::Application::Get().GetWindow();
    bool maximized = window->IsMaximized();
    
    // Maximize or restore on doublt click
    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) and ImGui::IsItemHovered())
    {
      (maximized) ? window->Restore() : window->Maximize();
    }
    else if (ImGui::IsItemActive())
    {
      if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
      {
        if (maximized)
        {
          window->Restore();
          
          // Get the original size of window
          [[maybe_unused]] uint32_t newWidth = window->GetWidth();
          [[maybe_unused]] uint32_t newHeight = window->GetHeight();
          
          // Offset position proportionally to mouse position on titlebar
          // This ensures we dragging window relatively to cursor position on titlebar
          // correctly when window size changes
          if (windowWidth - (float)newWidth > 0.0f)
          {
            moveOffsetX *= (float)newWidth / windowWidth;
          }
        }
        
        // Update the new position of window
        ImVec2 point = ImGui::GetMousePos();
        window->SetPosition({point.x - moveOffsetX, point.y - moveOffsetY});
      }
    }
  }
  
  void RendererLayer::UI_WindowButtons()
  {
    // Window buttons
    static const ImU32 buttonColN = KanVasX::Color::MultipliedValue(KanVasX::Color::Text, 0.9f);
    static const ImU32 buttonColP = KanVasX::Color::TextDark;
    
    static const float buttonWidth = 14.0f;
    static const float buttonHeight = 14.0f;
    static const KanViz::Window* window = KanViz::Application::Get().GetWindow();
    
    // Minimize Button
    {
      static const float iconHeight = m_iconMinimize->GetHeight();
      
      if (KanVasX::UI::DrawButtonImage("Minimize", KanVasX::UI::GetTextureID(m_iconMinimize->GetRendererID()), false, {buttonWidth, iconHeight}, {0, 10.0f},
                                       buttonColN, KanVasX::Color::MultipliedValue(KanVasX::Color::Yellow, 1.4f), buttonColP))
      {
        window->Iconify();
      }
    }
    
    KanVasX::UI::SameLine();
    // Maximize Button
    {
      bool isMaximized = window->IsMaximized();
      if (KanVasX::UI::DrawButtonImage("Maximized/Restore",
                                       KanVasX::UI::GetTextureID(isMaximized ? m_iconRestore->GetRendererID() : m_iconMaximize->GetRendererID()),
                                       false, {buttonWidth, buttonHeight}, {0, -5.0f},
                                       buttonColN, KanVasX::Color::MultipliedValue(KanVasX::Color::Blue, 1.4f), buttonColP))
      {
        (isMaximized) ? window->Restore() : window->Maximize();
      }
    }
    
    KanVasX::UI::SameLine();
    // Close Button
    {
      if (KanVasX::UI::DrawButtonImage("Close", KanVasX::UI::GetTextureID(m_iconClose->GetRendererID()), false, {buttonWidth, buttonHeight}, {0, -5.0f},
                                       KanVasX::Color::Text, KanVasX::Color::MultipliedValue(KanVasX::Color::Red, 1.4f), buttonColP))
      {
        KanViz::Application::Get().Close();
      }
    }
  }
} // namespace KanVest
