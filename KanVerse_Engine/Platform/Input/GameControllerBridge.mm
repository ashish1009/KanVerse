/*!
 * @file GameControllerBridge.mm
 * @brief Implementation of hardware OS bridge using GameController.framework.
 * @copyright Copyright (c) 2026 KanVerse Contributors. All rights reserved.
 * @license MIT License
 */

#include "Platform/Input/GameControllerBridge.hpp"
#include "Core/Diagnostics/Log.hpp"

#import <GameController/GameController.h>
#import <AppKit/AppKit.h>
#include <unordered_set>

#pragma comment(lib, "GameController")

namespace KanVerse {
  
  // Static hash set tracking raw macOS virtual keycodes that are currently depressed
  static std::unordered_set<uint16_t> s_ActiveKeys;
  
  void InputBridge::Init() {
    // Instructs macOS to begin background scanning for MFi, Xbox, and PlayStation controllers
    [GCController startWirelessControllerDiscoveryWithCompletionHandler:^{
      KV_CORE_INFO("GameController: Wireless discovery scan complete.");
    }];
  }
  
  void InputBridge::Shutdown() {
    [GCController stopWirelessControllerDiscovery];
    s_ActiveKeys.clear();
  }
  
  void InputBridge::SetKeyState(uint16_t keyCode, bool isPressed) {
    if (isPressed) {
      s_ActiveKeys.insert(keyCode);
    } else {
      s_ActiveKeys.erase(keyCode);
    }
  }
  
  bool InputBridge::IsKeyPressed(uint16_t keyCode) {
    return s_ActiveKeys.find(keyCode) != s_ActiveKeys.end();
  }
  
  GamepadInputState InputBridge::PollInput() {
    GamepadInputState state{};
    
    // Query macOS for any actively paired controllers
    NSArray<GCController*>* controllers = [GCController controllers];
    if (controllers.count > 0) {
      GCController* controller = controllers.firstObject;
      GCExtendedGamepad* profile = controller.extendedGamepad;
      
      if (profile) {
        state.Connected = true;
        
        // Map raw analog stick values (Normalized from -1.0 to 1.0)
        state.LeftStick.x = static_cast<float>(profile.leftThumbstick.xAxis.value);
        state.LeftStick.y = static_cast<float>(profile.leftThumbstick.yAxis.value);
        state.RightStick.x = static_cast<float>(profile.rightThumbstick.xAxis.value);
        state.RightStick.y = static_cast<float>(profile.rightThumbstick.yAxis.value);
        
        // Map raw hardware face buttons
        state.ButtonSouth = profile.buttonA.isPressed;
        state.ButtonEast  = profile.buttonB.isPressed;
        state.ButtonWest  = profile.buttonX.isPressed;
        state.ButtonNorth = profile.buttonY.isPressed;
        state.LeftTrigger  = profile.leftTrigger.isPressed;
        state.RightTrigger = profile.rightTrigger.isPressed;
        
        // Apply a strict 15% radial deadzone to the hardware sticks to prevent analog drift
        if (simd_length(state.LeftStick) < 0.15f) {
          state.LeftStick = simd_make_float2(0.0f, 0.0f);
        }
      }
    }
    
    // Note: No WASD fallback logic here. The engine merely reports the raw hardware state.
    return state;
  }
} // namespace KanVerse

