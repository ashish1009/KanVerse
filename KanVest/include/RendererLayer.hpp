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
    // Single Instance -----------------------------------
    static RendererLayer* s_instance;                          //< Renderer Layer single instance
  };
} // namespace KanVest
