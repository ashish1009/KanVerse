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

  static time_t parseDateYYYYMMDD(const std::string &s) {
    // Accepts "YYYY-MM-DD", returns time_t for 00:00:00 local time on that date
    std::tm tm = {};
    if (s.size() != 10) return 0;
    std::istringstream iss(s);
    iss >> std::get_time(&tm, "%Y-%m-%d");
    if (iss.fail()) return 0;
    tm.tm_hour = 0; tm.tm_min = 0; tm.tm_sec = 0;
    // mktime assumes localtime. Yahoo wants UTC timestamps, but using local is usually fine for daily ranges.
    return mktime(&tm);
  }

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
    
    UI_PrimaryViewportPanel_DEMO();
    UI_StockAnalyzer();
    
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
  
  void RendererLayer::UI_StockAnalyzer()
  {
    IK_PERFORMANCE_FUNC("RendererLayer::UI_StockAnalyzer");
    
    ImGui::Begin("KanVest Stock Analyzer");
    {      
    }
    ImGui::End();
  }

  void RendererLayer::UI_PrimaryViewportPanel_DEMO()
  {
    IK_PERFORMANCE_FUNC("RendererLayer::UI_PrimaryViewportPanel");
    
    // Buffers / state
    static std::string symbol = "RELIANCE.NS";
    static char symbolBuf[64] = "RELIANCE.NS";
    static double price = -1, openPrice = -1, dayHigh = -1, dayLow = -1, prevClose = -1;
    static double change = 0.0, changePercent = 0.0;
    static double fiftyTwoHigh = -1, fiftyTwoLow = -1;
    static double marketCap = -1, trailingPE = -1, forwardPE = -1;
    static double dividendRate = -1, dividendYield = -1;
    static double avgVolume3m = -1, avgVolume10day = -1;
    static double regularMarketVolume = -1;
    static std::string currency = "", exchangeName = "", fullExchangeName = "", shortName = "", longName = "", marketState = "";
    static std::vector<double> closes, opens, highs, lows, vols;
    static double customHigh = -1, customLow = -1;
    static std::string lastUpdated = "Never";
    static float refreshInterval = 2000.0f;
    static double lastUpdateTime = 0.0;
    static bool showOpen = false, showHigh = false, showLow = false, showClose = true, showVolume = false;
    static std::string startDate = "2024-01-01", endDate = "2024-03-31";
    static bool autoFallbackBO = true;
    
    double currentTime = ImGui::GetTime();
    float timeSinceUpdate = static_cast<float>(currentTime - lastUpdateTime);
    float timeLeft = std::max(0.0f, refreshInterval - timeSinceUpdate);
    
    // Helper to convert & format timestamp -> readable
    auto formatNow = []() -> std::string {
      time_t now = time(nullptr);
      char buf[128];
      strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S", localtime(&now));
      return std::string(buf);
    };
    
    // Full query/update routine
    auto updateData = [&]() {
      // sync symbolBuf -> symbol
      symbol = std::string(symbolBuf);
      
      std::string liveURL = "https://query1.finance.yahoo.com/v8/finance/chart/" + symbol;
      std::string liveData = fetchURL(liveURL);
      
      // If liveData doesn't contain usual fields, try .BO fallback for Indian stocks
      if (liveData.find("\"regularMarketPrice\"") == std::string::npos && autoFallbackBO && symbol.find(".NS") != std::string::npos) {
        std::string altSymbol = symbol.substr(0, symbol.find(".NS")) + ".BO";
        std::string altData = fetchURL("https://query1.finance.yahoo.com/v8/finance/chart/" + altSymbol);
        if (altData.find("\"regularMarketPrice\"") != std::string::npos) {
          symbol = altSymbol;
          std::snprintf(symbolBuf, sizeof(symbolBuf), "%s", symbol.c_str());
          liveData = altData;
        }
      }
      
      // --- Meta fields (many common ones) ---
      price = extractValue(liveData, "regularMarketPrice");
      openPrice = extractValue(liveData, "regularMarketOpen");
      dayHigh = extractValue(liveData, "regularMarketDayHigh");
      dayLow = extractValue(liveData, "regularMarketDayLow");
      prevClose = extractValue(liveData, "chartPreviousClose");
      
      change = price - prevClose;
      // some endpoints include percent field, try reading directly:
      changePercent = extractValue(liveData, "regularMarketChangePercent");
      if (changePercent == -1 && prevClose > 0) changePercent = (change / prevClose) * 100.0;
      
      fiftyTwoHigh = extractValue(liveData, "fiftyTwoWeekHigh");
      fiftyTwoLow = extractValue(liveData, "fiftyTwoWeekLow");
      
      regularMarketVolume = extractValue(liveData, "regularMarketVolume");
      avgVolume3m = extractValue(liveData, "averageDailyVolume3Month");
      avgVolume10day = extractValue(liveData, "averageDailyVolume10Day");
      
      marketCap = extractValue(liveData, "marketCap");
      trailingPE = extractValue(liveData, "trailingPE");
      forwardPE = extractValue(liveData, "forwardPE");
      
      dividendRate = extractValue(liveData, "dividendRate");
      dividendYield = extractValue(liveData, "dividendYield");
      
      currency = extractString(liveData, "currency");
      exchangeName = extractString(liveData, "exchangeName");
      fullExchangeName = extractString(liveData, "fullExchangeName");
      shortName = extractString(liveData, "shortName");
      longName = extractString(liveData, "longName");
      marketState = extractString(liveData, "marketState");
      
      // --- Historical range (user controlled) ---
      time_t s = parseDateYYYYMMDD(startDate);
      time_t e = parseDateYYYYMMDD(endDate);
      if (s == 0 || e == 0 || e <= s) {
        // If bad input, fallback to default 90 days
        e = time(nullptr);
        s = e - 60*60*24*90;
      }
      
      // Use period1= start (UTC), period2 = end (UTC) + one day to include end date
      long period1 = static_cast<long>(s);
      long period2 = static_cast<long>(e + 60*60*24);
      
      std::string histURL = "https://query1.finance.yahoo.com/v8/finance/chart/" + symbol +
      "?period1=" + std::to_string(period1) +
      "&period2=" + std::to_string(period2) +
      "&interval=1d";
      std::string histData = fetchURL(histURL);
      
      // parse arrays
      closes = extractArray(histData, "close");
      opens = extractArray(histData, "open");
      highs = extractArray(histData, "high");
      lows = extractArray(histData, "low");
      vols = extractArray(histData, "volume"); // returns as double but volumes are ints
      
      if (!closes.empty()) {
        customHigh = *std::max_element(closes.begin(), closes.end());
        customLow = *std::min_element(closes.begin(), closes.end());
      } else {
        customHigh = customLow = -1;
      }
      
      lastUpdated = formatNow();
      lastUpdateTime = ImGui::GetTime();
    };
    
    // Auto refresh
    if (timeSinceUpdate >= refreshInterval)
      updateData();
    
    // UI
    ImGui::Begin("📊 KanVest Stock Tracker");
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6,6));
      
      // Top row: symbol input + refresh + controls
      ImGui::Columns(3, "topcols", false);
      ImGui::SetColumnWidth(0, 260);
      ImGui::InputText("Symbol", symbolBuf, sizeof(symbolBuf)); // syncs to symbol on update
      ImGui::NextColumn();
      
      if (ImGui::Button("🔄 Refresh Now")) { updateData(); }
      ImGui::SameLine();
      ImGui::Checkbox("Fallback .BO", &autoFallbackBO);
      ImGui::NextColumn();
      
      ImGui::TextDisabled("Auto: %.0fs", refreshInterval);
      ImGui::SameLine();
      if (ImGui::SmallButton("-")) refreshInterval = std::max(1.0f, refreshInterval - 5.0f);
      ImGui::SameLine();
      if (ImGui::SmallButton("+")) refreshInterval += 5.0f;
      
      ImGui::Columns(1);
      ImGui::Spacing();
      
      // Big two-column layout: left live metrics, right chart controls & small sparkline
      ImGui::Columns(2, "maincols", true);
      // LEFT: Live metrics
      ImGui::BeginChild("LeftMetrics", ImVec2(0,0), true);
      ImGui::Text("%s (%s)", shortName.empty() ? symbol.c_str() : shortName.c_str(), currency.c_str());
      ImGui::TextWrapped("%s", longName.c_str());
      ImGui::Separator();
      
      ImGui::Columns(2, "vals", false);
      ImGui::TextDisabled("Price"); ImGui::NextColumn(); ImGui::Text("%.2f", price); ImGui::NextColumn();
      ImGui::TextDisabled("Open"); ImGui::NextColumn(); ImGui::Text("%.2f", openPrice); ImGui::NextColumn();
      ImGui::TextDisabled("Day High"); ImGui::NextColumn(); ImGui::Text("%.2f", dayHigh); ImGui::NextColumn();
      ImGui::TextDisabled("Day Low"); ImGui::NextColumn(); ImGui::Text("%.2f", dayLow); ImGui::NextColumn();
      ImGui::TextDisabled("Prev Close"); ImGui::NextColumn(); ImGui::Text("%.2f", prevClose); ImGui::NextColumn();
      ImGui::TextDisabled("Change"); ImGui::NextColumn(); ImGui::Text("%.2f (%.2f%%)", change, changePercent); ImGui::NextColumn();
      
      ImGui::TextDisabled("Volume"); ImGui::NextColumn(); ImGui::Text("%.0f", regularMarketVolume); ImGui::NextColumn();
      ImGui::TextDisabled("AvgVol(3M)"); ImGui::NextColumn(); ImGui::Text("%.0f", avgVolume3m); ImGui::NextColumn();
      ImGui::TextDisabled("AvgVol(10d)"); ImGui::NextColumn(); ImGui::Text("%.0f", avgVolume10day); ImGui::NextColumn();
      
      ImGui::TextDisabled("MarketCap"); ImGui::NextColumn(); ImGui::Text("%.0f", marketCap); ImGui::NextColumn();
      ImGui::TextDisabled("Trailing PE"); ImGui::NextColumn(); ImGui::Text("%.2f", trailingPE); ImGui::NextColumn();
      ImGui::TextDisabled("Forward PE"); ImGui::NextColumn(); ImGui::Text("%.2f", forwardPE); ImGui::NextColumn();
      
      ImGui::TextDisabled("Dividend Rate"); ImGui::NextColumn(); ImGui::Text("%.4f", dividendRate); ImGui::NextColumn();
      ImGui::TextDisabled("Dividend Yield"); ImGui::NextColumn(); ImGui::Text("%.4f", dividendYield); ImGui::NextColumn();
      
      ImGui::TextDisabled("Exchange"); ImGui::NextColumn(); ImGui::Text("%s", exchangeName.c_str()); ImGui::NextColumn();
      ImGui::TextDisabled("Full Exchange"); ImGui::NextColumn(); ImGui::Text("%s", fullExchangeName.c_str()); ImGui::NextColumn();
      ImGui::TextDisabled("Market State"); ImGui::NextColumn(); ImGui::Text("%s", marketState.c_str()); ImGui::NextColumn();
      
      ImGui::Columns(1);
      ImGui::EndChild();
      
      // RIGHT: Chart controls, date range, sparkline
      ImGui::NextColumn();
      ImGui::BeginChild("RightChart", ImVec2(0,0), true);
      
      ImGui::Text("Custom Date Range (YYYY-MM-DD)");
      ImGui::InputText("Start", &startDate[0], 11); // small hack: input directly to string buffer
      // safer approach: use a separate char buffer; for brevity we reuse string's internal buffer — ensure length >= 11
      ImGui::SameLine();
      ImGui::InputText("End", &endDate[0], 11);
      ImGui::SameLine();
      if (ImGui::SmallButton("Fetch Range")) updateData();
      
      // Series toggles
      ImGui::Separator();
      ImGui::Text("Chart Series");
      ImGui::Checkbox("Open", &showOpen); ImGui::SameLine();
      ImGui::Checkbox("High", &showHigh); ImGui::SameLine();
      ImGui::Checkbox("Low", &showLow); ImGui::SameLine();
      ImGui::Checkbox("Close", &showClose); ImGui::SameLine();
      ImGui::Checkbox("Volume", &showVolume);
      
      // small inline sparkline of closes
      if (!closes.empty()) {
        std::vector<float> cf(closes.begin(), closes.end());
        ImGui::PlotLines("Close (spark)", cf.data(), (int)cf.size(), 0, nullptr,
                         *std::min_element(cf.begin(), cf.end()), *std::max_element(cf.begin(), cf.end()), ImVec2(0,80));
      } else {
        ImGui::TextDisabled("No historical data yet for sparkline.");
      }
      
      // full chart button
      if (!closes.empty()) {
        if (ImGui::Button("📊 Open Full Chart", ImVec2(-1, 0)))
          ImGui::OpenPopup("FullChart");
      }
      
      ImGui::Separator();
      ImGui::Text("Last Updated: %s", lastUpdated.c_str());
      ImGui::TextColored(timeLeft > 5 ? ImVec4(0.4f,0.9f,0.4f,1.0f) : ImVec4(1.0f,0.7f,0.3f,1.0f),
                         "Next Update In: %.1f s", timeLeft);
      
      ImGui::EndChild(); // RightChart
      
      ImGui::Columns(1);
      
      ImGui::PopStyleVar();
      
      // --- Popup full chart using ImPlot ---
      if (ImGui::BeginPopupModal("FullChart", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
      {
        if (closes.empty()) {
          ImGui::Text("No historical data to chart.");
        } else if (!ImPlot::GetCurrentContext()) {
          ImGui::TextColored(ImVec4(1,0.4f,0.4f,1.0f),
                             "ImPlot context not found. Call ImPlot::CreateContext() at app init.");
        } else {
          // Prepare X axis days (index) and selected Y arrays
          std::vector<double> x(closes.size());
          std::iota(x.begin(), x.end(), 0);
          ImPlot::SetNextAxisLimits(ImAxis_X1, 0, (double)closes.size() - 1, ImGuiCond_Always);

          if (ImPlot::BeginPlot(symbol.c_str(), ImVec2(800, 400))) {
            ImPlot::SetupAxis(ImAxis_X1, "Day");
            ImPlot::SetupAxis(ImAxis_Y1, "Price");

            // Plot price series (choose colors)
            if (showClose) ImPlot::PlotLine("Close", x.data(), closes.data(), (int)closes.size());
            if (showOpen && opens.size() == closes.size()) ImPlot::PlotLine("Open", x.data(), opens.data(), (int)opens.size());
            if (showHigh && highs.size() == closes.size()) ImPlot::PlotLine("High", x.data(), highs.data(), (int)highs.size());
            if (showLow && lows.size() == closes.size()) ImPlot::PlotLine("Low", x.data(), lows.data(), (int)lows.size());
            
            // Optionally overlay volume scaled to price graph (simple scaling)
            if (showVolume && vols.size() == closes.size()) {
              // create scaled volume array to overlay (so it fits on chart)
              double maxPrice = *std::max_element(closes.begin(), closes.end());
              double maxVol = *std::max_element(vols.begin(), vols.end());
              std::vector<double> volScaled(vols.size());
              for (size_t i=0;i<vols.size();++i)
                volScaled[i] = (vols[i] / maxVol) * (0.25 * maxPrice); // scale factor
              ImPlot::PlotLine("Volume (scaled)", x.data(), volScaled.data(), (int)volScaled.size());
            }
            
            ImPlot::EndPlot();
          }
        }
        
        if (ImGui::Button("Close", ImVec2(120,0))) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
      }
      
    } // end window
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

    // KanVest Name -------------------------------------------------------------------------
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
