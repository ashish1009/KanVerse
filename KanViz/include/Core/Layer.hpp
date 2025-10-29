//
//  Layer.hpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#pragma once

#include "Core/Event/Events.h"
#include "Core/TimeStep.hpp"

namespace KanViz
{
  /// This is the base class to store the layers in application
  class Layer
  {
  public:
    Layer(const std::string& name)
    : m_name(name)
    {
      
    }
    
    virtual ~Layer() = default;
    
    // Virtual APIs --------------------------------------------------------------------------------------------------------------------------------
    /// This funciton initialises the layer, calls when we push layer in the application (To be called only once)
    virtual void OnAttach() = 0;
    /// This funciton destroy the layer, calls when we pop layer from the application (To be called only once)
    virtual void OnDetach() = 0;
    
    /// This function renders the GUI Window for this layer. To be called each frame from application.
    /// - Important: Core KanViz::Application is taking care to call the RenderGui API for all Layers
    virtual void OnImGuiRender() = 0;
    /// This function updates the layer data. To be called each frame from application.
    /// - Parameter ts: Time step between 2 Frames
    /// - Important: Core KanViz::Application is taking care to call the Update(ts) API for all Layers
    virtual void OnUpdate(const TimeStep& ts) = 0;
    /// This function handles all the events triggered for the layer. To be whenever an event is triggered in the
    /// window
    /// - Parameter event: Event (Base class) intance. Dispatch event from Dispatcher
    /// - Important: Core KanViz::Application is taking care to call the EventHandler(event) API for all Layers
    virtual void OnEvent(Event& event) = 0;
    
    // Getter --------------------------------------------------------------------------------------------------------------------------------------
    /// This function returns the name of layer
    const std::string& GetName() const
    {
      return m_name;
    }
    
    DELETE_COPY_MOVE_CONSTRUCTORS(Layer)
    
  private:
    std::string m_name {"Default"};
  };
} // namespace KanViz
