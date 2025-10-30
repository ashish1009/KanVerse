//
//  RendererLayer.cpp
//  KanVest
//
//  Created by Ashish . on 29/10/25.
//

#include "RendererLayer.hpp"

#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <curl/curl.h>
#include <algorithm>

namespace KanVest
{
  static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
  }
  
  std::string fetchURL(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string response;
    if (curl) {
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
      curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
      curl_easy_perform(curl);
      curl_easy_cleanup(curl);
    }
    return response;
  }
  
  double extractValue(const std::string& text, const std::string& key) {
    std::string patternStr = "\"" + key + "\"\\s*:\\s*([-+]?[0-9]*\\.?[0-9]+)";
    std::regex pattern(patternStr);
    std::smatch match;
    if (std::regex_search(text, match, pattern))
      return std::stod(match[1].str());
    return -1;
  }
  
  std::string extractString(const std::string& text, const std::string& key) {
    std::string patternStr = "\"" + key + "\"\\s*:\\s*\"([^\"]+)\"";
    std::regex pattern(patternStr);
    std::smatch match;
    if (std::regex_search(text, match, pattern))
      return match[1].str();
    return "";
  }
  
  std::vector<double> extractArray(const std::string& text, const std::string& key) {
    std::vector<double> values;
    std::string patternStr = "\"" + key + "\"\\s*:\\s*\\[([^\\]]+)\\]";
    std::regex pattern(patternStr);
    std::smatch match;
    if (std::regex_search(text, match, pattern)) {
      std::string arr = match[1].str();
      std::regex numPattern("([-+]?[0-9]*\\.?[0-9]+)");
      for (std::sregex_iterator it(arr.begin(), arr.end(), numPattern), end; it != end; ++it)
        values.push_back(std::stod((*it)[1].str()));
    }
    return values;
  }
  
  static void SeparatorText(const char* label)
  {
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextUnformatted(label);
    ImGui::Separator();
    ImGui::Spacing();
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
  }
  
  void RendererLayer::OnDetach() noexcept
  {
    IK_PROFILE();
    IK_LOG_WARN("RendererLayer", "Detaching '{0}' Layer from application", GetName());
  }
  
  void RendererLayer::OnUpdate(const KanViz::TimeStep& ts)
  {
    IK_PERFORMANCE_FUNC("RendererLayer::OnUpdate");
  }
  
  void RendererLayer::OnImGuiRender()
  {
    UI_StartMainWindowDocking();
    UI_PrimaryViewportPanel();
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
    if (KanViz::Application::Get().GetSpecification().windowSpec.hideTitleBar)
    {
      float titlebarHeight = UI_DrawTitlebar();
      KanVasX::UI::SetCursorPosY(titlebarHeight + ImGui::GetCurrentWindow()->WindowPadding.y);
    }

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
    
  void RendererLayer::UI_PrimaryViewportPanel()
  {
    IK_PERFORMANCE_FUNC("RendererLayer::UI_PrimaryViewportPanel");
    
    static std::string symbol = "RELIANCE.NS";
    static double price = -1, high52 = -1, low52 = -1, volume = -1;
    static std::string currency = "";
    static double customHigh = -1, customLow = -1;
    static std::string lastUpdated = "Never";
    static float refreshInterval = 20.0f; // seconds
    static double lastUpdateTime = 0.0;
    static std::vector<double> closes;
    
    double currentTime = ImGui::GetTime();
    float timeSinceUpdate = static_cast<float>(currentTime - lastUpdateTime);
    float timeLeft = std::max(0.0f, refreshInterval - timeSinceUpdate);
    
    // Fetch + update
    auto updateData = [&]() {
      std::string liveURL = "https://query1.finance.yahoo.com/v8/finance/chart/" + symbol;
      std::string liveData = fetchURL(liveURL);
      
      // Auto fallback to .BO if .NS fails
      if (liveData.find("\"regularMarketPrice\"") == std::string::npos &&
          symbol.find(".NS") != std::string::npos)
      {
        std::string altSymbol = symbol.substr(0, symbol.find(".NS")) + ".BO";
        std::string altData = fetchURL("https://query1.finance.yahoo.com/v8/finance/chart/" + altSymbol);
        if (altData.find("\"regularMarketPrice\"") != std::string::npos) {
          symbol = altSymbol;
          liveData = altData;
        }
      }
      
      price = extractValue(liveData, "regularMarketPrice");
      high52 = extractValue(liveData, "fiftyTwoWeekHigh");
      low52 = extractValue(liveData, "fiftyTwoWeekLow");
      volume = extractValue(liveData, "regularMarketVolume");
      currency = extractString(liveData, "currency");
      
      // Custom date range (example: Jan–Mar 2024)
      long period1 = 1704067200; // 1 Jan 2024
      long period2 = 1711843200; // 31 Mar 2024
      std::string histURL =
      "https://query1.finance.yahoo.com/v8/finance/chart/" + symbol +
      "?period1=" + std::to_string(period1) +
      "&period2=" + std::to_string(period2) +
      "&interval=1d";
      
      std::string histData = fetchURL(histURL);
      closes = extractArray(histData, "close");
      
      if (!closes.empty()) {
        customHigh = *std::max_element(closes.begin(), closes.end());
        customLow = *std::min_element(closes.begin(), closes.end());
      } else {
        customHigh = customLow = -1;
      }
      
      // Timestamp
      time_t now = time(nullptr);
      char buf[128];
      strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S", localtime(&now));
      lastUpdated = buf;
      
      lastUpdateTime = ImGui::GetTime();
    };
    
    if (timeSinceUpdate >= refreshInterval)
      updateData();
    
    if (ImGui::Begin("📊 KanVest Stock Tracker", nullptr,
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));
      ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(100, 100, 255, 100));
      
      SeparatorText("Live Market Data");
      ImGui::Spacing();
      
      ImGui::PushItemWidth(160);
      ImGui::InputText("##Symbol", symbol.data(), 120);
      ImGui::SameLine();
      if (ImGui::Button("🔄 Refresh Now", ImVec2(120, 0)))
        updateData();
      ImGui::PopItemWidth();
      
      ImGui::SameLine();
      ImGui::TextDisabled("(Auto %.0fs)", refreshInterval);
      ImGui::Spacing();
      
      // --- Live Data Card ---
      ImGui::BeginChild("LiveData", ImVec2(0, 130), true, ImGuiWindowFlags_NoScrollbar);
      ImGui::Text("Symbol:   %s", symbol.c_str());
      ImGui::Text("Price:    %.2f %s", price, currency.c_str());
      ImGui::Text("Volume:   %.0f", volume);
      ImGui::Text("52W High: %.2f   |   52W Low: %.2f", high52, low52);
      ImGui::EndChild();
      
      ImGui::Spacing();
      SeparatorText("Custom Range (Jan–Mar 2024)");
      
      // --- Custom Range Card ---
      ImGui::BeginChild("CustomRange", ImVec2(0, 70), true);
      if (customHigh > 0 && customLow > 0)
        ImGui::Text("High: %.2f   |   Low: %.2f", customHigh, customLow);
      else
        ImGui::TextDisabled("No historical data available.");
      ImGui::EndChild();
      
      // 📊 Chart Button
      if (!closes.empty()) {
        ImGui::Spacing();
        if (ImGui::Button("📊 Open Full Chart", ImVec2(-1, 0)))
          ImGui::OpenPopup("FullChart");
      }
      
      // Popup Chart
      if (ImGui::BeginPopupModal("FullChart", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
      {
        if (ImPlot::BeginPlot(symbol.c_str(), ImVec2(600, 400))) {
          std::vector<double> days(closes.size());
          std::iota(days.begin(), days.end(), 0);
          ImPlot::SetupAxes("Day", "Price");
          ImPlot::PlotLine("Close", days.data(), closes.data(), (int)closes.size());
          ImPlot::EndPlot();
        }
        
        if (ImGui::Button("Close", ImVec2(120, 0)))
          ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
      }
      
      ImGui::Spacing();
      SeparatorText("Update Status");
      
      // --- Update status ---
      ImGui::BeginChild("UpdateInfo", ImVec2(0, 60), true);
      ImGui::Text("Last Updated: %s", lastUpdated.c_str());
      
      ImVec4 color = timeLeft > 5 ? ImVec4(0.4f, 0.9f, 0.4f, 1.0f)
      : ImVec4(1.0f, 0.7f, 0.3f, 1.0f);
      ImGui::TextColored(color, "Next Update In: %.1f s", timeLeft);
      ImGui::EndChild();
      
      ImGui::PopStyleColor();
      ImGui::PopStyleVar();
    }
    ImGui::End();
  }

  float RendererLayer::UI_DrawTitlebar()
  {
    IK_PERFORMANCE_FUNC("RendererLayer::UI_DrawTitlebar");
    
    static constexpr float titleBarHeight = 45.0f;
//    const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;
//    
//    // Drag and Control the window with user title bar ----------------------------------
//    UI_TitlebarDragArea(titleBarHeight);
//    
//    // Title bar rectangle --------------------------------------------------------------
//    UI::SetCursorPos(windowPadding);
//    UI::DrawRect(IM_COL32(0, 0, 0, 0), titleBarHeight);
//    
//    // Draw Kreator Logo ---------------------------------------------------------------
//    ImGui::SetItemAllowOverlap();
//    UI::SetCursorPos(windowPadding);
//    static const ImVec2 size = {titleBarHeight - 10.0f, titleBarHeight - 10.0f};
//    if (UI::DrawButtonImage("MainMenu", m_applicationIcon, false, size, {10.0f, 5.0f}, UI::Color::Highlight))
//    {
//      ImGui::OpenPopup("MainMenu");
//    }
//    
//    if (UI::BeginPopup("MainMenu"))
//    {
//      ImGui::PushStyleColor(ImGuiCol_HeaderHovered, UI::Color::Highlight);
//      static const IKan::Window* window = IKan::Application::Get().GetWindow();
//      if (ImGui::MenuItem("Minimize"))
//      {
//        window->Iconify();
//      }
//      if (ImGui::MenuItem("Maximize", nullptr, false, !window->IsMaximized()))
//      {
//        window->Maximize();
//      }
//      ImGui::Separator();
//      if (ImGui::MenuItem("Close", "Cmd + Q"))
//      {
//        IKan::Application::Get().Close();
//      }
//      ImGui::PopStyleColor();
//      UI::EndPopup();
//    }
//    
//    // Studio Name -------------------------------------------------------------------------
//    UI::SetCursorPos({0.0f, 0.0f});
//    UI::Text(UI::FontType::SemiHeader, "IKan-Studio", UI::AlignX::Center, {0.0f, 12.0f}, UI::Color::Highlight);
//    
//    // Title Rectangles --------------------------------------------------------------------
//    UI::SetCursorPos({ImGui::GetWindowWidth() / 4, windowPadding.y});
//    UI::DrawRect(UI::Color::Alpha(UI::Color::HighlightMuted, 0.4f), titleBarHeight / 2, 0.5f, {0.0f, titleBarHeight / 4});
//    
//    // Project Name ------------------------------------------------------------------------
//    UI::SetCursorPos({ImGui::GetWindowWidth() / 4, 0.0f});
//    const std::string projectName = IKan::Project::GetActive()->GetConfig().name;
//    UI::Text(UI::FontType::Large, projectName, UI::AlignX::Left, {5.0f, 14.0f}, UI::Color::Alpha(UI::Color::Text, 0.4f));
//    UI::Tooltip("Current project (" + IKan::Project::GetActive()->GetConfig().projectFileName + ")");
//    
//    // Current Scene name ---------------------------------------------------------------
//    UI::SetCursorPos({3 * ImGui::GetWindowWidth() / 4, 0.0f});
//    const std::string sceneName = m_currentScene->GetName();
//    UI::Text(UI::FontType::Large, sceneName, UI::AlignX::Left, {-ImGui::CalcTextSize(sceneName.c_str()).x * 1.2f, 14.0f},
//             m_sceneFilePath == "" ? UI::Color::Alpha(UI::Color::Error, 0.4f) : UI::Color::Alpha(UI::Color::Text, 0.4f));
//    if (m_sceneFilePath == "")
//    {
//      UI::Tooltip("Unsaved Scene");
//    }
//    else
//    {
//      UI::Tooltip("Current scene (" + m_sceneFilePath.string() + ")");
//    }
//    
//    // Draw the Menu Tab in Title bar --------------------------------------------------
//    UI::SetCursorPosX(60.0f);
//    UI::SetCursorPosY(titleBarHeight / 4);
//    UI_MenuBar(titleBarHeight);
//    
//    // Render the Window Buttons -------------------------------------------------------
//    UI::SetCursorPosX(ImGui::GetWindowWidth() - 78);
//    UI::SetCursorPosY(10.0f);
//    UI_WindowButtons();
//    
    return titleBarHeight;
  }
  
  void RendererLayer::UI_TitlebarDragArea(float titlebarHeight)
  {
//    static float moveOffsetX;
//    static float moveOffsetY;
//    
//    auto* rootWindow = ImGui::GetCurrentWindow()->RootWindow;
//    const float windowWidth = (int32_t)rootWindow->RootWindow->Size.x;
//    
//    if (ImGui::InvisibleButton("##titleBarDragZone", ImVec2(ImGui::GetContentRegionAvail().x, titlebarHeight * 2), ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_AllowItemOverlap))
//    {
//      ImVec2 point = ImGui::GetMousePos();
//      ImRect rect = rootWindow->Rect();
//      
//      // Calculate the difference between the cursor pos and window pos
//      moveOffsetX = point.x - rect.Min.x;
//      moveOffsetY = point.y - rect.Min.y;
//    }
//    
//    const IKan::Window* window = IKan::Application::Get().GetWindow();
//    bool maximized = window->IsMaximized();
//    
//    // Maximize or restore on doublt click
//    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) and ImGui::IsItemHovered())
//    {
//      (maximized) ? window->Restore() : window->Maximize();
//    }
//    else if (ImGui::IsItemActive())
//    {
//      if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
//      {
//        if (maximized)
//        {
//          window->Restore();
//          
//          // Get the original size of window
//          [[maybe_unused]] uint32_t newWidth = window->GetWidth();
//          [[maybe_unused]] uint32_t newHeight = window->GetHeight();
//          
//          // Offset position proportionally to mouse position on titlebar
//          // This ensures we dragging window relatively to cursor position on titlebar
//          // correctly when window size changes
//          if (windowWidth - (float)newWidth > 0.0f)
//          {
//            moveOffsetX *= (float)newWidth / windowWidth;
//          }
//        }
//        
//        // Update the new position of window
//        ImVec2 point = ImGui::GetMousePos();
//        window->SetPosition({point.x - moveOffsetX, point.y - moveOffsetY});
//      }
//    }
  }
//  
//  void RendererLayer::UI_MenuBar(float frameeHeight)
//  {
//    IK_PERFORMANCE("RendererLayer::UI_MenuBar")
//    
//    // Menu Bar Rectactangle Size
//    const ImRect menuBarRect =
//    {
//      ImGui::GetCursorPos(), // Min Rect Coord
//      {ImGui::GetContentRegionAvail().x, frameeHeight} // Max Rect Coord
//    };
//    
//    ImGui::BeginGroup();
//    if (UI::BeginMenuBar(menuBarRect))
//    {
//      bool menuOpen = ImGui::IsPopupOpen("##menubar", ImGuiPopupFlags_AnyPopupId);
//      
//      // Push the Colors if Menu is active
//      if (menuOpen)
//      {
//        ImGui::PushStyleColor(ImGuiCol_Header, UI::Color::Highlight);
//        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, UI::Color::Highlight);
//      }
//      
//      static auto popItemHighlight = [&menuOpen]
//      {
//        if (menuOpen)
//        {
//          ImGui::PopStyleColor(2);
//          menuOpen = false;
//        }
//      };
//      
//      // Menu Items
//      UI_Utils::AddMenu("File", popItemHighlight, [this]() {
//        // Project options ----------------------
//        if (ImGui::MenuItem("Create Project...", "Cmd + Shift + N"))
//        {
//          FolderExplorer::ShowCreatePopup("Create Project", m_directories.studioPath / "Kreator", nullptr);
//          m_folderExplorerAction = FolderExplorerAction::NewProject;
//        }
//        if (ImGui::MenuItem("Open Project...", "Cmd + Shift + O"))
//        {
//          FolderExplorer::ShowOpenPopup("Open Project", IKan::ProjectExtension, "", nullptr);
//          m_folderExplorerAction = FolderExplorerAction::OpenProject;
//        }
//        
//        UI_Utils::ScopedMenu("Open Recent", [this]() {
//          size_t i = 0;
//          std::string openProjectFilePath = "";
//          for (auto it = m_userPreferences->recentProjects.begin(); it != m_userPreferences->recentProjects.end(); it++)
//          {
//            // Show only 10 Items
//            if (i > 10)
//            {
//              break;
//            }
//            
//            if (std::filesystem::exists(it->second.filePath))
//            {
//              ImGui::PushStyleColor(ImGuiCol_HeaderHovered, UI::Color::HighlightMuted);
//              ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::Text);
//              if (ImGui::MenuItem(it->second.name.c_str()))
//              {
//                RecentProject projectEntry;
//                projectEntry.name = it->second.name;
//                projectEntry.filePath = it->second.filePath;
//                projectEntry.lastOpened = time(NULL);
//                
//                it = m_userPreferences->recentProjects.erase(it);
//                
//                m_userPreferences->recentProjects[projectEntry.lastOpened] = projectEntry;
//                
//                UserPreferencesSerializer preferencesSerializer(m_userPreferences);
//                preferencesSerializer.Serialize(m_userPreferences->filePath);
//                
//                OpenProject(projectEntry.filePath, false);
//                ImGui::PopStyleColor(2);
//                break;
//              }
//              ImGui::PopStyleColor(2);
//            }
//            else
//            {
//              m_userPreferences->recentProjects.erase(it);
//              UserPreferencesSerializer serializer(m_userPreferences);
//              serializer.Serialize(m_userPreferences->filePath);
//              break;
//            }
//            
//            i++;
//          }
//        });
//        ImGui::Separator();
//        
//        // Scene Options ------------------------
//        if (ImGui::MenuItem("New Scene", "Cmd+N"))
//        {
//          NewScenePopup();
//        }
//        if (ImGui::MenuItem("Open Scene", "Cmd+O"))
//        {
//          OpenScene();
//        }
//        if (ImGui::MenuItem("Save Scene", "Cmd+S"))
//        {
//          SaveScene();
//        }
//        if (ImGui::MenuItem("Save Scene As", "Cmd+Shift+S"))
//        {
//          SaveSceneAs();
//        }
//        ImGui::Separator();
//      });
//      
//      static bool serializePreferences = false;
//      UI_Utils::AddMenu("Editor", popItemHighlight, [this]() {
//      });
//      
//      UI_Utils::AddMenu("View", popItemHighlight, [this]() {
//        for (auto& [id, panelData] : m_panels.GetPanels())
//        {
//          ImGui::MenuItem(panelData.name, nullptr, &panelData.isOpen);
//        }
//        bool modified = false;
//        if (ImGui::MenuItem("Performance", nullptr, &m_userPreferences->showPerformancePanel))
//        {
//          modified = true;
//        }
//        if (ImGui::MenuItem("Scene Settings", nullptr, &m_userPreferences->showSceneSettings))
//        {
//          modified = true;
//        }
//        
//        if (modified)
//        {
//          UserPreferencesSerializer serializer(m_userPreferences);
//          serializer.Serialize(m_userPreferences->filePath);
//        }
//        ImGui::Separator();
//      });
//      
//      UI_Utils::AddMenu("Debug", popItemHighlight, [this]() {
//        bool modified = false;
//        
//        if (ImGui::MenuItem("Show System Info", nullptr, &m_userPreferences->renderSystemInfo))
//        {
//          modified = true;
//        }
//        if (ImGui::MenuItem("Show Collider", nullptr, &m_userPreferences->showColliders))
//        {
//          modified = true;
//        }
//        if (ImGui::MenuItem("Show Relationship Connection", nullptr, &m_userPreferences->showRelationshipConnection))
//        {
//          modified = true;
//        }
//        if (ImGui::MenuItem("Show Joint Connection", nullptr, &m_userPreferences->showJointConnections))
//        {
//          modified = true;
//        }
//        if (ImGui::MenuItem("Show Camera Path", nullptr, &m_userPreferences->showDebugCameraControllerPath))
//        {
//          modified = true;
//        }
//        if (ImGui::MenuItem("Show Mini viewport", nullptr, &m_userPreferences->showMiniViewport))
//        {
//          modified = true;
//        }
//        
//        if (modified)
//        {
//          UserPreferencesSerializer serializer(m_userPreferences);
//          serializer.Serialize(m_userPreferences->filePath);
//        }
//      });
//      
//      UI_Utils::AddMenu("Help", popItemHighlight, [this]() {
//      });
//      
//      if (menuOpen)
//      {
//        ImGui::PopStyleColor(2);
//      }
//    }
//    UI::EndMenuBar();
//    ImGui::EndGroup();
//  }
//  
  void RendererLayer::UI_WindowButtons()
  {
    IK_PERFORMANCE_FUNC("RendererLayer::UI_WindowButtons");
//    // Window buttons
//    static const ImU32 buttonColN = UI::Color::MultipliedValue(UI::Color::Text, 0.9f);
//    static const ImU32 buttonColH = UI::Color::MultipliedValue(UI::Color::Text, 1.2f);
//    static const ImU32 buttonColP = UI::Color::TextDark;
//    static const float buttonWidth = 14.0f;
//    static const float buttonHeight = 14.0f;
//    static const IKan::Window* window = IKan::Application::Get().GetWindow();
//    
//    // Minimize Button
//    {
//      static const float iconHeight = m_iconMinimize->GetHeight();
//      const float padY = (buttonHeight - (float)iconHeight) / 2.0f;
//      
//      if (UI::DrawButtonImage("Minimize", m_iconMinimize, false, {buttonWidth, iconHeight}, {0, 10.0f}, buttonColN, UI::Color::MultipliedValue(UI::Color::Yellow, 1.4f), buttonColP))
//      {
//        window->Iconify();
//      }
//    }
//    
//    UI::SameLine();
//    // Maximize Button
//    {
//      bool isMaximized = window->IsMaximized();
//      if (UI::DrawButtonImage("Maximized/Restore", isMaximized ? m_iconRestore : m_iconMaximize, false, {buttonWidth, buttonHeight}, {0, -5.0f}, buttonColN, UI::Color::MultipliedValue(UI::Color::Blue, 1.4f), buttonColP))
//      {
//        (isMaximized) ? window->Restore() : window->Maximize();
//      }
//    }
//    
//    UI::SameLine();
//    // Close Button
//    {
//      if (UI::DrawButtonImage("Close", m_iconClose, false, {buttonWidth, buttonHeight}, {0, -5.0f}, UI::Color::Text, UI::Color::MultipliedValue(UI::Color::Red, 1.4f), buttonColP))
//      {
//        IKan::Application::Get().Close();
//      }
//    }
  }
} // namespace KanVest
