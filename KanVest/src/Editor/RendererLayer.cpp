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
  static const std::filesystem::path KanVestResourcePath = "../../../KanVest/Resources";
  // Kretor Resource Path
#define KanVestResourcePath(path) std::filesystem::absolute(KanVestResourcePath / path)

  // Kreate Texture
#define CreateTexture(path) KanViz::TextureFactory::Create(KanVestResourcePath(path))

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
    KanVasX::UI::Separator();
    ImGui::TextUnformatted(label);
    KanVasX::UI::Separator();
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
    
    // Load Textures -----------------------------------------------------------------------------
    m_applicationIcon = CreateTexture("Textures/Logo/KanVest.png");
    
    // Window Icons
    m_iconClose = CreateTexture("Textures/Icons/Close.png");
    m_iconMinimize = CreateTexture("Textures/Icons/Minimize.png");
    m_iconMaximize = CreateTexture("Textures/Icons/Maximize.png");
    m_iconRestore = CreateTexture("Textures/Icons/Restore.png");
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
    
    // Set the Imgui theme ----------------------------------------------------------------------
    KanVest::UI::Font::Load({
      {UI::FontType::Regular,                 {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         14}},
      {UI::FontType::Light,                   {KanVestResourcePath("Fonts/Opensans/Light.ttf"),           14}},
      {UI::FontType::Italic,                  {KanVestResourcePath("Fonts/Opensans/Italic.ttf"),          14}},
      {UI::FontType::LightItalic,             {KanVestResourcePath("Fonts/Opensans/LightItalic.ttf"),     14}},
      {UI::FontType::SemiBold,                {KanVestResourcePath("Fonts/Opensans/SemiBold.ttf"),        14}},
      {UI::FontType::Bold,                    {KanVestResourcePath("Fonts/Opensans/Bold.ttf"),            14}},
      {UI::FontType::SemiBoldItalic,          {KanVestResourcePath("Fonts/Opensans/SemiBoldItalic.ttf"),  14}},
      {UI::FontType::BoldItalic,              {KanVestResourcePath("Fonts/Opensans/BoldItalic.ttf"),      14}},
      {UI::FontType::ExtraBold,               {KanVestResourcePath("Fonts/Opensans/ExtraBold.ttf"),       14}},
      {UI::FontType::ExtraBoldItalic,         {KanVestResourcePath("Fonts/Opensans/ExtraBoldItalic.ttf"), 14}},
      
      {UI::FontType::FixedWidthRegular,       {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       10}},
      {UI::FontType::FixedWidthLight,         {KanVestResourcePath("Fonts/HfMonorita/Light.ttf"),         10}},
      {UI::FontType::FixedWidthMedium,        {KanVestResourcePath("Fonts/HfMonorita/Medium.ttf"),        10}},
      {UI::FontType::FixedWidthBold,          {KanVestResourcePath("Fonts/HfMonorita/Bold.ttf"),          10}},
      
      {UI::FontType::Small,                   {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         12}},
      {UI::FontType::Medium,                  {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         13}},
      {UI::FontType::Large,                   {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         16}},
      {UI::FontType::ExtraLarge,              {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         18}},
      {UI::FontType::SemiHeader,              {KanVestResourcePath("Fonts/Opensans/SemiBold.ttf"),        20}},
      {UI::FontType::Header,                  {KanVestResourcePath("Fonts/Opensans/Bold.ttf"),            30}},
      {UI::FontType::HugeHeader,              {KanVestResourcePath("Fonts/Opensans/ExtraBold.ttf"),       40}},
    });
    
    KanVasX::Color::Initialize();
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
    const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;
    
    // Drag and Control the window with user title bar ----------------------------------
    UI_TitlebarDragArea(titleBarHeight);
    
    // Title bar rectangle --------------------------------------------------------------
    KanVasX::UI::SetCursorPos(windowPadding);
    KanVasX::UI::DrawFilledRect(KanVasX::Color::TitleBar, titleBarHeight);

    // Draw KanVest Logo ----------------------------------------------------------------
    ImGui::SetItemAllowOverlap();
    KanVasX::UI::SetCursorPos(windowPadding);
    static const ImVec2 size = {titleBarHeight - 10.0f, titleBarHeight - 10.0f};
    if (KanVasX::UI::DrawButtonImage("MainMenu", KanVasX::UI::GetTextureID(m_applicationIcon->GetRendererID()), false, size, {10.0f, 5.0f}, KanVasX::Color::White))
    {
      ImGui::OpenPopup("MainMenu");
    }

    // Studio Name -------------------------------------------------------------------------
    KanVasX::UI::SetCursorPos({0.0f, 0.0f});
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header), "KanVest", KanVasX::UI::AlignX::Center, {0.0f, 12.0f}, KanVasX::Color::Highlight);
    
    // Title Rectangles --------------------------------------------------------------------
    KanVasX::UI::SetCursorPos({ImGui::GetWindowWidth() / 4, windowPadding.y});
    KanVasX::UI::DrawRect(KanVasX::Color::Alpha(KanVasX::Color::HighlightMuted, 0.4f), titleBarHeight / 1.2, 0.5f, {0.0f, titleBarHeight / 4});
    
    // Render the Window Buttons -------------------------------------------------------
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
    IK_PERFORMANCE_FUNC("RendererLayer::UI_WindowButtons");
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
