//
//  ImGuiLayer.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Core/Layer.hpp"
#include "Core/Window.hpp"

namespace KanViz::UI
{
  /// This structure stores the ImGui Font data
  struct ImGuiFont
  {
    std::filesystem::path filePath {};
    uint32_t size {0};
  };
  
  /// This class represents the ImGui layer for rendering the GUI within the engine.
  class ImGuiLayer : public Layer
  {
  public:
    /// This constructor creates the ImGui layer instance.
    /// - Parameter windowPointer: application window pointer
    ImGuiLayer(const Window* windowPointer);
    /// This destructor destroys the ImGui layer instance
    ~ImGuiLayer();
    
    // Fundamentals --------------------------------------------------------------------------------------------------------------------------------
    /// This function begins the ImGui rendering for each frame NOTE: Call once per frame
    void Begin();
    /// This function ends the rendering of ImGui each frame NOTE: Call once per frame
    void End();
    /// This function saves the location of ini file path
    /// - Parameter iniFilepath: file path relative to executable
    /// - Note: Directory must exist.
    void SetIniFilePath(const std::filesystem::path& iniFilepath);
    /// This function overrides font of ImGui with new font path
    /// - Parameter fonts: fonts to be loaded in GUI
    void LoadFonts(const std::vector<UI::ImGuiFont>& fonts);
    
    // Virtual overridden --------------------------------------------------------------------------------------------------------------------------
    /// This function initialises the ImGui layer. Creates the ImGui layer using ImGui file library
    void OnAttach() override;
    /// This function destroys the ImGui layer and deletes all ImGui resources.
    void OnDetach() override;
    /// This function does nothing for now.
    void OnImGuiRender() override {}
    /// This function does nothing for now.
    void OnUpdate(const TimeStep&) override {}
    /// This function handles all the events triggered for the ImGui layer. To be called whenever an event is triggered
    /// in the window
    /// - Note: Core ikan::Application is taking care to call the HandleEvents(event) API for all Layers
    /// - Parameter event: Event (Base class) intance. Dispatch event from Event Dispatcher
    void OnEvent(Event& event) override;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(ImGuiLayer);
    
  private:
    bool m_blockEvents {true};
    const Window* const m_window;
    std::filesystem::path m_initFilePath{};
  };
} // namespace KanViz::UI
