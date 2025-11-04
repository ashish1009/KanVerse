//
//  RendererLayer.cpp
//  KanVest
//
//  Created by Ashish . on 29/10/25.
//

#include "RendererLayer.hpp"

#include "Stocks/StockController.hpp"
#include "Stocks/StockAPI.hpp"
#include "Stocks/StockParser.hpp"
#include "Stocks/Stock_UI.hpp"

#include "Portfolio/LoginManager.hpp"
#include "Portfolio/UserManager.hpp"

namespace KanVest
{
  static const std::filesystem::path KanVestResourcePath = "../../../KanVest/Resources";
  
  // Kretor Resource Path
#define KanVestResourcePath(path) std::filesystem::absolute(KanVestResourcePath / path)
  
  // Kreate Texture
#define CreateTexture(path) KanViz::TextureFactory::Create(KanVestResourcePath(path))
    
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
    
    // Set the Imgui theme ----------------------------------------------------------------------
    KanVest::UI::Font::Load({
      {UI::FontType::Regular,                 {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         18}},
      {UI::FontType::Light,                   {KanVestResourcePath("Fonts/Opensans/Light.ttf"),           18}},
      {UI::FontType::Italic,                  {KanVestResourcePath("Fonts/Opensans/Italic.ttf"),          18}},
      {UI::FontType::LightItalic,             {KanVestResourcePath("Fonts/Opensans/LightItalic.ttf"),     18}},
      {UI::FontType::SemiBold,                {KanVestResourcePath("Fonts/Opensans/SemiBold.ttf"),        18}},
      {UI::FontType::Bold,                    {KanVestResourcePath("Fonts/Opensans/Bold.ttf"),            18}},
      {UI::FontType::SemiBoldItalic,          {KanVestResourcePath("Fonts/Opensans/SemiBoldItalic.ttf"),  18}},
      {UI::FontType::BoldItalic,              {KanVestResourcePath("Fonts/Opensans/BoldItalic.ttf"),      18}},
      {UI::FontType::ExtraBold,               {KanVestResourcePath("Fonts/Opensans/ExtraBold.ttf"),       18}},
      {UI::FontType::ExtraBoldItalic,         {KanVestResourcePath("Fonts/Opensans/ExtraBoldItalic.ttf"), 18}},
      
      {UI::FontType::FixedWidthRegular,       {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       10}},
      {UI::FontType::FixedWidthLight,         {KanVestResourcePath("Fonts/HfMonorita/Light.ttf"),         10}},
      {UI::FontType::FixedWidthMedium,        {KanVestResourcePath("Fonts/HfMonorita/Medium.ttf"),        10}},
      {UI::FontType::FixedWidthBold,          {KanVestResourcePath("Fonts/HfMonorita/Bold.ttf"),          10}},
      
      {UI::FontType::Small,                   {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         16}},
      {UI::FontType::Medium,                  {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         20}},
      {UI::FontType::Large,                   {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         22}},
      {UI::FontType::ExtraLarge,              {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         24}},
      {UI::FontType::SemiHeader,              {KanVestResourcePath("Fonts/Opensans/SemiBold.ttf"),        28}},
      {UI::FontType::Header,                  {KanVestResourcePath("Fonts/Opensans/Bold.ttf"),            32}},
      {UI::FontType::LargeHeader,             {KanVestResourcePath("Fonts/Opensans/ExtraBold.ttf"),       40}},
      {UI::FontType::HugeHeader,              {KanVestResourcePath("Fonts/Opensans/ExtraBold.ttf"),       50}},
      
      {UI::FontType::FixedWidthHeader_12,     {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       12}},
      {UI::FontType::FixedWidthHeader_14,     {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       14}},
      {UI::FontType::FixedWidthHeader_16,     {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       16}},
      {UI::FontType::FixedWidthHeader_18,     {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       18}},
      {UI::FontType::FixedWidthHeader_20,     {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       20}},
      {UI::FontType::FixedWidthHeader_22,     {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       22}},
      {UI::FontType::FixedWidthHeader_24,     {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       24}},
      {UI::FontType::FixedWidthHeader_26,     {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       26}},
      {UI::FontType::FixedWidthHeader_28,     {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       28}},
      {UI::FontType::FixedWidthHeader_30,     {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       30}},
      {UI::FontType::FixedWidthHeader_32,     {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       32}},
      {UI::FontType::FixedWidthHeader_34,     {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       34}},
      {UI::FontType::FixedWidthHeader_36,     {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       36}},
      {UI::FontType::FixedWidthHeader_38,     {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       38}},
      {UI::FontType::FixedWidthHeader_40,     {KanVestResourcePath("Fonts/HfMonorita/Regular.ttf"),       40}},

      {UI::FontType::Header_22,               {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         22}},
      {UI::FontType::Header_24,               {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         24}},
      {UI::FontType::Header_26,               {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         26}},
      {UI::FontType::Header_28,               {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         28}},
      {UI::FontType::Header_30,               {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         30}},
      {UI::FontType::Header_32,               {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         32}},
      {UI::FontType::Header_34,               {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         34}},
      {UI::FontType::Header_36,               {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         36}},
      {UI::FontType::Header_38,               {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         38}},
      {UI::FontType::Header_40,               {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         40}},
      {UI::FontType::Header_42,               {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         42}},
      {UI::FontType::Header_44,               {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         44}},
      {UI::FontType::Header_46,               {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         46}},
      {UI::FontType::Header_48,               {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         48}},
      {UI::FontType::Header_50,               {KanVestResourcePath("Fonts/Opensans/Regular.ttf"),         50}},
    });
    
    KanVasX::Color::Initialize();
    KanVasX::Panel::Initialize(KanVasX::UI::GetTextureID(m_shadowTexture->GetRendererID()));
    
    KanVasX::Widget::Initialize();
    KanVasX::Widget::SetSearchIcon(KanVasX::UI::GetTextureID(m_searchIcon->GetRendererID()));
    KanVasX::Widget::SetSettingIcon(KanVasX::UI::GetTextureID(m_settingIcon->GetRendererID()));
    
    StockUI::Initialize(KanVasX::UI::GetTextureID(m_reloadIcon->GetRendererID()));
    
    // Login popup
    m_loginPopup.Set("KanVest Logic", true /* open flag */, 600, 410, true /* center */);
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
    UI_LoginPage();
    UI_SignupPage();
    
    if (UserManager::GetCurrentUser().Valid())
    {
      UI_StartMainWindowDocking();
      UI_StockAnalyzer();
      UI_EndMainWindowDocking();
    }
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
  
  void RendererLayer::UI_LoginPage()
  {
    m_loginPopup.Show(ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar, [this]()
                        {
      KanVasX::ScopedColor bgColor(ImGuiCol_ChildBg, KanVasX::Color::Background);

      // Kreator Logo and title ----
      {
        // Icon
        static constexpr ImVec2 logoSize {200, 200};
        KanVasX::UI::SetCursorAt(KanVasX::UI::AlignX::Center, logoSize.x);
        KanVasX::UI::Image(KanVasX::UI::GetTextureID(m_welcomeIcon->GetRendererID()), logoSize, KanVasX::Color::TextBright);
        
        // Welcome Header
        KanVasX::UI::Text(UI::Font::Get(UI::FontType::HugeHeader), "Welcome to KanVest", KanVasX::UI::AlignX::Center, {0, 0}, KanVasX::Color::Text);
        KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, 600);
        
        static KanVasX::InputBuffer<256> usernameBuffer{"##UserName"};
        static KanVasX::InputBuffer<256> passwordBuffer{"##Password"};
        
        // Username
        KanVasX::UI::Text(UI::Font::Get(UI::FontType::FixedWidthHeader_14), "Username", KanVasX::UI::AlignX::Center, {-50.0f, 20.0f});
        ImGui::SameLine();
        KanVasX::UI::ShiftCursorY(-8.0f);
        usernameBuffer.StrCpy("ashish");
        usernameBuffer.TextInput(false, 100, " ");
        KanVasX::UI::Text(UI::Font::Get(UI::FontType::FixedWidthHeader_14), "Password", KanVasX::UI::AlignX::Center, {-50.0f, 15.0f});

        // Passeord
        static bool showPassword = false;
        const auto& eyeTexture = showPassword ? m_openEyeIcon : m_closeEyeIcon;

        ImGui::SameLine();
        KanVasX::UI::ShiftCursorY(-8.0f);
        passwordBuffer.StrCpy("ashish");
        passwordBuffer.TextInput(false, 100, " ", showPassword ? 0 : ImGuiInputTextFlags_Password);
        
        ImGui::SameLine();
        if (KanVasX::UI::DrawButtonImage("##ShowPassword", KanVasX::UI::GetTextureID(eyeTexture->GetRendererID()), false, {20.0f, 20.0f}))
        {
          showPassword ^= 1;
        }
        
        // Login
        KanVasX::UI::ShiftCursorX(ImGui::GetContentRegionAvail().x * 0.5f - 105.0f);

        static std::string loginMessage;
        static bool loginSuccess = false;
        if (KanVasX::UI::DrawButton("Login", UI::Font::Get(UI::FontType::Bold)) or ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Enter))
        {
          loginSuccess = LoginManager::HandleLogin(usernameBuffer, passwordBuffer, loginMessage);
          ImGui::CloseCurrentPopup();
        }
        
        // Sign Up
        ImGui::SameLine();
        if (KanVasX::UI::DrawButton("Sign Up", UI::Font::Get(UI::FontType::Bold)))
        {
          ImGui::CloseCurrentPopup();
          m_signUpPopup.Set("KanVest Signup", true /* open flag */, 600, 450, true /* center */);
        }

        // Forget pasword
        ImGui::SameLine();
        if (KanVasX::UI::DrawButton("Forget password", UI::Font::Get(UI::FontType::Regular)))
        {

        }

        KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, 600);
        if (!loginSuccess)
        {
          KanVasX::UI::Text(UI::Font::Get(UI::FontType::Regular), loginMessage.c_str(), KanVasX::UI::AlignX::Center, {0, 5.0}, KanVasX::Color::Red);
        }
      }
      
      KanVasX::UI::DrawShadowAllDirection(KanVasX::UI::GetTextureID(m_shadowTexture->GetRendererID()));
    });
  }
  
  void RendererLayer::UI_SignupPage()
  {
    m_signUpPopup.Show(ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar, [this]()
                      {
      KanVasX::ScopedColor bgColor(ImGuiCol_ChildBg, KanVasX::Color::Background);
      
      // Kreator Logo and title ----
      {
        // Icon
        static constexpr ImVec2 logoSize {200, 200};
        KanVasX::UI::SetCursorAt(KanVasX::UI::AlignX::Center, logoSize.x);
        KanVasX::UI::Image(KanVasX::UI::GetTextureID(m_welcomeIcon->GetRendererID()), logoSize, KanVasX::Color::TextBright);
        
        // Welcome Header
        KanVasX::UI::Text(UI::Font::Get(UI::FontType::HugeHeader), "Welcome to KanVest", KanVasX::UI::AlignX::Center, {0, 0}, KanVasX::Color::Text);
        KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, 600);
        
        static KanVasX::InputBuffer<256> usernameBuffer{"##UserName"};
        static KanVasX::InputBuffer<256> passwordBuffer{"##Password"};
        static KanVasX::InputBuffer<256> confirmPasswordBuffer{"##ConfirmPassword"};

        // Username
        KanVasX::UI::Text(UI::Font::Get(UI::FontType::FixedWidthHeader_14), "Username", KanVasX::UI::AlignX::Center, {-50.0f, 20.0f});
        ImGui::SameLine();
        KanVasX::UI::ShiftCursorY(-8.0f);
        usernameBuffer.TextInput(false, 100, " ");
        
        // Passeord
        static bool showPassword = false;
        const auto& eyeTexture = showPassword ? m_openEyeIcon : m_closeEyeIcon;
        
        KanVasX::UI::Text(UI::Font::Get(UI::FontType::FixedWidthHeader_14), "Password", KanVasX::UI::AlignX::Center, {-50.0f, 15.0f});
        ImGui::SameLine();
        KanVasX::UI::ShiftCursorY(-8.0f);
        passwordBuffer.TextInput(false, 100, " ", showPassword ? 0 : ImGuiInputTextFlags_Password);

        KanVasX::UI::Text(UI::Font::Get(UI::FontType::FixedWidthHeader_14), "Confirm Password", KanVasX::UI::AlignX::Center, {-82.0f, 15.0f});
        ImGui::SameLine();
        KanVasX::UI::ShiftCursorY(-8.0f);
        confirmPasswordBuffer.TextInput(false, 100, " ", showPassword ? 0 : ImGuiInputTextFlags_Password);

        ImGui::SameLine();
        if (KanVasX::UI::DrawButtonImage("##ShowPassword", KanVasX::UI::GetTextureID(eyeTexture->GetRendererID()), false, {20.0f, 20.0f}))
        {
          showPassword ^= 1;
        }
        
        // Sign Up
        KanVasX::UI::ShiftCursorX(ImGui::GetContentRegionAvail().x * 0.5f - 50.0f);
        
        static std::string signUpMessage;
        static bool signUpSuccess = false;
        if (KanVasX::UI::DrawButton("Sign Up", UI::Font::Get(UI::FontType::Bold)) or ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Enter))
        {
          if (passwordBuffer != confirmPasswordBuffer)
          {
            signUpSuccess = false;
            signUpMessage = "Password do not match";
          }
          else
          {
            signUpSuccess = LoginManager::HandleSignUp(usernameBuffer, passwordBuffer, signUpMessage);
          }
        }

        // Cancel
        ImGui::SameLine();
        if (KanVasX::UI::DrawButton("Cancel", UI::Font::Get(UI::FontType::Bold)) or ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Escape))
        {
          ImGui::CloseCurrentPopup();
          m_loginPopup.Set("KanVest Login", true /* open flag */, 600, 410, true /* center */);
        }

        KanVasX::UI::DrawFilledRect(KanVasX::Color::Separator, 1, 600);
        if (signUpSuccess)
        {
          ImGui::CloseCurrentPopup();
          m_loginPopup.Set("KanVest Login", true /* open flag */, 600, 410, true /* center */);
        }
        else
        {
          KanVasX::UI::Text(UI::Font::Get(UI::FontType::Regular), signUpMessage.c_str(), KanVasX::UI::AlignX::Center, {0, 5.0}, KanVasX::Color::Red);
        }
      }
      
      KanVasX::UI::DrawShadowAllDirection(KanVasX::UI::GetTextureID(m_shadowTexture->GetRendererID()));
    });
  }
  
  void RendererLayer::UI_StockAnalyzer()
  {
    IK_PERFORMANCE_FUNC("RendererLayer::UI_StockAnalyzer");
    StockUI::StockPanel();
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
      
      std::string liveData = StockAPI::FetchLiveData(symbol, "1m", "1d");
      
      // If liveData doesn't contain usual fields, try .BO fallback for Indian stocks
      if (liveData.find("\"regularMarketPrice\"") == std::string::npos && autoFallbackBO && symbol.find(".NS") != std::string::npos) {
        std::string altSymbol = symbol.substr(0, symbol.find(".NS")) + ".BO";
        std::string altData = StockAPI::FetchLiveData(altSymbol, "1m", "1d");
        if (altData.find("\"regularMarketPrice\"") != std::string::npos) {
          symbol = altSymbol;
          std::snprintf(symbolBuf, sizeof(symbolBuf), "%s", symbol.c_str());
          liveData = altData;
        }
      }
      
      // --- Meta fields (many common ones) ---
      price = StockParser::ExtractValue(liveData, "regularMarketPrice");
      openPrice = StockParser::ExtractValue(liveData, "regularMarketOpen");
      dayHigh = StockParser::ExtractValue(liveData, "regularMarketDayHigh");
      dayLow = StockParser::ExtractValue(liveData, "regularMarketDayLow");
      prevClose = StockParser::ExtractValue(liveData, "chartPreviousClose");
      
      change = price - prevClose;
      // some endpoints include percent field, try reading directly:
      changePercent = StockParser::ExtractValue(liveData, "regularMarketChangePercent");
      if (changePercent == -1 && prevClose > 0) changePercent = (change / prevClose) * 100.0;
      
      fiftyTwoHigh = StockParser::ExtractValue(liveData, "fiftyTwoWeekHigh");
      fiftyTwoLow = StockParser::ExtractValue(liveData, "fiftyTwoWeekLow");
      std::cout << fiftyTwoLow;
      regularMarketVolume = StockParser::ExtractValue(liveData, "regularMarketVolume");
      avgVolume3m = StockParser::ExtractValue(liveData, "averageDailyVolume3Month");
      avgVolume10day = StockParser::ExtractValue(liveData, "averageDailyVolume10Day");
      
      marketCap = StockParser::ExtractValue(liveData, "marketCap");
      trailingPE = StockParser::ExtractValue(liveData, "trailingPE");
      forwardPE = StockParser::ExtractValue(liveData, "forwardPE");
      
      dividendRate = StockParser::ExtractValue(liveData, "dividendRate");
      dividendYield = StockParser::ExtractValue(liveData, "dividendYield");
      
      currency = StockParser::ExtractString(liveData, "currency");
      exchangeName = StockParser::ExtractString(liveData, "exchangeName");
      fullExchangeName = StockParser::ExtractString(liveData, "fullExchangeName");
      shortName = StockParser::ExtractString(liveData, "shortName");
      longName = StockParser::ExtractString(liveData, "longName");
      marketState = StockParser::ExtractString(liveData, "marketState");
      
      // --- Historical range (user controlled) ---
      time_t s = StockParser::ParseDateYYYYMMDD(startDate);
      time_t e = StockParser::ParseDateYYYYMMDD(endDate);
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
      std::string histData = StockAPI::FetchLiveData(symbol, "1m", "1d");

//      std::string histData = StockAPI::FetchURL(histURL);
      
      // parse arrays
      closes = StockParser::ExtractArray(histData, "close");
      opens = StockParser::ExtractArray(histData, "open");
      highs = StockParser::ExtractArray(histData, "high");
      lows = StockParser::ExtractArray(histData, "low");
      vols = StockParser::ExtractArray(histData, "volume"); // returns as double but volumes are ints
      
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
        
        ImGui::SameLine(300);
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
    KanVasX::UI::Text(KanVest::UI::Font::Get(KanVest::UI::FontType::Header), "KanVest", KanVasX::UI::AlignX::Center, {0.0f, 10.0f}, KanVasX::Color::TextMuted);
    
    // Title Rectangles --------------------------------------------------------------------
    KanVasX::UI::SetCursorPos({ImGui::GetWindowWidth() / 4, windowPadding.y});
    KanVasX::UI::DrawRect(KanVasX::Color::Alpha(KanVasX::Color::HighlightMuted, 0.4f), titleBarHeight / 1.6, 0.5f, {0.0f, titleBarHeight / 4});
    
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
