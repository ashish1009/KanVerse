/*!
 * @file ImGuiLayer.mm
 * @brief Objective-C++ implementation of ImGuiLayer bridging metal-cpp and Dear ImGui backends.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Renderer/UI/ImGuiLayer.hpp"
#include "Renderer/UI/ImGuiTheme.hpp"
#include "Core/Application/Application.hpp"
#include "Platform/Window/NativeWindow.hpp"
#include "Core/Diagnostics/Log.hpp"
#include "Core/Diagnostics/Assert.hpp"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

#import <AppKit/AppKit.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#import <imgui.h>
#import <imgui_impl_metal.h>
#import <imgui_impl_osx.h>

namespace KanVerse {
  
  /*
   * ImGuiLayer Implementation Notes:
   * --------------------------------
   * BridgesKanVerse's C++ `MTL::Device`, `MTL::CommandBuffer`, and `MTL::RenderCommandEncoder`
   * pointers to Objective-C `id<MTL...>` handles using zero-overhead `__bridge` casts.
   * `ImGui_ImplOSX_Init(mtkView)` automatically installs an `NSEvent` local monitor in
   * recent ImGui versions so mouse clicks, scroll wheels, and keystrokes are routed seamlessly.
   */
  
  ImGuiLayer::ImGuiLayer()
  : Layer("ImGuiLayer") {}
  
  void ImGuiLayer::OnAttach() {
    @autoreleasepool {
      KV_CORE_INFO("ImGuiLayer: Initializing Dear ImGui Studio context...");
      
      IMGUI_CHECKVERSION();
      ImGui::CreateContext();
      
      ImGuiIO& io = ImGui::GetIO();
      io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
      io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
      io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
      
      // Apply custom KanVerse Studio charcoal dark theme
//      ImGuiTheme::ApplyDarkTheme();
      
      auto& app = Application::Get();
      auto* nativeWin = static_cast<const NativeWindow*>(&app.GetWindow());
      MTKView* mtkView = (__bridge MTKView*)nativeWin->GetNativeView();
      id<MTLDevice> device = (__bridge id<MTLDevice>)app.GetRenderContext()->GetDevice();
      
      KV_CORE_ASSERT(mtkView != nil, "[ImGuiLayer] Native MTKView is nil!");
      KV_CORE_ASSERT(device != nil,  "[ImGuiLayer] Native MTLDevice is nil!");
      
      ImGui_ImplOSX_Init(mtkView);
      ImGui_ImplMetal_Init(device);
      
      KV_CORE_INFO("ImGuiLayer: Dear ImGui Metal & OSX backends initialized.");
    }
  }
  
  void ImGuiLayer::OnDetach() {
    @autoreleasepool {
      ImGui_ImplMetal_Shutdown();
      ImGui_ImplOSX_Shutdown();
      ImGui::DestroyContext();
      KV_CORE_INFO("ImGuiLayer: Dear ImGui context destroyed.");
    }
  }
  
  void ImGuiLayer::OnEvent(Event& event) {
    if (!m_BlockEvents) {
      return;
    }
    
    ImGuiIO& io = ImGui::GetIO();
    if (event.IsInCategory(EventCategoryMouse) && io.WantCaptureMouse) {
      event.SetHandled(true);
    }
    if (event.IsInCategory(EventCategoryKeyboard) && io.WantCaptureKeyboard) {
      event.SetHandled(true);
    }
  }
  
  void ImGuiLayer::Begin(MTL::RenderPassDescriptor* passDescriptor) {
    @autoreleasepool {
      auto& app = Application::Get();
      auto* nativeWin = static_cast<const NativeWindow*>(&app.GetWindow());
      MTKView* mtkView = (__bridge MTKView*)nativeWin->GetNativeView();
      MTLRenderPassDescriptor* objcPassDesc = (__bridge MTLRenderPassDescriptor*)passDescriptor;
      
      if (!objcPassDesc && mtkView) {
        objcPassDesc = mtkView.currentRenderPassDescriptor;
      }
      
      ImGui_ImplMetal_NewFrame(objcPassDesc);
      ImGui_ImplOSX_NewFrame(mtkView);
      ImGui::NewFrame();
    }
  }
  
  void ImGuiLayer::End(MTL::CommandBuffer* commandBuffer, MTL::RenderCommandEncoder* renderEncoder) {
    @autoreleasepool {
      ImGui::Render();
      
      if (commandBuffer && renderEncoder) {
        id<MTLCommandBuffer> objcCmdBuf = (__bridge id<MTLCommandBuffer>)commandBuffer;
        id<MTLRenderCommandEncoder> objcEncoder = (__bridge id<MTLRenderCommandEncoder>)renderEncoder;
        
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), objcCmdBuf, objcEncoder);
      }
      
      ImGuiIO& io = ImGui::GetIO();
      if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
      }
    }
  }
  
} // namespace KanVerse
