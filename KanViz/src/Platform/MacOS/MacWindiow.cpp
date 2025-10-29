//
//  MacWindiow.cpp
//  KanViz
//
//  Created by Ashish . on 29/10/25.
//

#include "MacWindiow.hpp"

#include "Core/Event/ApplicationEvents.h"
#include "Core/Event/MouseEvents.h"
#include "Core/Event/KeyboardEvents.h"

namespace KanViz
{
  namespace MacWindowUtils
  {
    /// This function returns the Mac window data
    /// - Parameter window: GLFW window pointer
    static MacWindowData& GetWindowData(GLFWwindow* window)
    {
      return *static_cast<MacWindowData*>(glfwGetWindowUserPointer(window));
    }
  } // namespace MacWindowUtils
  
  MacWindow::MacWindow(const WindowSpecification& specification)
  : m_data({specification, nullptr})
  {
    IK_PROFILE();
    IK_LOG_INFO(LogModule::Window, "Creating core '{0}' window instance", m_data.specification.title);
    
    // Initialize GLFW Library
    if (!glfwInit())
    {
      IK_ASSERT(false, "Can not initialise the GLFW library !");
    }
    
    // Common hints
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, m_data.specification.samples);
    
    if (m_data.specification.rendererType == RendererType::OpenGL)
    {
      glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    }
    else
    {
      IK_ASSERT(false, "Invalid Renderer Type");
    }
    
    // Params to create the GLFW window
    int32_t width = static_cast<int32_t>(m_data.specification.width);
    int32_t height = static_cast<int32_t>(m_data.specification.height);
    
    IK_ASSERT(width > 0 or height > 0, "Window size can not be zero !");
    
    GLFWmonitor* primaryMonitor = nullptr;
    GLFWwindow* sharedContext = nullptr;
    
    // Overwrite the data if full screen is enabled.
    if (m_data.specification.isFullScreen)
    {
      primaryMonitor = glfwGetPrimaryMonitor();
      const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
      width = mode->width;
      height = mode->height;
      IK_ASSERT(0 != width or 0 != height, "Null width and height of window !");
    }
    
    // Create the GLFW Window.
    m_window = glfwCreateWindow(width, height, m_data.specification.title.data(), primaryMonitor, sharedContext);
    IK_ASSERT(m_window != nullptr, "Unable to create the window!");
    
    if (m_data.specification.rendererType == RendererType::OpenGL)
    {
      glfwSwapInterval(1); // Enable vsync
    }
    
    glfwSetWindowUserPointer(m_window, &m_data); // Set the user defined pointer to GLFW Window, this pointer will be retrieved when an interrupt will be triggered.
    SetEventCallbacks(); // Set GLFW callbacks.
    SetAtCenter(); // Default window at center
    
    // Control Window
    SetResizable(m_data.specification.resizable);
    if (m_data.specification.maximized)
    {
      Maximize();
    }
  }
  
  MacWindow::~MacWindow()
  {
    IK_PROFILE();
    IK_LOG_WARN(LogModule::Window, "Destroying core '{0}' window instance", m_data.specification.title);
  }
  
  void MacWindow::SetEventCallbacks()
  {
    // Call back for Window Resize Event -----------------------------------------------------------------------------
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
      MacWindowData& data = MacWindowUtils::GetWindowData(window);
      data.specification.width = static_cast<uint32_t>(width);
      data.specification.height = static_cast<uint32_t>(height);
      WindowResizeEvent event(data.specification.width, data.specification.height);
      data.eventCallbackFunction(event);
    });
    
    // Call back for Window Close Event ------------------------------------------------------------------------------
    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
      MacWindowData& data = MacWindowUtils::GetWindowData(window);
      WindowCloseEvent event;
      data.eventCallbackFunction(event);
    });
    
    // Call back for Window Focused Event ----------------------------------------------------------------------------
    glfwSetWindowFocusCallback( m_window,
[](
   [[maybe_unused]] GLFWwindow* window,
   [[maybe_unused]] int32_t icontified) {
     MacWindowData& data = MacWindowUtils::GetWindowData(window);
     WindowResizeEvent event(data.specification.width, data.specification.height);
     data.eventCallbackFunction(event);
   });
    // Call back for Window Minimize Event ----------------------------------------------------------------------------
    glfwSetWindowIconifyCallback(m_window, [](GLFWwindow* window, int iconified) {
      MacWindowData& data = MacWindowUtils::GetWindowData(window);
      WindowMinimizeEvent event((bool)iconified);
      data.eventCallbackFunction(event);
    });
    
    // Call back for Window Maximized Event ----------------------------------------------------------------------------
    glfwSetWindowMaximizeCallback(m_window, [](GLFWwindow* window, int maximized) {
      MacWindowData& data = MacWindowUtils::GetWindowData(window);
      WindowMaximizeEvent event((bool)maximized);
      data.eventCallbackFunction(event);
    });
    
    // Call back for Set the Key Event -------------------------------------------------------------------------------
    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scanCode, int action, int mods) {
      MacWindowData& data = MacWindowUtils::GetWindowData(window);
      switch (action)
      {
        case GLFW_PRESS:
        {
          KeyPressedEvent event(static_cast<Key>(key), 0);
          data.eventCallbackFunction(event);
          break;
        }
        case GLFW_RELEASE:
        {
          KeyReleasedEvent event(static_cast<Key>(key));
          data.eventCallbackFunction(event);
          break;
        }
        case GLFW_REPEAT:
        {
          KeyPressedEvent event(static_cast<Key>(key), 1);
          data.eventCallbackFunction(event);
          break;
        }
      }
    });
    
    // Call back for Character pressed Event -------------------------------------------------------------------------
    glfwSetCharCallback(m_window, [](GLFWwindow* window, uint32_t keyCode) {
      MacWindowData& data = MacWindowUtils::GetWindowData(window);
      KeyTypedEvent event(static_cast<Key>(keyCode));
      data.eventCallbackFunction(event);
    });
    
    // Call back for Mouse button pressed Event ----------------------------------------------------------------------
    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
      MacWindowData& data = MacWindowUtils::GetWindowData(window);
      
      switch (action)
      {
        case GLFW_PRESS:
        {
          MouseButtonPressedEvent event(static_cast<MouseButton>(button));
          data.eventCallbackFunction(event);
          break;
        }
        case GLFW_RELEASE:
        {
          MouseButtonReleasedEvent event(static_cast<MouseButton>(button));
          data.eventCallbackFunction(event);
          break;
        }
      }
    });
    
    // Call back for Mouse Scrolled Event ----------------------------------------------------------------------------
    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset) {
      MacWindowData& data = MacWindowUtils::GetWindowData(window);
      MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
      data.eventCallbackFunction(event);
    });
    
    // Call back for Mouse Cursor Event ------------------------------------------------------------------------------
    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPosition, double yPosition) {
      MacWindowData& data = MacWindowUtils::GetWindowData(window);
      MouseMovedEvent event(static_cast<float>(xPosition), static_cast<float>(yPosition));
      data.eventCallbackFunction(event);
    });
  }
  
  void MacWindow::Update()
  {
    IK_PERFORMANCE_FUNC("MacWindow::Update");
    
    glfwPollEvents();
    
    // Calculate the timestep using GLFW time
    float currentFrameTime = static_cast<float>(glfwGetTime());
    m_timeStep = currentFrameTime - m_lastFrameTime;
    m_lastFrameTime = currentFrameTime;
  }
  
  void MacWindow::SetEventFunction(EventCallbackFn eventCallbackFunction) noexcept
  {
    m_data.eventCallbackFunction = eventCallbackFunction;
  }
  
  void MacWindow::Maximize() const noexcept
  {
    glfwMaximizeWindow(m_window);
  }
  
  void MacWindow::Restore() const noexcept
  {
    glfwRestoreWindow(m_window);
  }
  
  void MacWindow::Iconify() const noexcept
  {
    glfwIconifyWindow(m_window);
  }
  
  void MacWindow::SetAtCenter() const noexcept
  {
    const GLFWvidmode* videMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    const WindowSpecification& spec = m_data.specification;
    int32_t x = (videMode->width / 2) - (static_cast<int32_t>(spec.width) / 2);
    int32_t y = (videMode->height / 2) - (static_cast<int32_t>(spec.height) / 2);
    glfwSetWindowPos(m_window, x, y);
  }
  
  void MacWindow::SetPosition(const glm::vec2& pos) const noexcept
  {
    glfwSetWindowPos(m_window, static_cast<int32_t>(pos.x), static_cast<int32_t>(pos.y));
  }
  void MacWindow::SetResizable(bool resizable) const noexcept
  {
    glfwSetWindowAttrib(m_window, GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE );
  }
  
  void MacWindow::SetSize(const glm::vec2& size) noexcept
  {
    if (0 == size.x or 0 == size.y or (m_data.specification.width == size.x and m_data.specification.height == size.y))
    {
      return; // No change required
    }
    
    m_data.specification.width = static_cast<uint32_t>(size.x);
    m_data.specification.height = static_cast<uint32_t>(size.y);
    
    glfwSetWindowSize(m_window, static_cast<int32_t>(m_data.specification.width), static_cast<int32_t>(m_data.specification.height));
    SetAtCenter();
  }
  
  void MacWindow::SetTitle(std::string_view title) noexcept
  {
    m_data.specification.title = title;
    glfwSetWindowTitle(m_window, m_data.specification.title.data());
  }
  
  // Getters ---------------------------------------------------------------------------------------------------------------------------------------
  bool MacWindow::IsActive() const noexcept
  {
    return !glfwWindowShouldClose(m_window);
  }
  
  bool MacWindow::IsMaximized() const noexcept
  {
    return static_cast<bool>(glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED));
  }
  
  bool MacWindow::IsTitleBarHidden() const noexcept
  {
    return m_data.specification.hideTitleBar;
  }
  
  bool MacWindow::IsFullscreen() const noexcept
  {
    return m_data.specification.isFullScreen;
  }
  
  uint32_t MacWindow::GetWidth() const noexcept
  {
    return m_data.specification.width;
  }
  
  uint32_t MacWindow::GetHeight() const noexcept
  {
    return m_data.specification.height;
  }
  
  const std::string& MacWindow::GetTitle() const noexcept
  {
    return m_data.specification.title;
  }
  
  const WindowSpecification& MacWindow::GetSpecification() const noexcept
  {
    return m_data.specification;
  }
  
  TimeStep MacWindow::GetTimestep() const noexcept
  {
    return m_timeStep;
  }
  
  void* MacWindow::GetNativeWindow() const noexcept
  {
    return static_cast<void*>(m_window);
  }
} // namespace KanViz
