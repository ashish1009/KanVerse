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
    
    double currentTime = ImGui::GetTime(); // seconds since app start
    float timeSinceUpdate = static_cast<float>(currentTime - lastUpdateTime);
    float timeLeft = std::max(0.0f, refreshInterval - timeSinceUpdate);
    
    // Function to fetch + update data
    auto updateData = [&]() {
      std::string liveURL = "https://query1.finance.yahoo.com/v8/finance/chart/" + symbol;
      std::string liveData = fetchURL(liveURL);
      
      price = extractValue(liveData, "regularMarketPrice");
      high52 = extractValue(liveData, "fiftyTwoWeekHigh");
      low52 = extractValue(liveData, "fiftyTwoWeekLow");
      volume = extractValue(liveData, "regularMarketVolume");
      currency = extractString(liveData, "currency");
      
      // --- Custom range (example: Jan–Mar 2024) ---
      long period1 = 1704067200; // 1 Jan 2024
      long period2 = 1711843200; // 31 Mar 2024
      std::string histURL =
      "https://query1.finance.yahoo.com/v8/finance/chart/" + symbol +
      "?period1=" + std::to_string(period1) +
      "&period2=" + std::to_string(period2) +
      "&interval=1d";
      
      std::string histData = fetchURL(histURL);
      std::vector<double> closes = extractArray(histData, "close");
      
      if (!closes.empty()) {
        customHigh = *std::max_element(closes.begin(), closes.end());
        customLow = *std::min_element(closes.begin(), closes.end());
      }
      
      // Record update time with full date/time
      time_t now = time(nullptr);
      char buf[128];
      strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S", localtime(&now));
      // Example: Thu, 30 Oct 2025 14:25:18
      lastUpdated = buf;
      
      lastUpdateTime = ImGui::GetTime();
    };

    // Auto refresh every 20 seconds
    if (timeSinceUpdate >= refreshInterval)
      updateData();
    
    if (ImGui::Begin("KanVest"))
    {
      ImGui::Separator();
      ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
      ImGui::Separator();
      
      ImGui::InputText("Symbol", symbol.data(), 120);
      
      ImGui::Text("Symbol: %s", symbol.c_str());
      ImGui::Text("Current Price: %.2f %s", price, currency.c_str());
      ImGui::Text("Volume: %.0f", volume);
      ImGui::Text("52W High: %.2f | 52W Low: %.2f", high52, low52);
      
      ImGui::Separator();
      ImGui::Text("=== CUSTOM RANGE (Jan–Mar 2024) ===");
      if (customHigh > 0 && customLow > 0)
        ImGui::Text("High: %.2f | Low: %.2f", customHigh, customLow);
      else
        ImGui::Text("No historical data available.");
      
      ImGui::Separator();
      ImGui::Text("Last Updated: %s", lastUpdated.c_str());
      ImGui::Text("Next update in: %.1f s", timeLeft);
      
      // Manual refresh button
      if (ImGui::Button("Refresh Now"))
        updateData();
      
      ImGui::SameLine();
      ImGui::TextDisabled("(Auto every %.0f s)", refreshInterval);
    }
    ImGui::End();
  }

} // namespace KanVest
