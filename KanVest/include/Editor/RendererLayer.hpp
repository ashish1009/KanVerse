//
//  RendererLayer.hpp
//  KanVest
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

namespace KanVest
{
  /// This class is the Renderer layer for Kreator application
  class RendererLayer : public KanViz::Layer
  {
  public:
    // Constructors and destructors ----------------------------------------------------------------------------------------------------------------
    /// This is Renderer Layer's Default Constructor
    explicit RendererLayer();
    /// Destructor for the renderer layer
    ~RendererLayer() override;
    
    // Overriden APIs ------------------------------------------------------------------------------------------------------------------------------
    /// This function initializes the layer. Called when the layer is pushed onto the application stack (only once).
    void OnAttach() noexcept override;
    /// This function destroy the layer, calls when we pop layer from the application (To be called only once)
    void OnDetach() noexcept override;
    
    /// This function updates the layer data. To be called each frame from application.
    /// - Parameter ts: Time step between 2 Frames
    /// - Note Core Application is taking care to call the Update(ts) API for all Layers
    void OnUpdate(const KanViz::TimeStep& ts) override;
    /// This function renders the GUI Window for this layer. To be called each frame from application.
    /// - Note: Core Application is taking care to call the RenderGui API for all Layers
    void OnImGuiRender() override;
    /// This function handles all the events triggered for the layer. To be whenever an event is triggered in the window.
    /// - Parameter event: Event (Base class) intance. Dispatch event from Event Dispatcher
    /// - Note Core Application is taking care to call the EventHandler(event) API for all Layers
    void OnEvent(KanViz::Event& event) override;
    
    // Static APIs ---------------------------------------------------------------------------------------------------
    /// This function returns the single instance of the renderer layer
    static RendererLayer& Get();
    
  private:
    /// This function starts main ImGui Window with Docking
    void UI_StartMainWindowDocking();
    /// This function ends Main ImGui window with Docking
    void UI_EndMainWindowDocking();
    
    /// This function show login page
    void UI_LoginPage();
    /// This function shows stock analyzer panel
    void UI_StockAnalyzer();
    
    /// This function shows Performance pane;
    void UI_PerformancePanel();
    
    /// This function shows primary Viewport panel
    void UI_PrimaryViewportPanel_DEMO();

    /// This function renders our title bar
    float UI_DrawTitlebar();
    /// This function handle the drag and Click on title bar
    /// - Parameter titlebarHeight: Title bar height
    void UI_TitlebarDragArea(float titlebarHeight);
    /// This function renders window buttons
    void UI_WindowButtons();

    // Single Instance -----------------------------------
    KanViz::Ref<KanViz::Image> m_applicationIcon;                  //< Application icon
    KanViz::Ref<KanViz::Image> m_iconMinimize, m_iconMaximize;     //< Icons to maximize and minimize the iwndow
    KanViz::Ref<KanViz::Image> m_iconRestore, m_iconClose;         //< Icons to close and restore the window
    KanViz::Ref<KanViz::Image> m_shadowTexture;                    //< Side shadow
    KanViz::Ref<KanViz::Image> m_searchIcon, m_settingIcon;        //< WIdget Icons
    KanViz::Ref<KanViz::Image> m_reloadIcon;                       //< Reload Icon

    static RendererLayer* s_instance;                              //< Renderer Layer single instance
  };
} // namespace KanVest
