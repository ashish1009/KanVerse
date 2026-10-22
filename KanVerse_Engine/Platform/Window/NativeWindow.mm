/*!
 * @file NativeWindow.mm
 * @brief AppKit native window implementation bridging Cocoa, mouse input, and KanVerse events.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

// Include the C++ interface headers that this Objective-C++ file implements[cite: 17]
#include "Platform/Window/NativeWindow.hpp"
#include "Platform/Events/WindowEvents.hpp"
#include "Platform/Events/MouseEvents.hpp"
#include "Platform/Events/KeyEvents.hpp"
#include "Platform/Input/GameControllerBridge.hpp"
#include "Core/Application/Application.hpp"
#include "Core/Diagnostics/Log.hpp"

// Import Apple's native Objective-C frameworks[cite: 17]
#import <AppKit/AppKit.h>         // macOS desktop UI, windowing, and event system[cite: 17]
#import <Metal/Metal.h>           // Apple's low-level GPU API[cite: 17]
#import <MetalKit/MetalKit.h>     // Metal utilities, specifically MTKView for rendering[cite: 17]
#import <pthread.h>               // POSIX threads for CPU core affinity/scheduling[cite: 17]

// ==============================================================================
// Custom MTKView Subclass (Routes mouse tracking, scroll, and key events)
// ==============================================================================

// Declare an Objective-C class that inherits from MTKView (Metal Kit View)[cite: 17]
// This view serves as the physical canvas where Metal draws its pixels[cite: 17]
@interface KanVerseMetalView : MTKView
- (instancetype)initWithFrame:(NSRect)frame
                       device:(id<MTLDevice>)device
                   windowData:(KanVerse::NativeWindow::WindowData*)data;
@end

@implementation KanVerseMetalView {
  // Store a raw pointer back to the C++ WindowData struct so we can fire C++ callbacks[cite: 17]
  KanVerse::NativeWindow::WindowData* _dataPtr;
  // Area defining where the OS should track mouse movement over our view[cite: 17]
  NSTrackingArea*                     _trackingArea;
}

// Custom initializer linking the C++ data to the Objective-C view[cite: 17]
- (instancetype)initWithFrame:(NSRect)frame
                       device:(id<MTLDevice>)device
                   windowData:(KanVerse::NativeWindow::WindowData*)data {
  self = [super initWithFrame:frame device:device];
  if (self) {
    _dataPtr = data;
    [self updateTrackingAreas]; // Initialize mouse tracking boundaries[cite: 17]
  }
  return self;
}

// Tells macOS this view can receive keyboard and mouse focus[cite: 17]
- (BOOL)acceptsFirstResponder {
  return YES;
}

// Tells macOS this view can be the primary active UI element[cite: 17]
- (BOOL)canBecomeKeyView {
  return YES;
}

// Sets up or refreshes the mouse tracking boundary[cite: 17]
- (void)updateTrackingAreas {
  [super updateTrackingAreas];
  
  // Clean up the old tracking area if it exists (e.g., when window resizes)[cite: 17]
  if (_trackingArea) {
    [self removeTrackingArea:_trackingArea];
  }
  
  // Track mouse moves even when not clicking, and only when inside the visible view[cite: 17]
  NSTrackingAreaOptions options = NSTrackingMouseMoved |
  NSTrackingActiveAlways |
  NSTrackingInVisibleRect;
  
  _trackingArea = [[NSTrackingArea alloc] initWithRect:self.bounds
                                               options:options
                                                 owner:self
                                              userInfo:nil];
  [self addTrackingArea:_trackingArea];
}

// Catches physical key presses and translates Apple's keycode into the C++ InputBridge[cite: 17]
- (void)keyDown:(NSEvent *)event {
  KanVerse::InputBridge::SetKeyState(event.keyCode, true);
}

// Catches physical key releases and translates Apple's keycode into the C++ InputBridge[cite: 17]
- (void)keyUp:(NSEvent *)event {
  KanVerse::InputBridge::SetKeyState(event.keyCode, false);
}

// Catches modifier keys (Shift, Ctrl, Option, Command) changing state[cite: 17]
- (void)flagsChanged:(NSEvent *)event {
  const bool isShift = (event.modifierFlags & NSEventModifierFlagShift) != 0;
  const bool isCtrl  = (event.modifierFlags & NSEventModifierFlagControl) != 0;
  KanVerse::InputBridge::SetKeyState(56, isShift); // 56 is the virtual keycode for Shift[cite: 17]
  KanVerse::InputBridge::SetKeyState(59, isCtrl);  // 59 is the virtual keycode for Control[cite: 17]
}

// Triggered when a mouse button is pressed[cite: 17]
- (void)mouseDown:(NSEvent *)event {
  if (_dataPtr && _dataPtr->EventCallback) {
    KanVerse::MouseButtonPressedEvent e(static_cast<int>(event.buttonNumber));
    _dataPtr->EventCallback(e);
  }
}

// Triggered when a mouse button is released[cite: 17]
- (void)mouseUp:(NSEvent *)event {
  if (_dataPtr && _dataPtr->EventCallback) {
    KanVerse::MouseButtonReleasedEvent e(static_cast<int>(event.buttonNumber));
    _dataPtr->EventCallback(e);
  }
}

// Triggered specifically on right-click down[cite: 17]
- (void)rightMouseDown:(NSEvent *)event {
  if (_dataPtr && _dataPtr->EventCallback) {
    KanVerse::MouseButtonPressedEvent e(1); // 1 = standard code for right-click[cite: 17]
    _dataPtr->EventCallback(e);
  }
}

// Triggered specifically on right-click release[cite: 17]
- (void)rightMouseUp:(NSEvent *)event {
  if (_dataPtr && _dataPtr->EventCallback) {
    KanVerse::MouseButtonReleasedEvent e(1);
    _dataPtr->EventCallback(e);
  }
}

// Triggered when the mouse moves without any buttons pressed[cite: 17]
- (void)mouseMoved:(NSEvent *)event {
  if (_dataPtr && _dataPtr->EventCallback) {
    // Convert OS screen coordinates into coordinates relative to this specific view[cite: 17]
    const NSPoint loc = [self convertPoint:event.locationInWindow fromView:nil];
    KanVerse::MouseMovedEvent e(static_cast<float>(loc.x), static_cast<float>(loc.y));
    _dataPtr->EventCallback(e);
  }
}

// Triggered when the mouse moves while a button is held down (dragging)[cite: 17]
- (void)mouseDragged:(NSEvent *)event {
  if (_dataPtr && _dataPtr->EventCallback) {
    const NSPoint loc = [self convertPoint:event.locationInWindow fromView:nil];
    KanVerse::MouseMovedEvent e(static_cast<float>(loc.x), static_cast<float>(loc.y));
    _dataPtr->EventCallback(e);
  }
}

// Triggered by physical mouse scroll wheels or trackpad scrolling[cite: 17]
- (void)scrollWheel:(NSEvent *)event {
  if (_dataPtr && _dataPtr->EventCallback) {
    KanVerse::MouseScrolledEvent e(static_cast<float>(event.scrollingDeltaX),
                                   static_cast<float>(event.scrollingDeltaY));
    _dataPtr->EventCallback(e);
  }
}
@end

// ==============================================================================
// Internal Cocoa Window & App Delegate
// ==============================================================================

// Declare an Objective-C class handling OS window and app lifecycle events[cite: 17]
@interface KanVerseWindowDelegate : NSObject <NSWindowDelegate, NSApplicationDelegate>
- (instancetype)initWithWindowData:(KanVerse::NativeWindow::WindowData*)data;
- (void)onQuitRequested:(id)sender;
@end

@implementation KanVerseWindowDelegate {
  KanVerse::NativeWindow::WindowData* _dataPtr;
}

- (instancetype)initWithWindowData:(KanVerse::NativeWindow::WindowData*)data {
  self = [super init];
  if (self) {
    _dataPtr = data;
  }
  return self;
}

// Custom handler triggered if the user clicks "Quit" from the Mac menu bar[cite: 17]
- (void)onQuitRequested:(id)sender {
  (void)sender;
  KV_CORE_INFO("Quit triggered via Cmd+Q: initiating clean engine shutdown.");
  if (_dataPtr && _dataPtr->EventCallback) {
    KanVerse::WindowCloseEvent event;
    _dataPtr->EventCallback(event);
  }
}

// Hook called right before macOS attempts to kill the application process[cite: 17]
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender {
  (void)sender;
  if (_dataPtr && _dataPtr->EventCallback) {
    KanVerse::WindowCloseEvent event;
    _dataPtr->EventCallback(event); // Dispatch event to C++ to break the game loop organically[cite: 17]
  }
  // Return cancel to stop macOS from abruptly killing the process (exit(0)), ensuring C++ destructors run[cite: 17]
  return NSTerminateCancel;
}

// Triggered when the user physically resizes the window[cite: 17]
- (void)windowDidResize:(NSNotification *)notification {
  if (!_dataPtr || !_dataPtr->EventCallback) return;
  
  NSWindow* window = [notification object];
  if (!window) return;
  
  // Get logical UI bounds, then convert to physical Retina pixels[cite: 17]
  NSRect bounds = [window.contentView bounds];
  NSRect pixelBounds = [window.contentView convertRectToBacking:bounds];
  
  uint32_t width  = static_cast<uint32_t>(pixelBounds.size.width);
  uint32_t height = static_cast<uint32_t>(pixelBounds.size.height);
  
  // Only dispatch if the window hasn't collapsed to 0[cite: 17]
  if (width > 0 && height > 0) {
    _dataPtr->Width  = width;
    _dataPtr->Height = height;
    
    KanVerse::WindowResizeEvent event(width, height);
    _dataPtr->EventCallback(event);
  }
}

// Triggered when the window's red close button is clicked[cite: 17]
- (void)windowWillClose:(NSNotification *)notification {
  (void)notification;
  if (_dataPtr && _dataPtr->EventCallback) {
    KanVerse::WindowCloseEvent event;
    _dataPtr->EventCallback(event);
  }
}
@end

namespace KanVerse {
  
  // Standard polymorphic factory to create a platform-specific window instance[cite: 17]
  std::unique_ptr<Window> Window::Create(const WindowProps& props) {
    return std::make_unique<NativeWindow>(props);
  }
  
  // C++ Constructor delegates to the initialization function[cite: 17]
  NativeWindow::NativeWindow(const WindowProps& props) {
    Init(props);
  }
  
  // C++ Destructor delegates to the cleanup function[cite: 17]
  NativeWindow::~NativeWindow() {
    Shutdown();
  }
  
  void NativeWindow::Init(const WindowProps& props) {
    // Copy the creation properties to the internal C++ struct[cite: 17]
    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;
    m_Data.VSync = props.VSync;
    m_Data.Maximized = props.Maximized;
    
    // Bootstraps a Cocoa application programmatically without an Xcode Storyboard[cite: 17]
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular]; // Gives the app a dock icon and menu[cite: 17]
    
    // Force the main application thread to run on Apple Silicon Performance Cores (P-Cores)[cite: 17]
    pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
    
    KanVerseWindowDelegate* delegate = [[KanVerseWindowDelegate alloc] initWithWindowData:&m_Data];
    [NSApp setDelegate:delegate];
    
    // Programmatically create the top Mac Menu bar with a Quit option[cite: 17]
    NSMenu* menubar = [[NSMenu alloc] init];
    NSMenuItem* appMenuItem = [[NSMenuItem alloc] init];
    [menubar addItem:appMenuItem];
    [NSApp setMainMenu:menubar];
    
    NSMenu* appMenu = [[NSMenu alloc] init];
    NSString* quitTitle = [NSString stringWithFormat:@"Quit %@", [[NSProcessInfo processInfo] processName]];
    NSMenuItem* quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle
                                                          action:@selector(onQuitRequested:)
                                                   keyEquivalent:@"q"];
    [quitMenuItem setTarget:delegate];
    [appMenu addItem:quitMenuItem];
    [appMenuItem setSubmenu:appMenu];
    
    // Define the window bounds and visual style (resizable, closable, titled)[cite: 17]
    const NSRect frame = NSMakeRect(0, 0, m_Data.Width, m_Data.Height);
    const NSWindowStyleMask style = NSWindowStyleMaskTitled |
    NSWindowStyleMaskClosable |
    NSWindowStyleMaskMiniaturizable |
    NSWindowStyleMaskResizable;
    
    NSWindow* window = [[NSWindow alloc] initWithContentRect:frame
                                                   styleMask:style
                                                     backing:NSBackingStoreBuffered
                                                       defer:NO];
    window.title = [NSString stringWithUTF8String:m_Data.Title.c_str()];
    [window center];
    window.delegate = delegate;
    
    // Request default GPU and create our custom drawing canvas view[cite: 17]
    id<MTLDevice> defaultDevice = MTLCreateSystemDefaultDevice();
    KanVerseMetalView* metalView = [[KanVerseMetalView alloc] initWithFrame:frame
                                                                     device:defaultDevice
                                                                 windowData:&m_Data];
    
    // Configure the Metal render surface formats (Color and Depth)[cite: 17]
    metalView.colorPixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
    metalView.depthStencilPixelFormat = MTLPixelFormatDepth32Float;
    metalView.framebufferOnly = NO; // Allows shaders to write to or read from the surface if needed[cite: 17]
    metalView.paused = NO;
    metalView.enableSetNeedsDisplay = NO; // Engine drives drawing, not the OS UI system[cite: 17]
    metalView.preferredFramesPerSecond = m_Data.VSync ? 120 : 0; // Lock to 120Hz or run uncapped[cite: 17]
    metalView.clearColor = MTLClearColorMake(0.06, 0.07, 0.09, 1.0);
    
    window.contentView = metalView;
    
    // Finalize app launch and bring window to the front[cite: 17]
    [NSApp finishLaunching];
    [window makeKeyAndOrderFront:nil];
    
    if (m_Data.Maximized) {
      [window zoom:nil]; // Simulate clicking the green maximize button[cite: 17]
    }
    
    // Perform an initial calculation of the true physical Retina pixel dimensions[cite: 17]
    NSRect initialPixelBounds = [window.contentView convertRectToBacking:[window.contentView bounds]];
    m_Data.Width = static_cast<uint32_t>(initialPixelBounds.size.width);
    m_Data.Height = static_cast<uint32_t>(initialPixelBounds.size.height);
    metalView.drawableSize = initialPixelBounds.size; // Match metal texture size to physical pixels[cite: 17]
    
    [window makeMainWindow];
    [window orderFrontRegardless];
    [NSApp activateIgnoringOtherApps:YES]; // Force focus onto the game window[cite: 17]
    
    // Convert Objective-C object pointers to raw C++ void* pointers without destroying them (ARC escape)[cite: 17]
    m_WindowHandle = (__bridge_retained void*)window;
    m_ViewHandle = (__bridge_retained void*)metalView;
    m_DelegateHandle = (__bridge_retained void*)delegate;
  }
  
  void NativeWindow::Shutdown() {
    // Reverse the ARC escape: transfer void* back to Objective-C and set to nil to safely deallocate[cite: 17]
    if (m_DelegateHandle) {
      KanVerseWindowDelegate* delegate = (__bridge_transfer KanVerseWindowDelegate*)m_DelegateHandle;
      delegate = nil;
      m_DelegateHandle = nullptr;
    }
    
    if (m_ViewHandle) {
      KanVerseMetalView* view = (__bridge_transfer KanVerseMetalView*)m_ViewHandle;
      view = nil;
      m_ViewHandle = nullptr;
    }
    
    if (m_WindowHandle) {
      NSWindow* window = (__bridge_transfer NSWindow*)m_WindowHandle;
      [window close]; // Request OS to tear down the window UI[cite: 17]
      window = nil;
      m_WindowHandle = nullptr;
    }
  }
  
  // The per-frame event pump called by KanVerse's main run loop[cite: 17]
  void NativeWindow::OnUpdate() {
    // Provide a memory pool to clean up temporary OS objects created during event processing[cite: 17]
    @autoreleasepool {
      NSEvent* event = nil;
      
      // Drain the OS message queue of all pending hardware/OS events[cite: 17]
      while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                         untilDate:[NSDate distantPast]
                                            inMode:NSDefaultRunLoopMode
                                           dequeue:YES]) != nil) {
        
        // Specifically look for keypress events[cite: 17]
        if ([event type] == NSEventTypeKeyDown) {
          NSString* chars = [event charactersIgnoringModifiers];
          NSUInteger modifiers = [event modifierFlags] & NSEventModifierFlagDeviceIndependentFlagsMask;
          
          if ([chars isEqualToString:@"q"] && (modifiers == NSEventModifierFlagCommand)) {
            KV_CORE_INFO("Cmd+Q caught in event loop. Triggering WindowCloseEvent...");
            if (m_Data.EventCallback) {
              KanVerse::WindowCloseEvent closeEvent;
              m_Data.EventCallback(closeEvent);
            }
            return;
          }
          
          // 1. Update InputBridge state
          KanVerse::InputBridge::SetKeyState(event.keyCode, true);
          
          // 2. Dispatch C++ KeyPressedEvent to the engine layer stack
          if (m_Data.EventCallback) {
            KanVerse::KeyPressedEvent e(static_cast<int>(event.keyCode), [event isARepeat] ? 1 : 0);
            m_Data.EventCallback(e);
          }
          
        } else if ([event type] == NSEventTypeKeyUp) {
          KanVerse::InputBridge::SetKeyState(event.keyCode, false);
          
          // Dispatch C++ KeyReleasedEvent
          if (m_Data.EventCallback) {
            KanVerse::KeyReleasedEvent e(static_cast<int>(event.keyCode));
            m_Data.EventCallback(e);
          }
        } else if ([event type] == NSEventTypeFlagsChanged) {
          // Route modifier key changes (Shift, Ctrl, etc.) to the C++ InputBridge[cite: 17]
          const bool isShift = (event.modifierFlags & NSEventModifierFlagShift) != 0;
          const bool isCtrl  = (event.modifierFlags & NSEventModifierFlagControl) != 0;
          KanVerse::InputBridge::SetKeyState(56, isShift);
          KanVerse::InputBridge::SetKeyState(59, isCtrl);
        }
        
        // Pass the event back to the OS to ensure normal OS behaviors (like window dragging) still work[cite: 17]
        [NSApp sendEvent:event];
      }
      // Force macOS to flush UI updates[cite: 17]
      [NSApp updateWindows];
    }
  }
  
  // Updates CoreAnimation presentation settings to lock or unlock framerate[cite: 17]
  void NativeWindow::SetVSync(bool enabled) {
    m_Data.VSync = enabled;
    if (m_ViewHandle) {
      KanVerseMetalView* view = (__bridge KanVerseMetalView*)m_ViewHandle;
      view.preferredFramesPerSecond = enabled ? 120 : 0;
    }
  }
  
  // Triggers OS-level zoom (maximize) functionality[cite: 17]
  void NativeWindow::Maximize() {
    if (m_WindowHandle) {
      NSWindow* nsWindow = (__bridge NSWindow*)m_WindowHandle;
      if (![nsWindow isZoomed]) {
        [nsWindow zoom:nil];
      }
    }
  }
  
  // Checks if the window is currently occupying the full screen/maximized state[cite: 17]
  bool NativeWindow::IsMaximized() const noexcept {
    if (m_WindowHandle) {
      NSWindow* nsWindow = (__bridge NSWindow*)m_WindowHandle;
      return [nsWindow isZoomed];
    }
    return false;
  }
  
  // Returns the raw NSWindow pointer[cite: 17]
  void* NativeWindow::GetNativeWindow() const noexcept {
    return m_WindowHandle;
  }
  
  // Returns the raw MTKView pointer needed by the Renderer to access the swapchain[cite: 17]
  void* NativeWindow::GetNativeView() const noexcept {
    return m_ViewHandle;
  }
  
} // namespace KanVerse

