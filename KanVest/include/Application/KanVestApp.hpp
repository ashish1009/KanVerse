//
//  KanVestApp.hpp
//  KanVest
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

namespace KanVest
{
  /// This is the client kreator application class
  class Application final : public KanViz::Application
  {
  public:
    /// This constructor creates the kreator application instance
    /// - Parameters:
    ///   - appSpec: core application specifications
    explicit Application(const KanViz::ApplicationSpecification& appSpec);
    /// This constructor destroys the Kreator application instance
    ~Application() override;
    
    // Overriden APIs ------------------------------------------------------------------------------------------------------------------------------
    /// This function calls before game loop
    void OnInit() override final;
    /// This function calls after game loop
    void OnShutdown() override final;
    /// This function calls inside Game Loop Running
    /// - Parameter ts: Time step for each frame
    void OnUpdate(const KanViz::TimeStep& ts) override final;
    /// This function calls when an event triggers
    /// - Parameter event: Triggered Events
    void OnEvent(KanViz::Event& event) override final;
    /// This function calls inside Game Loop under ImGui Layer
    void OnImGuiRender() override final;
    
  private:
    KanViz::Ref<KanViz::Layer> m_layer;
  };
} // namespace KanVest
